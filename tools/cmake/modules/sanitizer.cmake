# Enable sanitizers for developers
option(${PROJECT_NAME}_USE_SANITIZERS "Turn on recommended sanitizers" ${${PROJECT_NAME}_DEVELOPER_MODE})
mark_as_advanced(${PROJECT_NAME}_USE_SANITIZERS)
if (NOT ${PROJECT_NAME}_USE_SANITIZERS)
  return()
endif()

if (CMAKE_C_COMPILER_ID STREQUAL MSVC)
  set(${PROJECT_NAME}_SANITIZERS
    /fsanitize=address
  )
else()
  set(${PROJECT_NAME}_SANITIZERS
    -fsanitize=address,leak,undefined -fno-omit-frame-pointer -fno-common
  )
  set(${PROJECT_NAME}_SANITIZERS_LINK
    -fsanitize=address,leak,undefined
  )
endif()
