# =========================
# Project Settings
# =========================
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(elf-browser)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Charts Gui Widgets)

# =========================
# User Options
# =========================
# Set default build type to debug
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_executable(${PROJECT_NAME}
    main.cxx
    MainWindow.ui
    MainWindow.cxx
    MainWindow.hxx
    BinUtilsDialog.ui
    BinUtilsDialog.cxx
    BinUtilsDialog.hxx
    BinUtils.cxx
    BinUtils.hxx
    SectionHeaderParser.cxx
    SectionHeaderParser.hxx
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Core
    Qt5::Charts
    Qt5::Gui
    Qt5::Widgets
)
