# =========================
# Project Settings
# =========================
cmake_minimum_required(VERSION 3.5)
project(elf-browser)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.9 REQUIRED COMPONENTS Core Charts Gui Widgets Qml)

# =========================
# Options
# =========================
# Set default build type to debug
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

option(${PROJECT_NAME}_BUILD_TESTING "" OFF)

# =========================
# Sources
# =========================
add_library(lib${PROJECT_NAME}
    BinUtils.cxx
    BinUtils.hxx
    BinUtilsDialog.cxx
    BinUtilsDialog.hxx
    BinUtilsDialog.ui
    CircularIterator.hxx
    DisassemblyHighlighter.cxx
    DisassemblyHighlighter.hxx
    FilterHeaderView.cxx
    FilterHeaderView.hxx
    FindLineEdit.cxx
    FindLineEdit.hxx
    HexNumberDelegate.cxx
    HexNumberDelegate.hxx
    MainWindow.cxx
    MainWindow.hxx
    MainWindow.ui
    MultiFilterProxyModel.cxx
    MultiFilterProxyModel.hxx
    MaxStore.hxx
    Palette.cxx
    Palette.hxx
    PieChart.cxx
    PieChart.hxx
    SectionHeader.cxx
    SectionHeader.hxx
    SectionHeaderItemModel.cxx
    SectionHeaderItemModel.hxx
    Symbol.cxx
    Symbol.hxx
    SymbolItemModel.cxx
    SymbolItemModel.hxx
    TableView.cxx
    TableView.hxx
)
target_include_directories(lib${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(lib${PROJECT_NAME}
    PUBLIC
    Qt5::Core
    Qt5::Charts
    Qt5::Gui
    Qt5::Widgets
    Qt5::Qml
)

add_executable(${PROJECT_NAME}
    main.cxx
)
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})

if (${${PROJECT_NAME}_BUILD_TESTING})
  add_subdirectory(test)
endif()
