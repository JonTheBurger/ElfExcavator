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
message("FETCHING MISSING DEPENDENCIES...")
include(FetchContent)
set(FETCH_CACHE "${PROJECT_SOURCE_DIR}/.cache/_deps")
if (NOT EXISTS ${FETCH_CACHE})
  file(MAKE_DIRECTORY ${FETCH_CACHE})
endif()

# Basically nobody has this as a package
set(FETCH_DEFAULT OFF)
if (NOT TARGET qhexview-lib)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_QHEXVIEW "Fetch dependency QHexView" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_QHEXVIEW)
if (${PROJECT_NAME}_FETCH_QHEXVIEW)
  FetchContent_Declare(
    QHexView
    GIT_REPOSITORY https://github.com/Dax89/QHexView.git
    GIT_TAG b3c0e4a8b7318f3c6ed8a29a75694b376ba16cf1
    GIT_SHALLOW TRUE
    SOURCE_DIR ${FETCH_CACHE}/QHexView
  )
  FetchContent_GetProperties(QHexView)
  if (NOT QHexView_POPULATED)
    find_package(Git)
    if (GIT_FOUND)
      execute_process(
        COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/QHexView.patch
        WORKING_DIRECTORY "${QHexView_SOURCE_DIR}"
      )
    endif()
    FetchContent_Populate(QHexView)
    add_subdirectory(${QHexView_SOURCE_DIR} ${QHexView_BINARY_DIR})
  endif()
endif()

set(FETCH_DEFAULT OFF)
if (NOT TARGET Catch2::Catch2WithMain AND ${PROJECT_NAME}_BUILD_TESTING)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_CATCH2 "Fetch dependency Catch2" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_CATCH2)
if (${PROJECT_NAME}_FETCH_CATCH2)
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

set(FETCH_DEFAULT OFF)
if (NOT TARGET elfio::elfio)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_ELFIO "Fetch dependency ELFIO" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_ELFIO)
if (${PROJECT_NAME}_FETCH_ELFIO)
  set(ELFIO_BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
  FetchContent_Declare(
    ELFIO
    GIT_REPOSITORY https://github.com/serge1/ELFIO.git
    GIT_TAG Release_3.9
    SOURCE_DIR ${FETCH_CACHE}/ELFIO
  )
  FetchContent_MakeAvailable(ELFIO)
endif()

set(FETCH_DEFAULT OFF)
if (NOT TARGET KF5::ItemModels)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_KITEMMODELS "Fetch dependency kitemmodels" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_KITEMMODELS)
if (${PROJECT_NAME}_FETCH_KITEMMODELS)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    kitemmodels
    GIT_REPOSITORY https://github.com/KDE/kitemmodels.git
    GIT_TAG v5.89.0
    SOURCE_DIR ${FETCH_CACHE}/kitemmodels
  )
  FetchContent_MakeAvailable(kitemmodels)
endif()

# Fetching the entirety of LLVM would be prohibitively expensive
set(FETCH_DEFAULT OFF)
if (NOT TARGET LLVMDemangle)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_USE_BUNDLED_LLVM "Use bundled LLVMDemangle" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_USE_BUNDLED_LLVM)
if (${PROJECT_NAME}_USE_BUNDLED_LLVM)
  add_subdirectory(llvm)
endif()

set(FETCH_DEFAULT OFF)
if (NOT TARGET ads::qtadvanceddocking)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_QT_ADVANCED_DOCKING_SYSTEM "Fetch dependency Qt-Advanced-Docking-System" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_QT_ADVANCED_DOCKING_SYSTEM)
if (${PROJECT_NAME}_FETCH_QT_ADVANCED_DOCKING_SYSTEM)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  if (NOT BUILD_SHARED_LIBS)
    set(BUILD_STATIC ON CACHE BOOL "" FORCE)
    find_package(X11)
  endif()
  FetchContent_Declare(
    Qt-Advanced-Docking-System
    GIT_REPOSITORY https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System.git
    GIT_TAG 3.8.0
    SOURCE_DIR ${FETCH_CACHE}/Qt-Advanced-Docking-System
  )
  FetchContent_MakeAvailable(Qt-Advanced-Docking-System)
  if (BUILD_SHARED)
    # If we couldn't find this package on the system and we're
    # building shared libs we need to install it ourselves.
    install(TARGETS qtadvanceddocking)
  elseif (X11_xcb_xkb_FOUND AND BUILD_STATIC)
    # The STATIC Qt ADS is missing this required link dependency on linux
    # It can be installed via 'sudo apt-get install libxkbcommon-dev'
    # This might not be needed for Wayland, but I have not tested,
    # which is why find_package(X11) is not REQUIRED
    target_link_libraries(qtadvanceddocking INTERFACE X11::xcb_xkb)
  endif()
  add_library(ads::qtadvanceddocking ALIAS qtadvanceddocking)
endif()

set(FETCH_DEFAULT OFF)
if (NOT TARGET range-v3::range-v3)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_RANGE_V3 "Fetch dependency range-v3" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_RANGE_V3)
if (${PROJECT_NAME}_FETCH_RANGE_V3)
  FetchContent_Declare(
    range-v3
    GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
    GIT_TAG 0.11.0
    SOURCE_DIR ${FETCH_CACHE}/range-v3
  )
  FetchContent_MakeAvailable(range-v3)
endif()

set(FETCH_DEFAULT OFF)
if (NOT TARGET spdlog::spdlog)
  set(FETCH_DEFAULT ON)
endif()
option(${PROJECT_NAME}_FETCH_SPDLOG "Fetch dependency spdlog" ${FETCH_DEFAULT})
mark_as_advanced(${PROJECT_NAME}_FETCH_SPDLOG)
if (${PROJECT_NAME}_FETCH_SPDLOG)
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
    SOURCE_DIR ${FETCH_CACHE}/spdlog
  )
  FetchContent_MakeAvailable(spdlog)
endif()

message("FETCHING MISSING DEPENDENCIES: DONE")
