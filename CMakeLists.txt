cmake_minimum_required(VERSION 3.20)
project(CyberLauncherQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)

set(LAUNCHER_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc)

# ---------- Іконка програми ----------
# macOS: icon.icns кладеться в .app; Windows: icon.ico вшивається в .exe (якщо файл є)
set(APP_ICON_MAC ${CMAKE_SOURCE_DIR}/icon.icns)
set(APP_ICON_WIN ${CMAKE_SOURCE_DIR}/icon.ico)

if(APPLE AND EXISTS ${APP_ICON_MAC})
    set_source_files_properties(${APP_ICON_MAC} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    list(APPEND LAUNCHER_SOURCES ${APP_ICON_MAC})
endif()

if(WIN32 AND EXISTS ${APP_ICON_WIN})
    enable_language(RC)
    file(WRITE ${CMAKE_BINARY_DIR}/launcher_icon.rc "IDI_ICON1 ICON \"${APP_ICON_WIN}\"\n")
    list(APPEND LAUNCHER_SOURCES ${CMAKE_BINARY_DIR}/launcher_icon.rc)
endif()

# MACOSX_BUNDLE - .app на Mac, WIN32 - програма без консольного вікна на Windows
add_executable(launcher MACOSX_BUNDLE WIN32 ${LAUNCHER_SOURCES})

set_target_properties(launcher PROPERTIES OUTPUT_NAME "CyberLauncher")
if(APPLE)
    set_target_properties(launcher PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Cyber Launcher"
        MACOSX_BUNDLE_GUI_IDENTIFIER local.cyberlauncher)
    if(EXISTS ${APP_ICON_MAC})
        set_target_properties(launcher PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns)
    endif()
endif()

target_include_directories(launcher PRIVATE include)
# Усі дані гри (папка game) лежатимуть у цій папці проєкту
target_compile_definitions(launcher PRIVATE LAUNCHER_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(launcher PRIVATE Qt6::Widgets CURL::libcurl)

# ---------- Windows ----------
if(MSVC)
    # Українські рядки в UTF-8 у вихідниках
    target_compile_options(launcher PRIVATE /utf-8)
endif()

if(WIN32)
    # Після збірки кладемо поруч з exe потрібні DLL Qt (якщо windeployqt знайдено)
    get_target_property(_qt_core_loc Qt6::Core IMPORTED_LOCATION_RELEASE)
    if(NOT _qt_core_loc)
        get_target_property(_qt_core_loc Qt6::Core IMPORTED_LOCATION)
    endif()
    find_program(WINDEPLOYQT_EXE windeployqt
        HINTS "${Qt6_DIR}/../../../bin" "${Qt6_DIR}/../../../../bin")
    if(NOT WINDEPLOYQT_EXE AND _qt_core_loc)
        get_filename_component(_qt_bin "${_qt_core_loc}" DIRECTORY)
        find_program(WINDEPLOYQT_EXE windeployqt HINTS "${_qt_bin}")
    endif()
    if(WINDEPLOYQT_EXE)
        add_custom_command(TARGET launcher POST_BUILD
            COMMAND "${WINDEPLOYQT_EXE}" --no-translations "$<TARGET_FILE:launcher>"
            COMMENT "windeployqt: копіюю DLL Qt")
    endif()
endif()
