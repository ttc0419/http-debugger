cmake_minimum_required(VERSION 3.5)

project(
    http-debugger
    VERSION 1.0.0
    DESCRIPTION "A cross-platform postman like tool to debug your http servers"
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    PROJECT_SOURCES
    src/main.cpp
    src/MainWidget.ui src/MainWidget.h src/MainWidget.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Network)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building for Windows")
    set(WINDOWS_ICON_RESOURCE resources/http-debugger.rc)
    qt_add_executable(http-debugger MANUAL_FINALIZATION ${PROJECT_SOURCES} ${WINDOWS_ICON_RESOURCE})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Building for MacOS")
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set(MACOS_ICON resources/images/icon.icns)
    set_source_files_properties(${MACOS_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    qt_add_executable(http-debugger MANUAL_FINALIZATION MACOSX_BUNDLE ${PROJECT_SOURCES} ${MACOS_ICON})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building for Linux")
    qt_add_executable(http-debugger MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    message(FATAL_ERROR "The platform is not supported!")
endif()

set_target_properties(
    http-debugger PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER http-debugger.ttc.icu
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(http-debugger PRIVATE Qt6::Widgets Qt6::Network)
qt_add_translations(http-debugger TS_FILES i18n/en.ts i18n/zh_CN.ts LUPDATE_OPTIONS "-noobsolete")

qt_finalize_executable(http-debugger)
