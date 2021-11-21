if (CMAKE_C_COMPILER_ID STREQUAL GNU OR CMAKE_C_COMPILER_ID MATCHES Clang)
  add_compile_options(-w)
endif()

if (${PROJECT_NAME}_BUILD_TESTING)
  set(CATCH_BUILD_STATIC_LIBRARY ON)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_BUILD_WERROR OFF)
  set(CATCH_INSTALL_DOCS OFF)
  set(CATCH_INSTALL_HELPERS OFF)
  add_subdirectory(Catch2)
endif()

set(ELFIO_BUILD_EXAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory(ELFIO)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(kitemmodels)

add_subdirectory(llvm)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Qt-Advanced-Docking-System)

add_subdirectory(range-v3)

add_subdirectory(spdlog)
