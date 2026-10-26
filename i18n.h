#pragma once
// Прості переклади інтерфейсу без .ts/.qm файлів.
// Щоб додати нову мову: 1) додай значення в enum Lang, 2) додай третій рядок у кожен X(...)
// нижче, 3) розшир `table`, `code()` і `fromCode()`, 4) додай пункт у languageBox (mainwindow.cpp).
#include <QString>

#include <atomic>

// X(ключ, "Українська", "English")
#define I18N_TABLE(X)                                                                              \
    /* Бокова панель */                                                                            \
    X(NavPlay, "Грати", "Play")                                                                    \
    X(NavMods, "Моди", "Mods")                                                                     \
    X(NavPacks, "Збірки", "Packs")                                                                 \
    X(NavSettings, "Налаштування", "Settings")                                                     \
    X(NavLog, "Журнал", "Log")                                                                     \
    /* Головна сторінка */                                                                         \
    X(NickCaption, "НІКНЕЙМ", "NICKNAME")                                                          \
    X(ChipJava, "Java: авто", "Java: auto")                                                        \
    X(ChipVersion, "Версія: %1", "Version: %1")                                                    \
    X(ChipPlaytime, "Час у грі: %1", "Playtime: %1")                                               \
    X(ChipLastNow, "Остання гра: зараз", "Last played: now")                                       \
    X(ChipLastNever, "Остання гра: ще не грав", "Last played: never")                              \
    X(ChipLast, "Остання гра: %1", "Last played: %1")                                              \
    X(PlayBtn, "ГРАТИ", "PLAY")                                                                    \
    X(PlayRunning, "ЗАПУЩЕНО", "RUNNING")                                                          \
    X(PlayLoading, "ЗАВАНТАЖЕННЯ...", "LOADING...")                                                \
    X(VersionCaption, "ВЕРСІЯ", "VERSION")                                                         \
    X(LoaderCaption, "ЗАВАНТАЖУВАЧ", "LOADER")                                                     \
    X(LoaderVanilla, "Vanilla (без модів)", "Vanilla (no mods)")                                   \
    X(LoaderFabric, "Fabric (з модами)", "Fabric (with mods)")                                     \
    /* Каталог модів */                                                                            \
    X(ModSearchPlaceholder, "Пошук модів на Modrinth...", "Search mods on Modrinth...")            \
    X(CatAll, "Усі категорії", "All categories")                                                   \
    X(CatOptimization, "Оптимізація", "Optimization")                                              \
    X(CatUtility, "Утиліти", "Utility")                                                            \
    X(CatAdventure, "Пригоди", "Adventure")                                                        \
    X(CatMagic, "Магія", "Magic")                                                                  \
    X(CatTechnology, "Технології", "Technology")                                                   \
    X(CatDecoration, "Декор", "Decoration")                                                        \
    X(CatWorldgen, "Генерація світу", "World generation")                                          \
    X(CatLibrary, "Бібліотеки", "Libraries")                                                       \
    X(CatMechanics, "Ігрова механіка", "Game mechanics")                                           \
    X(CatMobs, "Істоти (моби)", "Mobs")                                                            \
    X(CatEquipment, "Спорядження", "Equipment")                                                    \
    X(CatFood, "Їжа", "Food")                                                                      \
    X(CatStorage, "Сховища", "Storage")                                                            \
    X(CatTransport, "Транспорт", "Transportation")                                                 \
    X(CatManagement, "Керування", "Management")                                                    \
    X(CatEconomy, "Економіка", "Economy")                                                          \
    X(CatSocial, "Соціальні", "Social")                                                            \
    X(CatMinigame, "Мініігри", "Minigames")                                                        \
    X(CatCursed, "Химерні", "Cursed")                                                              \
    X(SortPopular, "Популярні", "Popular")                                                         \
    X(SortRelevance, "За збігом", "Relevance")                                                     \
    X(SortNewest, "Нові", "Newest")                                                                \
    X(SortUpdated, "Оновлені", "Recently updated")                                                 \
    X(Search, "Пошук", "Search")                                                                   \
    X(ModCatalogCaption, "КАТАЛОГ МОДІВ", "MOD CATALOG")                                           \
    X(ShowMore, "Показати ще", "Show more")                                                        \
    X(Install, "Встановити", "Install")                                                            \
    X(ModInstalledCaption, "ВСТАНОВЛЕНІ МОДИ", "INSTALLED MODS")                                   \
    X(RemoveSelected, "Видалити вибраний", "Remove selected")                                      \
    /* Каталог збірок */                                                                           \
    X(PacksHint,                                                                                   \
      "Готові збірки з Modrinth (Fabric, Minecraft 1.19+). Після встановлення збірка з'являється "  \
      "у списку версій на головній сторінці.",                                                     \
      "Ready-made modpacks from Modrinth (Fabric, Minecraft 1.19+). After installation the pack "   \
      "appears in the version list on the Play page.")                                             \
    X(PackSearchPlaceholder, "Пошук збірок на Modrinth...", "Search modpacks on Modrinth...")      \
    X(PackCatalogCaption, "КАТАЛОГ ЗБІРОК", "MODPACK CATALOG")                                     \
    X(InstallPack, "Встановити збірку", "Install modpack")                                         \
    X(PackInstalledCaption, "ВСТАНОВЛЕНІ ЗБІРКИ", "INSTALLED MODPACKS")                            \
    X(RemovePack, "Видалити збірку", "Remove modpack")                                             \
    X(PackModsCaption, "МОДИ У ЗБІРЦІ", "MODS IN THE PACK")                                        \
    X(PackNoteCatalog,                                                                             \
      "Це обкладинка збірки. Список її модів з іконками з'явиться після встановлення: обери "      \
      "збірку у списку «Встановлені збірки».",                                                     \
      "This is the pack cover. The list of its mods with icons appears after installation: select " \
      "the pack in \"Installed modpacks\".")                                                       \
    X(PackNoteInstalled,                                                                           \
      "Іконки модів підбираються з Modrinth за назвами файлів.",                                   \
      "Mod icons are matched on Modrinth by file names.")                                          \
    X(PackNoteNone, "Обери збірку, щоб побачити її прев'ю.", "Select a pack to see its preview.")  \
    /* Налаштування */                                                                             \
    X(RamCaption, "ПАМ'ЯТЬ ДЛЯ ГРИ (ГБ)", "GAME MEMORY (GB)")                                      \
    X(RamHint, "Скільки оперативної пам'яті виділити Minecraft",                                   \
      "How much RAM to allocate to Minecraft")                                                     \
    X(VersionsCaption, "СПИСОК ВЕРСІЙ", "VERSION LIST")                                            \
    X(Snapshots, "Показувати снапшоти", "Show snapshots")                                          \
    X(RefreshList, "Оновити список", "Refresh list")                                               \
    X(FilesCaption, "ФАЙЛИ ГРИ", "GAME FILES")                                                     \
    X(OpenFolder, "Відкрити папку з грою", "Open game folder")                                     \
    X(RecentCaption, "ОСТАННІ ПОДІЇ", "RECENT EVENTS")                                             \
    X(NoEvents, "Поки що подій немає", "No events yet")                                            \
    X(LangCaption, "МОВА", "LANGUAGE")                                                             \
    X(LangHint, "Мова інтерфейсу лаунчера. Застосовується одразу.",                                \
      "Launcher interface language. Applied immediately.")                                         \
    /* Форматування */                                                                             \
    X(DurZero, "0 хв", "0 min")                                                                    \
    X(DurLess, "менше хвилини", "under a minute")                                                  \
    X(DurHM, "%1 г %2 хв", "%1 h %2 min")                                                          \
    X(DurM, "%1 хв", "%1 min")                                                                     \
    X(CountM, "млн", "M")                                                                          \
    X(CountK, "тис.", "K")                                                                         \
    X(Downloads, "%1 завантажень", "%1 downloads")                                                 \
    /* Статуси */                                                                                  \
    X(Searching, "Шукаю ...", "Searching ...")                                                     \
    X(NothingFound, "Нічого не знайдено", "Nothing found")                                         \
    X(AllResults, "Це всі результати", "That's all the results")                                   \
    X(Added, "Додано: %1", "Added: %1")                                                            \
    X(Found, "Знайдено: %1", "Found: %1")                                                          \
    X(Error, "Помилка: %1", "Error: %1")                                                           \
    X(Installing, "Встановлюю ...", "Installing ...")                                              \
    X(ModInstalled, "Мод встановлено", "Mod installed")                                            \
    X(ModRemoved, "Мод видалено", "Mod removed")                                                   \
    X(ModRemoveFail, "Не вдалося видалити файл", "Could not delete the file")                      \
    X(InstallingPack, "Встановлюю збірку ...", "Installing modpack ...")                           \
    X(PackInstalled,                                                                               \
      "Збірку встановлено. Вона з'явилась у списку версій на головній сторінці",                   \
      "Modpack installed. It now appears in the version list on the Play page")                    \
    X(PackRemoved, "Збірку видалено", "Modpack removed")                                           \
    X(RemovePackTitle, "Видалити збірку", "Remove modpack")                                        \
    X(RemovePackQ, "Видалити збірку «%1» разом із її модами та світами?",                          \
      "Remove the modpack \"%1\" together with its mods and worlds?")                              \
    X(HintPack, "Збірка «%1» · Minecraft %2", "Modpack \"%1\" · Minecraft %2")                     \
    X(HintFabric,                                                                                  \
      "Моди для Fabric · Minecraft %1. Щоб грати з ними, обери «Fabric» на головній сторінці",     \
      "Fabric mods · Minecraft %1. To play with them, choose \"Fabric\" on the Play page")         \
    X(HintLoading, "Версії ще завантажуються ...", "Versions are still loading ...")               \
    X(PackNotFound, "Збірку не знайдено", "Modpack not found")                                     \
    X(VersionNotFound, "Версію Minecraft %1 не знайдено у списку",                                 \
      "Minecraft version %1 was not found in the list")                                            \
    X(VersionsFail, "Не вдалося отримати список версій: %1", "Could not fetch the version list: %1")

namespace i18n {

enum class Lang : int { Uk = 0, En = 1 };

enum Key {
#define X(id, uk, en) id,
    I18N_TABLE(X)
#undef X
        KeyCount
};

inline std::atomic<int>& langRef() {
    static std::atomic<int> l{0};
    return l;
}
inline Lang lang() { return static_cast<Lang>(langRef().load()); }
inline void setLang(Lang l) { langRef().store(static_cast<int>(l)); }

inline const char* code(Lang l) { return l == Lang::En ? "en" : "uk"; }
inline Lang fromCode(const QString& c) {
    return c.startsWith(QLatin1String("en"), Qt::CaseInsensitive) ? Lang::En : Lang::Uk;
}

// Безпечно викликати з будь-якого потоку
inline QString tr(Key k) {
    static const char* const table[][2] = {
#define X(id, uk, en) {uk, en},
        I18N_TABLE(X)
#undef X
    };
    return QString::fromUtf8(table[k][static_cast<int>(lang())]);
}

}  // namespace i18n
