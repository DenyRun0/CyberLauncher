cmake_minimum_required(VERSION 3.20)
project(CyberLauncherQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)

set(APP_ICON ${CMAKE_SOURCE_DIR}/icon.icns)
set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

add_executable(launcher MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
    ${APP_ICON})

set_target_properties(launcher PROPERTIES
    OUTPUT_NAME "CyberLauncher"
    MACOSX_BUNDLE_BUNDLE_NAME "Cyber Launcher"
    MACOSX_BUNDLE_ICON_FILE icon.icns
    MACOSX_BUNDLE_GUI_IDENTIFIER local.cyberlauncher)

target_include_directories(launcher PRIVATE include)
# Усі дані гри (папка game) лежатимуть у цій папці проєкту
target_compile_definitions(launcher PRIVATE LAUNCHER_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(launcher PRIVATE Qt6::Widgets CURL::libcurl)
