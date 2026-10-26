#pragma once
// core.hpp підключаємо ПЕРШИМ, до заголовків Qt
#include "core.hpp"

#include <QElapsedTimer>
#include <QMainWindow>
#include <QPixmap>
#include <QString>
#include <QTimer>

#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QComboBox;
class QListWidget;
class QListWidgetItem;

// Результати пошуку в каталозі Modrinth (моди або збірки)
struct Catalog {
    std::mutex m;
    std::vector<core::ModHit> hits;
    bool append = false;
    std::atomic<bool> ready{false};
    core::Status st;
};

// Склад вибраної ВСТАНОВЛЕНОЇ збірки: jar-файли з папки mods і знайдені для них проєкти Modrinth
// (щоб показати іконку кожного мода). Заповнюється у фоні.
struct PackView {
    struct Cached {
        std::vector<std::string> files;
        std::vector<int> hitIndex;
        std::vector<core::ModHit> hits;
        std::string coverId;
    };
    std::mutex m;
    std::string slug;                // для якої збірки зібрано дані
    std::vector<std::string> files;  // назви jar-файлів
    std::vector<int> hitIndex;       // індекс у hits або -1, якщо проєкт не знайдено
    std::vector<core::ModHit> hits;
    std::string coverId;             // projectId збірки на Modrinth (для обкладинки), може бути порожнім
    std::map<std::string, Cached> cache;
    std::atomic<bool> dirty{false};
    std::atomic<int> generation{0};  // збільшується при виборі іншої збірки; старі потоки самі завершуються
};

// Спільний стан для фонових потоків. Навмисно не видаляється:
// потоки можуть жити до виходу з програми.
struct Shared {
    std::vector<core::VersionInfo> versions;
    std::mutex m;
    std::atomic<int> state{0};  // 0 - завантажується, 1 - готово, 2 - помилка
    core::Status st;            // стан гри

    Catalog mods, packs;
    PackView pack;

    std::mutex im;  // іконки каталогів: id проєкту -> файл
    std::map<std::string, std::string> icons;
    std::atomic<bool> iconsDirty{false};

    std::atomic<bool> installedDirty{true};
    std::atomic<bool> profilesDirty{true};
    std::mutex pm;
    std::string newPack;  // щойно встановлена збірка
};

// Куди ставити моди: версія гри та папка
struct Target {
    std::string mc;
    std::string loaderVersion;
    std::filesystem::path dir;
    bool isPack = false;
    std::string packName;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // Slot з назвою on_<ім'я елемента>_<сигнал> Qt підключає до кнопки САМ.
    void on_playButton_clicked();
    void on_refreshButton_clicked();
    void on_openFolderButton_clicked();
    void on_snapshotsCheck_toggled(bool checked);
    void on_languageBox_currentIndexChanged(int index);

    void on_modSearchButton_clicked();
    void on_modSearchEdit_returnPressed();
    void on_modCategoryBox_currentIndexChanged(int index);
    void on_modSortBox_currentIndexChanged(int index);
    void on_modMoreButton_clicked();
    void on_modInstallButton_clicked();
    void on_modRemoveButton_clicked();

    void on_packSearchButton_clicked();
    void on_packSearchEdit_returnPressed();
    void on_packSortBox_currentIndexChanged(int index);
    void on_packMoreButton_clicked();
    void on_packInstallButton_clicked();
    void on_packRemoveButton_clicked();

    void pollStatus();

private:
    void loadVersionsAsync();
    void fillVersionBox(const QString& forceId = QString());
    void loadConfig();
    void saveConfig();
    void rebuildBackground();

    // Мова
    void retranslateUi();
    void fillCombos();
    void updateModsHint();

    void refreshInstalledList();
    void refreshInstalledPacks();
    void runModSearch(bool append);
    void runPackSearch(bool append);
    void startSearch(Catalog& c, const core::SearchParams& p, bool append);
    bool fillCatalog(Catalog& c, QListWidget* list, bool& hasMore);
    void applyIcons(QListWidget* list);
    void ensureCatalogs();
    void autoLoadMore();

    // Прев'ю збірки та іконки її модів
    void updatePackPreview();
    void startPackResolve(const std::string& slug);
    void rebuildPackMods();

    QString currentVersionId() const;
    core::Loader currentLoader() const;
    bool currentTarget(Target& t) const;

    Ui::MainWindow* ui;
    Shared& sh;
    QTimer timer;
    std::thread worker;
    QString wantedVersion;
    QPixmap bgSource;  // фонова картинка
    QPixmap bgScaled;  // вона ж, підігнана під розмір вікна
    size_t shownLines = 0;
    bool versionsShown = false;

    // Час у грі
    QElapsedTimer sessionTimer;
    bool wasRunning = false;
    qint64 playSeconds = 0;
    qint64 lastPlayedEpoch = 0;
    QString lastPlayedVersion;
    QString launchedVersion;

    // Каталоги
    QString modsContext;  // "версія|завантажувач", щоб помітити зміну
    bool modsRequested = false;
    bool packsRequested = false;
    bool modHasMore = false;
    bool packHasMore = false;
    int modAutoLeft = 0;   // скільки додаткових сторінок ще підвантажити автоматично
    int packAutoLeft = 0;
    bool previewInstalled = false;  // прев'ю показує встановлену збірку (true) чи збірку з каталогу
};
