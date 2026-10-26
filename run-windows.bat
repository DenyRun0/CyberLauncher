@echo off
chcp 65001 >nul
cd /d "%~dp0"

if exist "CyberLauncher.exe" (
    start "" "CyberLauncher.exe"
    exit /b 0
)
if exist "build\Release\CyberLauncher.exe" (
    start "" "build\Release\CyberLauncher.exe"
    exit /b 0
)

echo Cyber Launcher ще не зібраний.
echo.
echo Найпростіше: завантажте CyberLauncher-windows.zip зі сторінки Releases на GitHub,
echo розпакуйте і запустіть CyberLauncher.exe.
echo Або зберіть самі за інструкцією в README.md.
echo.
pause
