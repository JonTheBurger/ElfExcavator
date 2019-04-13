# Required for FetchContent
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(CTest)

# =========================
# Google Test
# =========================
# Do not run MOC/UIC/RCC on google test/mock sources
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# =========================
# Tests
# =========================
add_executable(Tests Tests.cpp)
target_link_libraries(Tests lib${PROJECT_NAME} gtest_main)
add_test(NAME Tests COMMAND Tests)
