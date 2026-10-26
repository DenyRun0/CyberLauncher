#include "mainwindow.h"
#include "i18n.h"
#include "ui_mainwindow.h"

#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QFile>
#include <QFont>
#include <QIcon>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPainter>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QResizeEvent>
#include <QSignalBlocker>
#include <QStringList>
#include <QUrl>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

// Роль елемента списку: true, якщо замість справжньої іконки поки що стоїть заглушка
static constexpr int kPlaceholderRole = Qt::UserRole + 1;
// Скільки додаткових сторінок (по 20 позицій) підвантажувати автоматично після пошуку.
// 2 => одразу видно 60 позицій; далі кнопка «Показати ще».
static constexpr int kAutoPages = 2;

static fs::path configPath() { return core::rootDir() / "config.json"; }

static QString T(i18n::Key k) { return i18n::tr(k); }
static std::string TS(i18n::Key k) { return i18n::tr(k).toStdString(); }
static std::string TSA(i18n::Key k, const QString& a) { return i18n::tr(k).arg(a).toStdString(); }

static QString formatDuration(qint64 secs) {
    using namespace i18n;
    if (secs <= 0) return T(DurZero);
    if (secs < 60) return T(DurLess);
    const qint64 h = secs / 3600, m = (secs % 3600) / 60;
    return h > 0 ? T(DurHM).arg(h).arg(m) : T(DurM).arg(m);
}

static QString formatCount(long long n) {
    using namespace i18n;
    if (n >= 1000000) return QString::number(n / 1000000.0, 'f', 1) + " " + T(CountM);
    if (n >= 1000) return QString::number(n / 1000.0, 'f', 1) + " " + T(CountK);
    return QString::number(n);
}

// Гарантована «фотографія» для кожної позиції: кольоровий квадрат із першою літерою назви.
// Замінюється справжньою іконкою/обкладинкою з Modrinth, щойно вона завантажиться.
static QIcon placeholderIcon(const QString& title) {
    const int S = 112;
    QPixmap pm(S, S);
    pm.fill(Qt::transparent);
    uint h = 0;
    for (QChar c : title) h = h * 31 + c.unicode();
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor::fromHsl(static_cast<int>(h % 360), 140, 70));
    p.drawRoundedRect(0, 0, S, S, 22, 22);
    const QString t = title.trimmed();
    QFont f;
    f.setBold(true);
    f.setPixelSize(60);
    p.setFont(f);
    p.setPen(QColor(255, 255, 255, 235));
    p.drawText(QRect(0, 0, S, S), Qt::AlignCenter, t.isEmpty() ? QString("?") : t.left(1).toUpper());
    p.end();
    return QIcon(pm);
}

static void setBoxItems(QComboBox* b, const std::vector<std::pair<QString, QString>>& items) {
    QSignalBlocker blk(b);  // зміна підписів не має запускати новий пошук
    const int keep = std::max(0, b->currentIndex());
    b->clear();
    for (const auto& it : items) b->addItem(it.first, it.second);
    if (!items.empty()) b->setCurrentIndex(std::min<int>(keep, static_cast<int>(items.size()) - 1));
}

// ---------- Склад збірки: назви jar -> проєкти Modrinth -> іконки ----------

static std::string normName(const std::string& s) {
    std::string r;
    for (unsigned char c : s)
        if (std::isalnum(c)) r.push_back(static_cast<char>(std::tolower(c)));
    return r;
}

// "sodium-fabric-0.5.8+mc1.20.4.jar" -> "sodium";  "fabric-api-0.97.0+1.20.4.jar" -> "fabric api"
static std::string jarQuery(const std::string& file) {
    std::string stem = file.size() > 4 ? file.substr(0, file.size() - 4) : file;
    std::vector<std::string> tokens;
    std::string cur;
    for (char c : stem) {
        if (c == '-' || c == '_' || c == '+' || c == ' ' || c == '[' || c == ']' || c == '(' || c == ')') {
            if (!cur.empty()) tokens.push_back(cur), cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    std::string q;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string low = tokens[i];
        std::transform(low.begin(), low.end(), low.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        const bool numeric = std::isdigit(static_cast<unsigned char>(low[0])) ||
                             (low.size() > 1 && low[0] == 'v' && std::isdigit(static_cast<unsigned char>(low[1]))) ||
                             (low.size() > 2 && low.compare(0, 2, "mc") == 0 &&
                              std::isdigit(static_cast<unsigned char>(low[2])));
        if (numeric && i > 0) break;  // далі йде версія
        if (i > 0 && (low == "fabric" || low == "quilt" || low == "forge" || low == "neoforge")) continue;
        if (!q.empty()) q += ' ';
        q += tokens[i];
    }
    return q;
}

// Обираємо результат, назва якого справді відповідає запиту (щоб не показати чужу іконку)
static int pickHit(const std::vector<core::ModHit>& r, const std::string& query) {
    const std::string nq = normName(query);
    if (nq.empty()) return -1;
    for (size_t i = 0; i < r.size(); ++i)
        if (normName(r[i].title) == nq) return static_cast<int>(i);
    for (size_t i = 0; i < r.size(); ++i) {
        const std::string nt = normName(r[i].title);
        if (nt.size() >= 3 && (nt.find(nq) != std::string::npos ||
                               (nt.size() >= 4 && nq.find(nt) != std::string::npos)))
            return static_cast<int>(i);
    }
    return -1;
}

static void packWorker(Shared* s, int gen, std::string slug) {
    auto alive = [s, gen] { return s->pack.generation.load() == gen; };
    auto cb = [s](const std::string& id, const std::string& path) {
        std::lock_guard<std::mutex> lk(s->im);
        s->icons[id] = path;
        s->iconsDirty = true;
    };
    try {
        core::Profile prof;
        if (!core::loadProfile(slug, prof)) return;

        std::vector<std::string> files;
        std::error_code ec;
        const fs::path dir = prof.dir / "mods";
        if (fs::exists(dir, ec))
            for (const auto& e : fs::directory_iterator(dir, ec))
                if (e.is_regular_file() && e.path().extension() == ".jar")
                    files.push_back(e.path().filename().string());
        std::sort(files.begin(), files.end());
        {
            std::lock_guard<std::mutex> lk(s->pack.m);
            if (!alive()) return;
            s->pack.slug = slug;
            s->pack.files = files;
            s->pack.hitIndex.assign(files.size(), -1);
            s->pack.hits.clear();
            s->pack.coverId.clear();
        }
        s->pack.dirty = true;

        // Обкладинка самої збірки
        try {
            core::SearchParams p;
            p.projectType = "modpack";
            p.loader = "fabric";
            p.query = prof.name;
            p.index = "relevance";
            auto r = core::searchProjects(p);
            const std::string nn = normName(prof.name);
            for (const auto& h : r) {
                if (nn.empty() || normName(h.title) != nn) continue;
                {
                    std::lock_guard<std::mutex> lk(s->pack.m);
                    if (!alive()) return;
                    s->pack.coverId = h.projectId;
                }
                s->pack.dirty = true;
                std::vector<core::ModHit> one{h};
                core::fetchIcons(one, cb);
                break;
            }
        } catch (...) {
        }

        // Іконка кожного мода збірки
        for (size_t i = 0; i < files.size(); ++i) {
            if (!alive()) return;
            const std::string q = jarQuery(files[i]);
            if (q.empty()) continue;
            try {
                core::SearchParams p;
                p.projectType = "mod";
                p.loader = "fabric";
                p.versions = {prof.mcVersion};
                p.query = q;
                p.index = "relevance";
                auto r = core::searchProjects(p);
                const int k = pickHit(r, q);
                if (k < 0) continue;
                {
                    std::lock_guard<std::mutex> lk(s->pack.m);
                    if (!alive()) return;
                    s->pack.hits.push_back(r[k]);
                    s->pack.hitIndex[i] = static_cast<int>(s->pack.hits.size()) - 1;
                }
                s->pack.dirty = true;
                bool have;
                {
                    std::lock_guard<std::mutex> lk(s->im);
                    have = s->icons.count(r[k].projectId) > 0;
                }
                if (!have) {
                    std::vector<core::ModHit> one{r[k]};
                    core::fetchIcons(one, cb);
                }
            } catch (...) {
            }
        }

        std::lock_guard<std::mutex> lk(s->pack.m);
        if (alive() && s->pack.slug == slug) {
            PackView::Cached c;
            c.files = s->pack.files;
            c.hitIndex = s->pack.hitIndex;
            c.hits = s->pack.hits;
            c.coverId = s->pack.coverId;
            s->pack.cache[slug] = std::move(c);
        }
    } catch (...) {
    }
}

// ---------- Вікно ----------

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), sh(*new Shared) {
    ui->setupUi(this);

    // У нікнеймі лише латиниця, цифри та підкреслення
    ui->nickEdit->setValidator(
        new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9_]{0,16}"), this));
    ui->nickEdit->setText("Player");

    ui->modResults->setIconSize(QSize(56, 56));
    ui->packResults->setIconSize(QSize(56, 56));
    ui->packModsList->setIconSize(QSize(32, 32));

    // Мови. Назви мов пишемо рідною мовою, їх не перекладаємо.
    {
        QSignalBlocker blk(ui->languageBox);
        ui->languageBox->addItem(QString::fromUtf8("Українська"), QString("uk"));
        ui->languageBox->addItem(QString("English"), QString("en"));
    }

    // Фон: якщо поряд із проєктом лежить background.jpg, беремо його, інакше вбудований bg.jpg
    const QString customBg =
        QString::fromStdString((core::projectDir() / "background.jpg").string());
    if (!bgSource.load(customBg)) bgSource.load(":/bg.jpg");
    rebuildBackground();

    loadConfig();
    retranslateUi();  // підписи, категорії, сортування; індекси з конфігу зберігаються

    // Кнопки лівої панелі перемикають сторінки
    connect(ui->navPlay, &QToolButton::clicked, this, [this] { ui->stack->setCurrentIndex(0); });
    connect(ui->navMods, &QToolButton::clicked, this, [this] { ui->stack->setCurrentIndex(1); });
    connect(ui->navPacks, &QToolButton::clicked, this, [this] { ui->stack->setCurrentIndex(2); });
    connect(ui->navSettings, &QToolButton::clicked, this, [this] { ui->stack->setCurrentIndex(3); });
    connect(ui->navLog, &QToolButton::clicked, this, [this] { ui->stack->setCurrentIndex(4); });

    // Прев'ю збірки: з каталогу (обкладинка) або встановленої (обкладинка + моди з іконками)
    connect(ui->packResults, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* cur) {
        if (!cur) return;
        previewInstalled = false;
        ++sh.pack.generation;  // зупиняє пошук іконок для попередньої збірки
        ui->packModsList->clear();
        updatePackPreview();
    });
    connect(ui->installedPacks, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* cur) {
        ui->packModsList->clear();
        if (!cur) {
            previewInstalled = false;
            ++sh.pack.generation;
        } else {
            previewInstalled = true;
            startPackResolve(cur->data(Qt::UserRole).toString().toStdString());
        }
        updatePackPreview();
    });

    ui->pathLabel->setText(QString::fromStdString(core::rootDir().string()));

    connect(&timer, &QTimer::timeout, this, &MainWindow::pollStatus);
    timer.start(100);

    loadVersionsAsync();
}

MainWindow::~MainWindow() {
    saveConfig();
    core::g_cancel = true;
    if (worker.joinable()) worker.join();
    delete ui;
}

QString MainWindow::currentVersionId() const { return ui->versionBox->currentData().toString(); }

core::Loader MainWindow::currentLoader() const {
    if (currentVersionId().startsWith("pack:")) return core::Loader::Fabric;
    return ui->loaderBox->currentIndex() == 1 ? core::Loader::Fabric : core::Loader::Vanilla;
}

// Куди ставити моди для обраної версії або збірки
bool MainWindow::currentTarget(Target& t) const {
    const QString id = currentVersionId();
    if (id.isEmpty()) return false;
    if (id.startsWith("pack:")) {
        core::Profile p;
        if (!core::loadProfile(id.mid(5).toStdString(), p)) return false;
        t.mc = p.mcVersion;
        t.loaderVersion = p.loaderVersion;
        t.dir = p.dir;
        t.isPack = true;
        t.packName = p.name;
        return true;
    }
    t.mc = id.toStdString();
    t.dir = core::instanceDir(t.mc, core::Loader::Fabric);
    return true;
}

// ---------- Мова ----------

// Список категорій і сортувань: підпис залежить від мови, «слаг» для Modrinth лишається незмінним
void MainWindow::fillCombos() {
    using namespace i18n;
    setBoxItems(ui->loaderBox, {{T(LoaderVanilla), QString()}, {T(LoaderFabric), QString()}});
    setBoxItems(ui->modCategoryBox, {
        {T(CatAll), ""},
        {T(CatOptimization), "optimization"}, {T(CatUtility), "utility"},
        {T(CatAdventure), "adventure"},       {T(CatMagic), "magic"},
        {T(CatTechnology), "technology"},     {T(CatDecoration), "decoration"},
        {T(CatWorldgen), "worldgen"},         {T(CatLibrary), "library"},
        {T(CatMechanics), "game-mechanics"},  {T(CatMobs), "mobs"},
        {T(CatEquipment), "equipment"},       {T(CatFood), "food"},
        {T(CatStorage), "storage"},           {T(CatTransport), "transportation"},
        {T(CatManagement), "management"},     {T(CatEconomy), "economy"},
        {T(CatSocial), "social"},             {T(CatMinigame), "minigame"},
        {T(CatCursed), "cursed"}});
    const std::vector<std::pair<QString, QString>> sorts = {
        {T(SortPopular), "downloads"}, {T(SortRelevance), "relevance"},
        {T(SortNewest), "newest"},     {T(SortUpdated), "updated"}};
    setBoxItems(ui->modSortBox, sorts);
    setBoxItems(ui->packSortBox, sorts);
}

void MainWindow::retranslateUi() {
    using namespace i18n;
    // Бокова панель
    ui->navPlay->setText(T(NavPlay));         ui->navPlay->setToolTip(T(NavPlay));
    ui->navMods->setText(T(NavMods));         ui->navMods->setToolTip(T(NavMods));
    ui->navPacks->setText(T(NavPacks));       ui->navPacks->setToolTip(T(NavPacks));
    ui->navSettings->setText(T(NavSettings)); ui->navSettings->setToolTip(T(NavSettings));
    ui->navLog->setText(T(NavLog));           ui->navLog->setToolTip(T(NavLog));

    // Головна
    ui->nickCaption->setText(T(NickCaption));
    ui->chipJava->setText(T(ChipJava));
    ui->versionCaption->setText(T(VersionCaption));
    ui->loaderCaption->setText(T(LoaderCaption));
    fillCombos();

    // Моди
    ui->modsTitle->setText(T(NavMods));
    ui->modSearchEdit->setPlaceholderText(T(ModSearchPlaceholder));
    ui->modSearchButton->setText(T(Search));
    ui->modResultsCaption->setText(T(ModCatalogCaption));
    ui->modMoreButton->setText(T(ShowMore));
    ui->modInstallButton->setText(T(Install));
    ui->modInstalledCaption->setText(T(ModInstalledCaption));
    ui->modRemoveButton->setText(T(RemoveSelected));
    updateModsHint();

    // Збірки
    ui->packsTitle->setText(T(NavPacks));
    ui->packsHint->setText(T(PacksHint));
    ui->packSearchEdit->setPlaceholderText(T(PackSearchPlaceholder));
    ui->packSearchButton->setText(T(Search));
    ui->packResultsCaption->setText(T(PackCatalogCaption));
    ui->packMoreButton->setText(T(ShowMore));
    ui->packInstallButton->setText(T(InstallPack));
    ui->packInstalledCaption->setText(T(PackInstalledCaption));
    ui->packRemoveButton->setText(T(RemovePack));
    ui->packModsCaption->setText(T(PackModsCaption));
    updatePackPreview();

    // Налаштування та журнал
    ui->settingsTitle->setText(T(NavSettings));
    ui->ramCaption->setText(T(RamCaption));
    ui->ramHint->setText(T(RamHint));
    ui->versionsCaption->setText(T(VersionsCaption));
    ui->snapshotsCheck->setText(T(Snapshots));
    ui->refreshButton->setText(T(RefreshList));
    ui->filesCaption->setText(T(FilesCaption));
    ui->openFolderButton->setText(T(OpenFolder));
    ui->recentCaption->setText(T(RecentCaption));
    ui->languageCaption->setText(T(LangCaption));
    ui->languageHint->setText(T(LangHint));
    ui->logTitle->setText(T(NavLog));
    // Решту (чіпи, кнопка «ГРАТИ», статуси) pollStatus() переписує сам кожні 100 мс
}

void MainWindow::updateModsHint() {
    using namespace i18n;
    Target t;
    if (!currentTarget(t)) {
        ui->modsHint->setText(T(HintLoading));
        return;
    }
    ui->modsHint->setText(
        t.isPack ? T(HintPack).arg(QString::fromStdString(t.packName), QString::fromStdString(t.mc))
                 : T(HintFabric).arg(QString::fromStdString(t.mc)));
}

void MainWindow::on_languageBox_currentIndexChanged(int index) {
    if (index < 0) return;
    i18n::setLang(i18n::fromCode(ui->languageBox->itemData(index).toString()));
    retranslateUi();
    saveConfig();
}

// ---------- Налаштування ----------

void MainWindow::loadConfig() {
    try {
        std::ifstream f(configPath());
        if (!f) return;
        auto j = core::json::parse(f);
        ui->nickEdit->setText(QString::fromStdString(j.value("nick", std::string("Player"))));
        wantedVersion = QString::fromStdString(j.value("version", std::string()));
        ui->snapshotsCheck->setChecked(j.value("snapshots", false));
        ui->ramSpin->setValue(j.value("ramGb", 2));
        ui->loaderBox->setCurrentIndex(j.value("loader", 0));
        playSeconds = static_cast<qint64>(j.value("playSeconds", 0LL));
        lastPlayedEpoch = static_cast<qint64>(j.value("lastPlayed", 0LL));
        lastPlayedVersion = QString::fromStdString(j.value("lastPlayedVersion", std::string()));

        i18n::setLang(i18n::fromCode(QString::fromStdString(j.value("lang", std::string("uk")))));
        QSignalBlocker blk(ui->languageBox);
        ui->languageBox->setCurrentIndex(i18n::lang() == i18n::Lang::En ? 1 : 0);
    } catch (...) {
    }
}

void MainWindow::saveConfig() {
    try {
        QString ver = currentVersionId();
        if (ver.isEmpty()) ver = wantedVersion;
        core::json j = {{"nick", ui->nickEdit->text().toStdString()},
                        {"version", ver.toStdString()},
                        {"snapshots", ui->snapshotsCheck->isChecked()},
                        {"ramGb", ui->ramSpin->value()},
                        {"loader", ui->loaderBox->currentIndex()},
                        {"lang", std::string(i18n::code(i18n::lang()))},
                        {"playSeconds", static_cast<long long>(playSeconds)},
                        {"lastPlayed", static_cast<long long>(lastPlayedEpoch)},
                        {"lastPlayedVersion", lastPlayedVersion.toStdString()}};
        fs::create_directories(configPath().parent_path());
        std::ofstream(configPath()) << j.dump(2);
    } catch (...) {
    }
}

// ---------- Версії та збірки у списку ----------

void MainWindow::loadVersionsAsync() {
    sh.state = 0;
    Shared* s = &sh;
    std::thread([s] {
        try {
            auto v = core::fetchVersions();
            std::lock_guard<std::mutex> lk(s->m);
            s->versions = std::move(v);
            s->state = 1;
        } catch (const std::exception& e) {
            s->st.log(TSA(i18n::VersionsFail, QString::fromUtf8(e.what())));
            s->state = 2;
        }
    }).detach();
}

void MainWindow::fillVersionBox(const QString& forceId) {
    QString keep = currentVersionId();
    if (keep.isEmpty()) keep = wantedVersion;
    if (!forceId.isEmpty()) keep = forceId;

    ui->versionBox->clear();
    const bool snaps = ui->snapshotsCheck->isChecked();
    int select = -1, firstRelease = -1;

    // Встановлені збірки йдуть першими: це «модові версії»
    for (const auto& p : core::listProfiles()) {
        const QString id = "pack:" + QString::fromStdString(p.slug);
        ui->versionBox->addItem(QString::fromUtf8("★ ") + QString::fromStdString(p.name) + " (" +
                                    QString::fromStdString(p.mcVersion) + ")",
                                id);
        if (id == keep) select = ui->versionBox->count() - 1;
    }
    {
        std::lock_guard<std::mutex> lk(sh.m);
        for (const auto& v : sh.versions) {
            if (!snaps && v.type != "release") continue;
            QString id = QString::fromStdString(v.id);
            QString label = id;
            if (v.type != "release") label += "  (" + QString::fromStdString(v.type) + ")";
            ui->versionBox->addItem(label, id);
            int idx = ui->versionBox->count() - 1;
            if (firstRelease < 0 && v.type == "release") firstRelease = idx;
            if (id == keep) select = idx;
        }
    }
    if (select < 0) select = firstRelease >= 0 ? firstRelease : (ui->versionBox->count() > 0 ? 0 : -1);
    ui->versionBox->setCurrentIndex(select);
}

// ---------- Кнопки (Qt підключає їх за назвою) ----------

void MainWindow::on_snapshotsCheck_toggled(bool) {
    if (versionsShown) fillVersionBox();
}

void MainWindow::on_refreshButton_clicked() {
    versionsShown = false;
    loadVersionsAsync();
}

void MainWindow::on_openFolderButton_clicked() {
    fs::create_directories(core::rootDir());
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(core::rootDir().string())));
}

void MainWindow::on_playButton_clicked() {
    const QString nick = ui->nickEdit->text().trimmed();
    const QString id = currentVersionId();
    if (nick.isEmpty() || id.isEmpty()) return;
    saveConfig();

    std::string mcId = id.toStdString();
    core::Loader loader = currentLoader();
    fs::path instanceOverride;
    std::string loaderVersion;
    if (id.startsWith("pack:")) {
        core::Profile p;
        if (!core::loadProfile(id.mid(5).toStdString(), p)) {
            sh.st.log(TS(i18n::PackNotFound));
            return;
        }
        mcId = p.mcVersion;
        instanceOverride = p.dir;
        loaderVersion = p.loaderVersion;
        loader = core::Loader::Fabric;
        launchedVersion = QString::fromStdString(p.name);
    } else {
        launchedVersion = id + (loader == core::Loader::Fabric ? " Fabric" : "");
    }

    core::VersionInfo chosen;
    {
        std::lock_guard<std::mutex> lk(sh.m);
        for (const auto& v : sh.versions)
            if (v.id == mcId) chosen = v;
    }
    if (chosen.id.empty()) {
        sh.st.log(TSA(i18n::VersionNotFound, QString::fromStdString(mcId)));
        return;
    }

    if (worker.joinable()) worker.join();
    sh.st.busy = true;
    sh.st.progress = 0.f;

    Shared* s = &sh;
    const std::string nickStd = nick.toStdString();
    const int ram = ui->ramSpin->value();
    worker = std::thread([s, chosen, nickStd, ram, loader, instanceOverride, loaderVersion] {
        try {
            core::installAndRun(chosen, nickStd, ram, loader, s->st, instanceOverride, loaderVersion);
        } catch (const std::exception& e) {
            s->st.log(TSA(i18n::Error, QString::fromUtf8(e.what())));
        }
        s->st.busy = false;
    });
}

// ---------- Каталог: пошук ----------

void MainWindow::startSearch(Catalog& c, const core::SearchParams& p, bool append) {
    if (c.st.busy) return;
    c.st.busy = true;
    c.st.setText(TS(i18n::Searching));
    Shared* s = &sh;
    Catalog* cp = &c;
    std::thread([s, cp, p, append] {
        std::vector<core::ModHit> forIcons;
        try {
            auto hits = core::searchProjects(p);
            forIcons = hits;
            const size_t n = hits.size();
            {
                std::lock_guard<std::mutex> lk(cp->m);
                cp->hits = std::move(hits);
                cp->append = append;
            }
            if (n == 0)
                cp->st.setText(TS(append ? i18n::AllResults : i18n::NothingFound));
            else
                cp->st.setText(i18n::tr(append ? i18n::Added : i18n::Found)
                                   .arg(static_cast<qulonglong>(n))
                                   .toStdString());
            cp->ready = true;
        } catch (const std::exception& e) {
            cp->st.setText(TSA(i18n::Error, QString::fromUtf8(e.what())));
        }
        cp->st.busy = false;
        try {
            core::fetchIcons(forIcons, [s](const std::string& id, const std::string& path) {
                std::lock_guard<std::mutex> lk(s->im);
                s->icons[id] = path;
                s->iconsDirty = true;
            });
        } catch (...) {
        }
    }).detach();
}

void MainWindow::runModSearch(bool append) {
    Target t;
    if (!currentTarget(t)) return;
    core::SearchParams p;
    p.projectType = "mod";
    p.loader = "fabric";
    p.versions = {t.mc};
    p.query = ui->modSearchEdit->text().trimmed().toStdString();
    p.category = ui->modCategoryBox->currentData().toString().toStdString();
    p.index = ui->modSortBox->currentData().toString().toStdString();
    p.offset = append ? ui->modResults->count() : 0;
    if (!append) modAutoLeft = kAutoPages;
    startSearch(sh.mods, p, append);
}

void MainWindow::runPackSearch(bool append) {
    core::SearchParams p;
    p.projectType = "modpack";
    p.loader = "fabric";
    {
        std::lock_guard<std::mutex> lk(sh.m);
        for (const auto& v : sh.versions)
            if (v.type == "release") p.versions.push_back(v.id);
    }
    p.query = ui->packSearchEdit->text().trimmed().toStdString();
    p.index = ui->packSortBox->currentData().toString().toStdString();
    p.offset = append ? ui->packResults->count() : 0;
    if (!append) packAutoLeft = kAutoPages;
    startSearch(sh.packs, p, append);
}

void MainWindow::on_modSearchButton_clicked() { runModSearch(false); }
void MainWindow::on_modSearchEdit_returnPressed() { runModSearch(false); }
void MainWindow::on_modCategoryBox_currentIndexChanged(int) { if (modsRequested) runModSearch(false); }
void MainWindow::on_modSortBox_currentIndexChanged(int) { if (modsRequested) runModSearch(false); }
void MainWindow::on_modMoreButton_clicked() { runModSearch(true); }

void MainWindow::on_packSearchButton_clicked() { runPackSearch(false); }
void MainWindow::on_packSearchEdit_returnPressed() { runPackSearch(false); }
void MainWindow::on_packSortBox_currentIndexChanged(int) { if (packsRequested) runPackSearch(false); }
void MainWindow::on_packMoreButton_clicked() { runPackSearch(true); }

// Показує результати пошуку у списку. Повертає true, якщо список змінився.
// Кожна позиція одразу отримує іконку-заглушку; справжню картинку підставляє applyIcons().
bool MainWindow::fillCatalog(Catalog& c, QListWidget* list, bool& hasMore) {
    if (!c.ready.exchange(false)) return false;
    std::lock_guard<std::mutex> lk(c.m);
    if (!c.append) list->clear();
    for (const auto& h : c.hits) {
        const QString title = QString::fromStdString(h.title);
        QString desc = QString::fromStdString(h.description);
        if (desc.size() > 120) desc = desc.left(117) + "...";
        auto* it = new QListWidgetItem(placeholderIcon(title),
                                       title + "\n" + desc + "\n" +
                                           T(i18n::Downloads).arg(formatCount(h.downloads)));
        it->setData(Qt::UserRole, QString::fromStdString(h.projectId));
        it->setData(kPlaceholderRole, true);
        it->setSizeHint(QSize(0, 84));
        list->addItem(it);
    }
    hasMore = c.hits.size() >= 20;
    return true;
}

// Підставляє завантажені іконки у списки (замінює заглушки)
void MainWindow::applyIcons(QListWidget* list) {
    std::lock_guard<std::mutex> lk(sh.im);
    for (int i = 0; i < list->count(); ++i) {
        QListWidgetItem* it = list->item(i);
        if (!it->data(kPlaceholderRole).toBool() && !it->icon().isNull()) continue;  // вже справжня
        auto f = sh.icons.find(it->data(Qt::UserRole).toString().toStdString());
        if (f == sh.icons.end()) continue;
        QPixmap pm(QString::fromStdString(f->second));
        if (pm.isNull()) continue;
        it->setIcon(QIcon(pm));
        it->setData(kPlaceholderRole, false);
    }
}

// Коли відкрито сторінку каталогу, одразу показуємо найпопулярніше
void MainWindow::ensureCatalogs() {
    if (sh.state != 1) return;
    const int page = ui->stack->currentIndex();
    if (page == 1 && !modsRequested && !sh.mods.st.busy) {
        Target t;
        if (currentTarget(t)) {
            modsRequested = true;
            runModSearch(false);
        }
    }
    if (page == 2 && !packsRequested && !sh.packs.st.busy) {
        packsRequested = true;
        runPackSearch(false);
    }
}

// Після пошуку самі підвантажуємо ще kAutoPages сторінок, щоб каталог одразу був великим
void MainWindow::autoLoadMore() {
    if (modAutoLeft > 0 && !sh.mods.st.busy && !sh.mods.ready.load() && ui->modResults->count() > 0) {
        if (modHasMore) {
            --modAutoLeft;
            runModSearch(true);
        } else {
            modAutoLeft = 0;
        }
    }
    if (packAutoLeft > 0 && !sh.packs.st.busy && !sh.packs.ready.load() && ui->packResults->count() > 0) {
        if (packHasMore) {
            --packAutoLeft;
            runPackSearch(true);
        } else {
            packAutoLeft = 0;
        }
    }
}

// ---------- Моди: встановлення та видалення ----------

void MainWindow::on_modInstallButton_clicked() {
    QListWidgetItem* item = ui->modResults->currentItem();
    Target t;
    if (!item || sh.mods.st.busy || !currentTarget(t)) return;
    const std::string projectId = item->data(Qt::UserRole).toString().toStdString();
    if (projectId.empty()) return;
    const fs::path modsDir = t.dir / "mods";
    const std::string mc = t.mc;

    sh.mods.st.busy = true;
    sh.mods.st.setText(TS(i18n::Installing));
    Shared* s = &sh;
    std::thread([s, projectId, mc, modsDir] {
        try {
            std::set<std::string> visited;
            core::installMod(projectId, mc, core::Loader::Fabric, modsDir, s->mods.st, visited);
            s->mods.st.setText(TS(i18n::ModInstalled));
            s->installedDirty = true;
        } catch (const std::exception& e) {
            s->mods.st.setText(TSA(i18n::Error, QString::fromUtf8(e.what())));
        }
        s->mods.st.busy = false;
    }).detach();
}

void MainWindow::on_modRemoveButton_clicked() {
    QListWidgetItem* item = ui->installedList->currentItem();
    Target t;
    if (!item || !currentTarget(t)) return;
    std::error_code ec;
    fs::remove(t.dir / "mods" / item->text().toStdString(), ec);
    sh.mods.st.setText(TS(ec ? i18n::ModRemoveFail : i18n::ModRemoved));
    refreshInstalledList();
}

void MainWindow::refreshInstalledList() {
    ui->installedList->clear();
    Target t;
    if (!currentTarget(t)) return;
    const fs::path dir = t.dir / "mods";
    std::error_code ec;
    if (!fs::exists(dir, ec)) return;
    std::vector<std::string> names;
    for (const auto& e : fs::directory_iterator(dir, ec))
        if (e.is_regular_file() && e.path().extension() == ".jar") names.push_back(e.path().filename().string());
    std::sort(names.begin(), names.end());
    for (const auto& n : names) ui->installedList->addItem(QString::fromStdString(n));
}

// ---------- Збірки: встановлення та видалення ----------

void MainWindow::on_packInstallButton_clicked() {
    QListWidgetItem* item = ui->packResults->currentItem();
    if (!item || sh.packs.st.busy) return;
    const std::string projectId = item->data(Qt::UserRole).toString().toStdString();
    if (projectId.empty()) return;

    sh.packs.st.busy = true;
    sh.packs.st.progress = 0.f;
    sh.packs.st.setText(TS(i18n::InstallingPack));
    Shared* s = &sh;
    std::thread([s, projectId] {
        try {
            const std::string slug = core::installModpack(projectId, s->packs.st);
            {
                std::lock_guard<std::mutex> lk(s->pm);
                s->newPack = slug;
            }
            s->profilesDirty = true;
            s->packs.st.setText(TS(i18n::PackInstalled));
        } catch (const std::exception& e) {
            s->packs.st.setText(TSA(i18n::Error, QString::fromUtf8(e.what())));
        }
        s->packs.st.busy = false;
    }).detach();
}

void MainWindow::on_packRemoveButton_clicked() {
    QListWidgetItem* item = ui->installedPacks->currentItem();
    if (!item) return;
    const QString slug = item->data(Qt::UserRole).toString();
    const auto answer = QMessageBox::question(
        this, T(i18n::RemovePackTitle), T(i18n::RemovePackQ).arg(item->text().section("  ·  ", 0, 0)));
    if (answer != QMessageBox::Yes) return;
    core::deleteProfile(slug.toStdString());
    {
        std::lock_guard<std::mutex> lk(sh.pack.m);
        sh.pack.cache.erase(slug.toStdString());
    }
    sh.profilesDirty = true;
    sh.packs.st.setText(TS(i18n::PackRemoved));
}

void MainWindow::refreshInstalledPacks() {
    ui->installedPacks->clear();
    for (const auto& p : core::listProfiles()) {
        auto* it = new QListWidgetItem(QString::fromStdString(p.name) + "  ·  Minecraft " +
                                       QString::fromStdString(p.mcVersion) +
                                       (p.versionNumber.empty() ? QString() : "  ·  " + QString::fromStdString(p.versionNumber)));
        it->setData(Qt::UserRole, QString::fromStdString(p.slug));
        ui->installedPacks->addItem(it);
    }
}

// ---------- Прев'ю збірки та іконки її модів ----------

void MainWindow::startPackResolve(const std::string& slug) {
    const int gen = ++sh.pack.generation;
    {
        std::lock_guard<std::mutex> lk(sh.pack.m);
        sh.pack.slug = slug;
        auto c = sh.pack.cache.find(slug);
        if (c != sh.pack.cache.end()) {  // вже знаємо склад: Modrinth не смикаємо
            sh.pack.files = c->second.files;
            sh.pack.hitIndex = c->second.hitIndex;
            sh.pack.hits = c->second.hits;
            sh.pack.coverId = c->second.coverId;
            sh.pack.dirty = true;
            return;
        }
        sh.pack.files.clear();
        sh.pack.hitIndex.clear();
        sh.pack.hits.clear();
        sh.pack.coverId.clear();
    }
    Shared* s = &sh;
    std::thread(packWorker, s, gen, slug).detach();
}

// Список модів вибраної встановленої збірки. Оновлюється поступово: рядки не перестворюються,
// тож прокрутка не стрибає, а іконки з'являються по мірі знаходження.
void MainWindow::rebuildPackMods() {
    const QListWidgetItem* cur = ui->installedPacks->currentItem();
    if (!cur || !previewInstalled) {
        ui->packModsList->clear();
        ui->packModsCaption->setText(T(i18n::PackModsCaption));
        return;
    }
    const std::string slug = cur->data(Qt::UserRole).toString().toStdString();
    std::lock_guard<std::mutex> lk(sh.pack.m);
    if (sh.pack.slug != slug) return;

    QListWidget* list = ui->packModsList;
    const int n = static_cast<int>(sh.pack.files.size());
    auto fill = [&](QListWidgetItem* it, int i) {
        const int hi = sh.pack.hitIndex[i];
        const QString file = QString::fromStdString(sh.pack.files[i]);
        if (hi >= 0) {
            const QString title = QString::fromStdString(sh.pack.hits[hi].title);
            it->setText(title + "\n" + file);
            it->setData(Qt::UserRole, QString::fromStdString(sh.pack.hits[hi].projectId));
            it->setIcon(placeholderIcon(title));
        } else {
            it->setText(file);
            it->setData(Qt::UserRole, QString());
            it->setIcon(placeholderIcon(file));
        }
        it->setData(kPlaceholderRole, true);
    };
    if (list->count() != n) {
        list->clear();
        for (int i = 0; i < n; ++i) {
            auto* it = new QListWidgetItem;
            it->setSizeHint(QSize(0, 44));
            fill(it, i);
            list->addItem(it);
        }
    } else {
        for (int i = 0; i < n; ++i) {
            QListWidgetItem* it = list->item(i);
            if (sh.pack.hitIndex[i] >= 0 && it->data(Qt::UserRole).toString().isEmpty()) fill(it, i);
        }
    }
    ui->packModsCaption->setText(T(i18n::PackModsCaption) + QString(" · %1").arg(n));
}

void MainWindow::updatePackPreview() {
    QPixmap pm;
    QString title, note;
    QListWidgetItem* it = nullptr;
    if (previewInstalled && (it = ui->installedPacks->currentItem())) {
        title = it->text().section("  ·  ", 0, 0);
        const std::string slug = it->data(Qt::UserRole).toString().toStdString();
        std::string cover;
        {
            std::lock_guard<std::mutex> lk(sh.pack.m);
            if (sh.pack.slug == slug) cover = sh.pack.coverId;
        }
        if (!cover.empty()) {
            std::lock_guard<std::mutex> lk(sh.im);
            auto f = sh.icons.find(cover);
            if (f != sh.icons.end()) pm.load(QString::fromStdString(f->second));
        }
        note = T(i18n::PackNoteInstalled);
    } else if ((it = ui->packResults->currentItem())) {
        title = it->text().section('\n', 0, 0);
        pm = it->icon().pixmap(112, 112);
        note = T(i18n::PackNoteCatalog);
    } else {
        note = T(i18n::PackNoteNone);
    }
    if (pm.isNull()) pm = placeholderIcon(title).pixmap(112, 112);
    ui->packPreviewImage->setPixmap(pm.scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->packPreviewTitle->setText(title);
    ui->packPreviewNote->setText(note);
}

// ---------- Оновлення вікна 10 разів на секунду ----------

void MainWindow::pollStatus() {
    using namespace i18n;
    if (sh.state == 1 && !versionsShown) {
        versionsShown = true;
        sh.profilesDirty = false;
        fillVersionBox();
        refreshInstalledPacks();
    }
    if (versionsShown && sh.profilesDirty.exchange(false)) {
        QString np;
        {
            std::lock_guard<std::mutex> lk(sh.pm);
            if (!sh.newPack.empty()) np = "pack:" + QString::fromStdString(sh.newPack);
            sh.newPack.clear();
        }
        fillVersionBox(np);
        refreshInstalledPacks();
    }

    const QString verId = currentVersionId();
    const bool isPack = verId.startsWith("pack:");
    const bool fabric = currentLoader() == core::Loader::Fabric;
    ui->loaderBox->setEnabled(!isPack);

    // Час у грі та остання гра
    const bool running = sh.st.gameRunning;
    if (running && !wasRunning) {
        sessionTimer.start();
    } else if (!running && wasRunning) {
        playSeconds += sessionTimer.elapsed() / 1000;
        lastPlayedEpoch = QDateTime::currentSecsSinceEpoch();
        lastPlayedVersion = launchedVersion;
        saveConfig();
    }
    wasRunning = running;
    const qint64 totalSecs = playSeconds + (running ? sessionTimer.elapsed() / 1000 : 0);
    ui->chipPlaytime->setText(T(ChipPlaytime).arg(formatDuration(totalSecs)));
    if (running) {
        ui->chipLast->setText(T(ChipLastNow));
    } else if (lastPlayedEpoch == 0) {
        ui->chipLast->setText(T(ChipLastNever));
    } else {
        ui->chipLast->setText(
            T(ChipLast).arg(QDateTime::fromSecsSinceEpoch(lastPlayedEpoch).toString("dd.MM.yyyy HH:mm") +
                            (lastPlayedVersion.isEmpty() ? QString() : " (" + lastPlayedVersion + ")")));
    }

    // Заголовок і «чіпи» показують обрану версію чи збірку
    QString title = "Minecraft";
    QString chipVer = "-";
    if (isPack) {
        core::Profile p;
        if (core::loadProfile(verId.mid(5).toStdString(), p)) {
            title = QString::fromStdString(p.name);
            chipVer = "Minecraft " + QString::fromStdString(p.mcVersion);
        }
    } else if (!verId.isEmpty()) {
        title = "Minecraft " + verId + (fabric ? " · Fabric" : "");
        chipVer = verId;
    }
    ui->heroTitle->setText(title);
    ui->chipVersion->setText(T(ChipVersion).arg(chipVer));

    // Змінилась версія чи завантажувач: оновлюємо сторінку модів
    const QString context = verId + "|" + QString::number(ui->loaderBox->currentIndex());
    if (context != modsContext) {
        modsContext = context;
        ui->modResults->clear();
        modsRequested = false;
        modHasMore = false;
        sh.installedDirty = true;
        updateModsHint();
    }
    ensureCatalogs();
    if (sh.installedDirty.exchange(false)) refreshInstalledList();

    const bool modsChanged = fillCatalog(sh.mods, ui->modResults, modHasMore);
    const bool packsChanged = fillCatalog(sh.packs, ui->packResults, packHasMore);
    autoLoadMore();

    const bool packViewDirty = sh.pack.dirty.exchange(false);
    if (packViewDirty) rebuildPackMods();

    const bool iconsChanged = sh.iconsDirty.exchange(false);
    if (modsChanged || packsChanged || packViewDirty || iconsChanged) {
        applyIcons(ui->modResults);
        applyIcons(ui->packResults);
        applyIcons(ui->packModsList);
        updatePackPreview();
    }

    ui->progressBar->setValue(static_cast<int>(sh.st.progress.load() * 100.f));
    {
        std::lock_guard<std::mutex> lk(sh.st.m);
        ui->statusLabel->setText(QString::fromStdString(sh.st.text));
        while (shownLines < sh.st.lines.size())
            ui->logView->appendPlainText(QString::fromStdString(sh.st.lines[shownLines++]));

        QStringList recent;
        const size_t from = sh.st.lines.size() > 5 ? sh.st.lines.size() - 5 : 0;
        for (size_t k = from; k < sh.st.lines.size(); ++k)
            recent << QString::fromStdString(sh.st.lines[k]);
        ui->logPreview->setText(recent.isEmpty() ? T(NoEvents) : recent.join("\n"));
    }
    {
        std::lock_guard<std::mutex> lk(sh.mods.st.m);
        ui->modStatusLabel->setText(QString::fromStdString(sh.mods.st.text));
    }
    {
        std::lock_guard<std::mutex> lk(sh.packs.st.m);
        ui->packStatusLabel->setText(QString::fromStdString(sh.packs.st.text));
    }
    ui->packProgress->setValue(static_cast<int>(sh.packs.st.progress.load() * 100.f));

    const bool modBusy = sh.mods.st.busy;
    const bool packBusy = sh.packs.st.busy;
    const bool ready = sh.state == 1;
    ui->modSearchButton->setEnabled(ready && !modBusy && !verId.isEmpty());
    ui->modMoreButton->setEnabled(ready && !modBusy && modHasMore);
    ui->modInstallButton->setEnabled(!modBusy && ui->modResults->currentItem() != nullptr);
    ui->packSearchButton->setEnabled(ready && !packBusy);
    ui->packMoreButton->setEnabled(ready && !packBusy && packHasMore);
    ui->packInstallButton->setEnabled(!packBusy && ui->packResults->currentItem() != nullptr);

    const bool canPlay = sh.state == 1 && ui->versionBox->currentIndex() >= 0 &&
                         !sh.st.busy && !sh.st.gameRunning && !ui->nickEdit->text().isEmpty();
    ui->playButton->setEnabled(canPlay);
    ui->playButton->setText(sh.st.gameRunning ? T(PlayRunning) : (sh.st.busy ? T(PlayLoading) : T(PlayBtn)));
}

// ---------- Фонова картинка ----------

void MainWindow::rebuildBackground() {
    if (bgSource.isNull()) return;
    const qreal dpr = devicePixelRatioF();
    const QSize target(qRound(width() * dpr), qRound(height() * dpr));
    if (target.isEmpty()) return;
    QPixmap scaled = bgSource.scaled(target, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    const int x = (scaled.width() - target.width()) / 2;
    const int y = (scaled.height() - target.height()) / 2;
    bgScaled = scaled.copy(x, y, target.width(), target.height());
    bgScaled.setDevicePixelRatio(dpr);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    rebuildBackground();
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor(5, 5, 5));
    if (!bgScaled.isNull()) p.drawPixmap(0, 0, bgScaled);
    p.fillRect(rect(), QColor(0, 0, 0, 150));  // затемнення, щоб текст читався
}
