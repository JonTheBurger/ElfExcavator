# =========================
# Project Settings
# =========================
cmake_minimum_required(VERSION 3.16)
project(ElfExcavator
  VERSION 0.0.2
  DESCRIPTION "Graphical ELF File Inspector"
  LANGUAGES C CXX
)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake/modules)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CPACK_PACKAGE_VENDOR "jontheburger")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_DEBUG_POSTFIX _d)

include(GNUInstallDirs)
include(GenerateExportHeader)

# Set default directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "" FORCE)
endif()

# Enable relative shared object lookup, enabling relocatable binaries
file(RELATIVE_PATH relativeBinToLibDir
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
)
if (APPLE)
  set(originPath @loader_path)
else()
  set(originPath $ORIGIN)
endif()
set(CMAKE_INSTALL_RPATH ${originPath} ${originPath}/${relativeBinToLibDir})
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

# Install
set(${PROJECT_NAME}_INSTALL_CMAKE_CONFIG_MODULE_DIR
  "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  CACHE PATH
  "Install prefix relative directory where ${PROJECT_NAME}Config.cmake installs to"
)

# CCache
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

option(${PROJECT_NAME}_BUILD_TESTING "" OFF)
option(${PROJECT_NAME}_USE_BUNDLED_LLVM "Use bundled copy of LLVMDemangle instead of system version" ON)

find_package(Qt5 5.9 REQUIRED COMPONENTS Core Charts Gui Widgets Qml)
add_subdirectory(external EXCLUDE_FROM_ALL)

add_library(ElfExcavator.Core
  src/model/ElfFile.cpp
  src/model/ElfFile.hpp
  src/model/TextFilter.cpp
  src/model/TextFilter.hpp
  src/presenter/MainPresenter.cpp
  src/presenter/MainPresenter.hpp
  src/presenter/MultiFilterProxyModel.cpp
  src/presenter/MultiFilterProxyModel.hpp
  src/presenter/SectionHeaderItemModel.cpp
  src/presenter/SectionHeaderItemModel.hpp
  src/presenter/SettingsPresenter.cpp
  src/presenter/SettingsPresenter.hpp
  src/presenter/SymbolTableItemModel.cpp
  src/presenter/SymbolTableItemModel.hpp
  src/view/CxxDisassemblyHighlighter.cpp
  src/view/CxxDisassemblyHighlighter.hpp
  src/view/HexNumberDelegate.cpp
  src/view/HexNumberDelegate.hpp
  src/view/HexView.cpp
  src/view/HexView.hpp
  src/view/MainWindow.cpp
  src/view/MainWindow.hpp
  src/view/MultiFilterHeaderView.cpp
  src/view/MultiFilterHeaderView.hpp
  src/view/MultiFilterTableView.cpp
  src/view/MultiFilterTableView.hpp
  src/view/PieChartForm.cpp
  src/view/PieChartForm.hpp
  src/view/SettingsForm.cpp
  src/view/SettingsForm.hpp
)
target_include_directories(ElfExcavator.Core PUBLIC src)
target_link_libraries(ElfExcavator.Core
  PUBLIC
    asan
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
if (CMAKE_C_COMPILER_ID STREQUAL MSVC)
  target_compile_options(ElfExcavator.Core
    PUBLIC
      /W4
      /permissive-
      /w14640
      /w14242
      /w14254
      /w14263
      /w14265
      /w14287
      /we4289
      /w14296
      /w14311
      /w14545
      /w14546
      /w14547
      /w14549
      /w14555
      /w14619
      /w14640
      /w14826
      /w14905
      /w14906
      /w14928
  )
elseif(CMAKE_C_COMPILER_ID MATCHES Clang)
  target_compile_options(ElfExcavator.Core
    PUBLIC
      -Weverything
  )
else()
  target_link_options(ElfExcavator.Core
    PUBLIC
      -fsanitize=address
  )
  target_compile_options(ElfExcavator.Core
    PUBLIC
      -fsanitize=address
      -fno-omit-frame-pointer
      -Wall
      -Wextra
      -pedantic
      -Wcast-align
      -Wconversion
      -Wdouble-promotion
      -Wduplicated-branches
      -Wduplicated-cond
      -Werror=return-type
      -Wlogical-op
      -Wmisleading-indentation
      -Wnon-virtual-dtor
      -Wnull-dereference
      -Wold-style-cast
      -Woverloaded-virtual
      -Wpedantic
      -Wshadow
      -Wsign-conversion
      -Wunused
      -Wuseless-cast
  )
endif()

add_executable(ElfExcavator src/main.cpp)
target_link_libraries(ElfExcavator PRIVATE ElfExcavator.Core)

if (${PROJECT_NAME}_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()

# =========================
# Install
# =========================
#include(InstallRequiredSystemLibraries)
#install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT Libraries)
#set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5charts5, libqt5gui5, libqt5widgets5, libqt5qml5")
#include(CPack)
