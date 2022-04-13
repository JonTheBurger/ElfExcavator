# Project Setup
cmake_minimum_required(VERSION 3.16)
project(
  ElfExcavator
  VERSION 0.0.2
  DESCRIPTION "Graphical ELF File Inspector"
  HOMEPAGE_URL "https://github.com/JonTheBurger/elfexcavator"
  LANGUAGES C CXX
)

# Public Options
option(${PROJECT_NAME}_BUILD_TESTING "Build tests for ${PROJECT_NAME}" ${${PROJECT_NAME}_DEVELOPER_MODE})
option(${PROJECT_NAME}_DEVELOPER_MODE "Search for and use developer tools" OFF)
option(${PROJECT_NAME}_PACKAGER_MODE "Turn on to disable all dependency auto-resolution" OFF)

# Setup Utility Modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake/modules)
include(ccache)
include(rpath)
include(sanitizer)
include(variables)
include(warnings)

# Resolve Dependencies
find_package(Qt5 5.9 CONFIG REQUIRED
  COMPONENTS
    Core Charts Gui Widgets Qml
)
find_package(Catch2)
find_package(elfio)
find_package(kf5itemmodels)
find_package(LLVM)
find_package(qhexview)
find_package(qtadvanceddocking)
find_package(range-v3)
find_package(spdlog 1.9.2)
add_subdirectory(external EXCLUDE_FROM_ALL)

# Add Core Library (for exe and reuse in tests)
add_library(elfexcavator.core)
add_subdirectory(src)
target_include_directories(elfexcavator.core
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)
target_link_libraries(elfexcavator.core
  PUBLIC
    elfio::elfio
    LLVMDemangle
    spdlog::spdlog
    range-v3::range-v3
    qtadvanceddocking
    KF5::ItemModels
    Qt5::Core
    Qt5::Charts
    Qt5::Gui
    Qt5::Widgets
    qhexview-lib
)
target_compile_options(elfexcavator.core
  PUBLIC
    ${${PROJECT_NAME}_WARNINGS}
    ${${PROJECT_NAME}_SANITIZERS}
)
target_link_options(elfexcavator.core
  PUBLIC
    ${${PROJECT_NAME}_SANITIZERS_LINK}
    $<$<CONFIG:DEBUG>:-fsanitize=address,leak,undefined>
)
target_compile_options(elfexcavator.core
  PUBLIC
    $<$<CONFIG:DEBUG>:-fsanitize=address,leak,undefined -fno-omit-frame-pointer -fno-common>
)

# Add Main Executable
add_executable(elfexcavator)
target_sources(elfexcavator PRIVATE src/main.cpp)
target_link_libraries(elfexcavator PRIVATE elfexcavator.core)

# Add Tests
if (${PROJECT_NAME}_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# Setup Install Rules
include(installation)
include(deb)
