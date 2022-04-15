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
option(${PROJECT_NAME}_PACKAGER_MODE "Turn on to disable all dependency auto-resolution" OFF)
option(${PROJECT_NAME}_DEVELOPER_MODE "Search for and use developer tools" OFF)
option(${PROJECT_NAME}_BUILD_TESTING "Build tests for ${PROJECT_NAME}" ${${PROJECT_NAME}_DEVELOPER_MODE})

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
find_package(Catch2 QUIET)
find_package(elfio QUIET)
find_package(KF5ItemModels QUIET)
find_package(LLVM QUIET)
find_package(qhexview QUIET)
find_package(qtadvanceddocking QUIET)
find_package(range-v3 QUIET)
find_package(spdlog 1.9.2 QUIET)
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
    LLVMDemangle
    elfio::elfio
    range-v3::range-v3
    spdlog::spdlog
    qhexview-lib
    ads::qtadvanceddocking
    KF5::ItemModels
    Qt5::Charts
    Qt5::Widgets
    Qt5::Gui
    Qt5::Core
)
target_compile_options(elfexcavator.core
  PUBLIC
    ${${PROJECT_NAME}_WARNINGS}
    ${${PROJECT_NAME}_SANITIZERS}
)
target_link_options(elfexcavator.core
  PUBLIC
    ${${PROJECT_NAME}_SANITIZERS_LINK}
)

# Add Main Executable
add_executable(elfexcavator)
target_sources(elfexcavator PRIVATE src/main.cpp)
target_link_libraries(elfexcavator PRIVATE elfexcavator.core)
install(TARGETS elfexcavator)

# Add Tests
if (${PROJECT_NAME}_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# Package
include(CPack)
