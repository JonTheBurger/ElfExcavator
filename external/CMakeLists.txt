# Automatically resolve any dependencies not installed on the system (unless asked not to)
if (${PROJECT_NAME}_PACKAGER_MODE)
  return()
endif()

# Disable warnings for dependencies
if (CMAKE_C_COMPILER STREQUAL MSVC)
  add_compile_options(/w)
else()
  add_compile_options(-w)
endif()

# Use common cache folder that can be shared between configurations
include(FetchContent)
set(FETCH_CACHE "${PROJECT_SOURCE_DIR}/.cache/_deps")
if (NOT EXISTS ${FETCH_CACHE})
  file(MAKE_DIRECTORY ${FETCH_CACHE})
endif()

# Basically nobody has this as a package
if (NOT TARGET qhexview-lib)
  FetchContent_Declare(
    QHexView
    GIT_REPOSITORY https://github.com/Dax89/QHexView.git
    GIT_TAG b3c0e4a8b7318f3c6ed8a29a75694b376ba16cf1
    GIT_SHALLOW TRUE
    SOURCE_DIR ${FETCH_CACHE}/QHexView
  )
  FetchContent_MakeAvailable(QHexView)
endif()

if (NOT TARGET Catch2::Catch2WithMain AND ${PROJECT_NAME}_BUILD_TESTING)
  set(CATCH_BUILD_STATIC_LIBRARY ON)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_BUILD_WERROR OFF)
  set(CATCH_INSTALL_DOCS OFF)
  set(CATCH_INSTALL_HELPERS OFF)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.7
    SOURCE_DIR ${FETCH_CACHE}/Catch2
  )
  FetchContent_MakeAvailable(Catch2)
endif()

if (NOT TARGET elfio::elfio)
  set(ELFIO_BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
  FetchContent_Declare(
    ELFIO
    GIT_REPOSITORY https://github.com/serge1/ELFIO.git
    GIT_TAG Release_3.9
    SOURCE_DIR ${FETCH_CACHE}/ELFIO
  )
  FetchContent_MakeAvailable(ELFIO)
endif()

if (NOT TARGET KF5::ItemModels)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    kitemmodels
    GIT_REPOSITORY https://github.com/KDE/kitemmodels.git
    GIT_TAG v5.68.0
    SOURCE_DIR ${FETCH_CACHE}/kitemmodels
  )
  FetchContent_MakeAvailable(kitemmodels)
endif()

# Fetching the entirety of LLVM would be prohibitively expensive
if (NOT TARGET LLVMDemangle)
  add_subdirectory(llvm)
endif()

if (NOT TARGET qtadvanceddocking)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    Qt-Advanced-Docking-System
    GIT_REPOSITORY https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System.git
    GIT_TAG 3.8.0
    SOURCE_DIR ${FETCH_CACHE}/Qt-Advanced-Docking-System
  )
  FetchContent_MakeAvailable(Qt-Advanced-Docking-System)
endif()

if (NOT TARGET range-v3::range-v3)
  FetchContent_Declare(
    range-v3
    GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
    GIT_TAG 0.11.0
    SOURCE_DIR ${FETCH_CACHE}/range-v3
  )
  FetchContent_MakeAvailable(range-v3)
endif()

if (NOT TARGET spdlog::spdlog)
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
    SOURCE_DIR ${FETCH_CACHE}/spdlog
  )
  FetchContent_MakeAvailable(spdlog)
endif()
