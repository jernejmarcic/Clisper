cmake_minimum_required(VERSION 3.25)

project(Clisper
    VERSION 0.8.0
    DESCRIPTION "Linux-first clipboard history utility and scratchpad"
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# Global build settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -pipe)
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

# All artifacts under build/ regardless of single/multi-config generator
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(MAGIC      REQUIRED IMPORTED_TARGET libmagic)
pkg_check_modules(EXIV2      REQUIRED IMPORTED_TARGET exiv2)
pkg_check_modules(TEXTCAT    REQUIRED IMPORTED_TARGET libexttextcat)
pkg_check_modules(TESSERACT  REQUIRED IMPORTED_TARGET tesseract)
pkg_check_modules(LEPT       REQUIRED IMPORTED_TARGET lept)
pkg_check_modules(SQLITE3    REQUIRED IMPORTED_TARGET sqlite3)

# sqlpp23: prefer system install, otherwise vendor via FetchContent so users
# don't need to compile it manually. Only the SQLite3 connector is built.
find_package(Sqlpp23 QUIET CONFIG)
if(NOT Sqlpp23_FOUND)
    message(STATUS "sqlpp23 not installed system-wide; fetching from upstream")
    include(FetchContent)
    set(BUILD_SQLITE3_CONNECTOR    ON  CACHE BOOL "" FORCE)
    set(BUILD_MYSQL_CONNECTOR      OFF CACHE BOOL "" FORCE)
    set(BUILD_MARIADB_CONNECTOR    OFF CACHE BOOL "" FORCE)
    set(BUILD_POSTGRESQL_CONNECTOR OFF CACHE BOOL "" FORCE)
    set(BUILD_SQLCIPHER_CONNECTOR  OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING              OFF CACHE BOOL "" FORCE)
    set(BUILD_WITH_MODULES         OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(sqlpp23
        GIT_REPOSITORY https://github.com/rbock/sqlpp23.git
        GIT_TAG        0.69
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(sqlpp23)
endif()

# -----------------------------------------------------------------------------
# Core ingester: build/main
# -----------------------------------------------------------------------------
add_executable(main
    src/core/main.cpp
    src/core/textMIMErefinement.cpp
)
target_include_directories(main PRIVATE src/core)
target_link_libraries(main PRIVATE
    PkgConfig::MAGIC
    PkgConfig::EXIV2
    PkgConfig::TEXTCAT
    PkgConfig::TESSERACT
    PkgConfig::LEPT
    PkgConfig::SQLITE3
    Threads::Threads
)
target_link_libraries(main PRIVATE sqlpp23::core sqlpp23::sqlite3)

# -----------------------------------------------------------------------------
# DB bootstrap helper: build/dbInit
# -----------------------------------------------------------------------------
add_executable(dbInit src/core/dbInit.cpp)
target_link_libraries(dbInit PRIVATE PkgConfig::SQLITE3)

add_custom_target(database
    COMMAND $<TARGET_FILE:dbInit>
    DEPENDS dbInit
    COMMENT "Bootstrap ~/.local/share/clisper/clisper.db"
)

# -----------------------------------------------------------------------------
# dmenu compatibility shim: build/dmenuCompatibility
# -----------------------------------------------------------------------------
add_executable(dmenuCompatibility src/compat/dmenuCompatibility.cpp)
target_link_libraries(dmenuCompatibility PRIVATE PkgConfig::SQLITE3)

# -----------------------------------------------------------------------------
# GTK frontend: build/clisperGTK
# -----------------------------------------------------------------------------
pkg_check_modules(GTK4   IMPORTED_TARGET gtk4)
pkg_check_modules(ADW    IMPORTED_TARGET libadwaita-1)

if(TARGET PkgConfig::GTK4 AND TARGET PkgConfig::ADW)
    add_executable(clisperGTK
        src/gui/main.cpp
        src/gui/clisperGTK.cpp
    )
    target_link_libraries(clisperGTK PRIVATE PkgConfig::GTK4 PkgConfig::ADW)
else()
    message(STATUS "GTK4 / libadwaita-1 not found — skipping clisperGTK target")
endif()

# -----------------------------------------------------------------------------
# Qt6 frontend: build/clisperQt
# -----------------------------------------------------------------------------
find_package(Qt6 6.5 COMPONENTS Core Gui Widgets Svg QUIET)

if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)

    set(QT_GUI_DIR src/gui-qt)
    set(QT_GUI_SOURCES
        ${QT_GUI_DIR}/main.cpp
        ${QT_GUI_DIR}/Config.cpp
        ${QT_GUI_DIR}/Database.cpp
        ${QT_GUI_DIR}/SyntaxTheme.cpp
        ${QT_GUI_DIR}/CodeHighlighter.cpp
        ${QT_GUI_DIR}/MarkdownHighlighter.cpp
        ${QT_GUI_DIR}/CarbonExporter.cpp
        ${QT_GUI_DIR}/ClipboardOut.cpp
        ${QT_GUI_DIR}/EntryListModel.cpp
        ${QT_GUI_DIR}/EntryDelegate.cpp
        ${QT_GUI_DIR}/DetailPanel.cpp
        ${QT_GUI_DIR}/TextPreview.cpp
        ${QT_GUI_DIR}/ImageEditor.cpp
        ${QT_GUI_DIR}/MainWindow.cpp
    )

    add_executable(clisperQt ${QT_GUI_SOURCES})
    target_include_directories(clisperQt PRIVATE ${QT_GUI_DIR})
    target_link_libraries(clisperQt PRIVATE
        Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg
        PkgConfig::SQLITE3
    )
else()
    message(STATUS "Qt6 not found — skipping clisperQt target")
endif()

# -----------------------------------------------------------------------------
# Smoke test (mirrors `make test`)
# -----------------------------------------------------------------------------
add_custom_target(smoke
    COMMAND echo "Smoke test — piping paragraph into build/main" |
            $<TARGET_FILE:main>
    DEPENDS main
    COMMENT "Pipe a paragraph into the ingester"
)

# -----------------------------------------------------------------------------
# Default target ("all" already builds every add_executable)
# -----------------------------------------------------------------------------
