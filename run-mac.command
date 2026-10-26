#!/bin/bash
# Запускає Cyber Launcher на macOS (подвійний клік у Finder).
cd "$(dirname "$0")" || exit 1

APP=""
for candidate in "CyberLauncher.app" "build/CyberLauncher.app"; do
    if [ -d "$candidate" ]; then APP="$candidate"; break; fi
done

if [ -z "$APP" ]; then
    echo "Cyber Launcher ще не зібраний."
    echo
    echo "Найпростіше: завантажте CyberLauncher-macos.zip зі сторінки Releases на GitHub,"
    echo "розпакуйте і запустіть цей файл знову."
    echo "Або зберіть самі за інструкцією в README.md."
    echo
    read -r -p "Натисніть Enter, щоб закрити..."
    exit 1
fi

# Прибираємо мітку карантину, яку macOS ставить на завантажені програми
xattr -dr com.apple.quarantine "$APP" 2>/dev/null
open "$APP"
