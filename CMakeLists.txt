# Project Setup
cmake_minimum_required(VERSION 3.16)
project(
  ElfExcavator
  VERSION 0.2.0
  DESCRIPTION "Graphical ELF File Inspector"
  HOMEPAGE_URL "https://github.com/JonTheBurger/elfexcavator"
  LANGUAGES C CXX
)

# Public Options
set(INSTALL_SYSTEM_DEPS OFF)
if (WIN32)
  set(INSTALL_SYSTEM_DEPS ON)
endif()

option(${PROJECT_NAME}_PACKAGER_MODE "Turn on to disable all dependency auto-resolution" OFF)
option(${PROJECT_NAME}_INSTALL_SYSTEM_DEPS "Bundle required system and runtime libraries when installing" ${INSTALL_SYSTEM_DEPS})
option(${PROJECT_NAME}_DEVELOPER_MODE "Search for and use developer tools" OFF)
option(${PROJECT_NAME}_BUILD_TESTING "Build tests for ${PROJECT_NAME}" ${${PROJECT_NAME}_DEVELOPER_MODE})

# Setup Utility Modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake/modules)
include(GNUInstallDirs)
include(ccache)
include(rpath)
include(sanitizer)
include(variables)
include(warnings)

# Resolve Dependencies
find_package(Qt5 5.9 CONFIG REQUIRED
  COMPONENTS
    Core Charts Gui LinguistTools Widgets
)
find_package(DeployQt5)
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
include(translations)
target_sources(elfexcavator.core PRIVATE data/Resources.qrc ${QM_FILES})
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
add_executable(elfexcavator WIN32)
target_sources(elfexcavator PRIVATE src/main.cpp)
target_link_libraries(elfexcavator PRIVATE elfexcavator.core)
set_target_properties(elfexcavator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER jontheburger.github.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
install(TARGETS elfexcavator)

# Add Tests
if (${PROJECT_NAME}_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# Package
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/data/share/
  DESTINATION share
)
if (COMMAND deploy_qt5)
  deploy_qt5(elfexcavator)
endif()
include(CPack)
