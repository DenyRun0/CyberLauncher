# Cyber Launcher

Лаунчер Minecraft (1.19+) з Fabric, каталогом модів і збірок Modrinth. Працює на Windows і macOS.

## Завантажити й запустити

Візьміть архів зі сторінки **Releases**.

**Windows:** `CyberLauncher-windows.zip` → розпакувати → `CyberLauncher.exe`.
Дані гри зберігаються в теці `game` поруч з exe.

**macOS (Apple Silicon):** `CyberLauncher-macos.zip` → розпакувати → відкрити `CyberLauncher.app` після перевірки походження та сумісності програми.
Дані гри: `~/Library/Application Support/CyberLauncher/game`.

Ядро визначає потрібну версію Java за метаданими Minecraft, шукає встановлену Java та за її відсутності передбачає автоматичне завантаження. Для запуску потрібне законне право користування Minecraft.

## Збірка з вихідного коду

Потрібні: CMake 3.20+, Qt 6 (Widgets), libcurl. Заголовки nlohmann/json лежать у `include/`.

**macOS**
```
cmake -B build -DCMAKE_PREFIX_PATH=<шлях до Qt>
cmake --build build
open build/CyberLauncher.app
```

**Windows** (Visual Studio + vcpkg)
```
vcpkg install curl:x64-windows
cmake -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=<шлях до Qt>
cmake --build build --config Release
build\Release\CyberLauncher.exe
```

Після збірки можна користуватись `run-windows.bat` або `run-mac.command`.

## Як опублікувати реліз

Зробіть тег і надішліть його на GitHub: GitHub Actions збере архіви для Windows і macOS і прикріпить їх до релізу.
```
git tag v1.0.0
git push origin v1.0.0
```

## Автоматизовані тести

Після конфігурації CMake з `-DBUILD_TESTING=ON`:

```bash
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

П'ять перевірок охоплюють кодування URL, шлях Maven, підстановку параметрів та відхилення непідтримуваної адреси.

## Керування модами

У розділі «Моди» виберіть встановлений мод і натисніть «Увімкнути / вимкнути». Вимкнення змінює розширення `.jar` на `.jar.disabled`; повторне натискання відновлює назву. Файл зберігається. Якщо однойменний файл уже існує, операція відхиляється. Перемикання заблоковане під час роботи гри та завантажень.
