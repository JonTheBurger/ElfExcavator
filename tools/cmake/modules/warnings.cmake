# Enable compiler warnings for developers
option(${PROJECT_NAME}_USE_WARNINGS "Turn on recommended compiler warnings" ${${PROJECT_NAME}_DEVELOPER_MODE})
mark_as_advanced(${PROJECT_NAME}_USE_WARNINGS)
if (NOT ${PROJECT_NAME}_USE_WARNINGS)
  return()
endif()

if (CMAKE_C_COMPILER_ID STREQUAL MSVC)
  set(${PROJECT_NAME}_WARNINGS
    /W4 /permissive-
    /w14242 /w14254 /w14263 /w14265 /w14287 /w14296
    /w14311
    /w14545 /w14546 /w14547 /w14549 /w14555
    /w14619 /w14640
    /w14826
    /w14905 /w14906 /w14928
    /we4289
  )
elseif(CMAKE_C_COMPILER_ID MATCHES Clang)
  set(${PROJECT_NAME}_WARNINGS
    -Weverything
  )
else()
  set(${PROJECT_NAME}_WARNINGS
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
    -Woverloaded-virtual
    -Wpedantic
    -Wshadow
    -Wsign-conversion
    -Wunused
    -Wuseless-cast
  )
endif()
