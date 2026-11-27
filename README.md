# Cyber Launcher

Лаунчер Minecraft (1.19+) з Fabric, каталогом модів і збірок Modrinth. Працює на Windows і macOS.

## Завантажити й запустити

Візьміть архів зі сторінки **Releases**.

**Windows:** `CyberLauncher-windows.zip` → розпакувати → `CyberLauncher.exe`.
Дані гри зберігаються в теці `game` поруч з exe.

**macOS (Intel):** `CyberLauncher-macos-intel.zip` → розпакувати → відкрити `CyberLauncher.app` після перевірки походження програми.

**macOS (Apple Silicon, M1/M2/M3/M4 та новіші):** `CyberLauncher-macos-arm64.zip` → розпакувати → відкрити `CyberLauncher.app`.

Цільова мінімальна версія для обох нових збірок — macOS 12 Monterey (Qt 6.8.3). Запуск релізу на Intel Mac провірено; сумісність з усіма версіями macOS не гарантується. Попередній архів `CyberLauncher-macos.zip` у релізі v3.0 призначений для Apple Silicon.
Дані гри: `~/Library/Application Support/CyberLauncher/game`.


Ядро визначає потрібну версію Java за метаданими Minecraft, шукає встановлену Java та за її відсутності передбачає автоматичне завантаження.

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


## Керування модами

У розділі «Моди» виберіть встановлений мод і натисніть «Увімкнути / вимкнути». Вимкнення змінює розширення `.jar` на `.jar.disabled`; повторне натискання відновлює назву. Файл зберігається. Якщо однойменний файл уже існує, операція відхиляється. Перемикання заблоковане під час роботи гри та завантажень.

## Документація

- [Практичні №1–20](CyberLauncher_Labs.md)
- [Інструкція користувача](docs/USER_MANUAL.md)
- [Реліз v3.1-build-8](https://github.com/DenyRun0/CyberLauncher/releases/tag/v3.1-build-8)

- [Галерея оригінальних знімків](docs/SCREENSHOTS.md)

