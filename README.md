# Cyber Launcher

Лаунчер Minecraft (1.19+) з Fabric, каталогом модів і збірок Modrinth. Працює на Windows і macOS.

## Завантажити й запустити

Візьміть архів зі сторінки **Releases**.

**Windows:** `CyberLauncher-windows.zip` → розпакувати → `CyberLauncher.exe`.
Дані гри зберігаються в теці `game` поруч з exe.

**macOS (Apple Silicon):** `CyberLauncher-macos.zip` → розпакувати → двічі клікнути `run-mac.command`.
Якщо macOS не дозволяє відкрити файл: правий клік → «Відкрити» → «Відкрити».
Дані гри: `~/Library/Application Support/CyberLauncher/game`.

Потрібен встановлений JDK (Java) з https://adoptium.net: версія 17 для Minecraft 1.18–1.20.4, 21 для новіших.

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
