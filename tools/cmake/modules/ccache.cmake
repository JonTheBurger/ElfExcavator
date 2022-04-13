# Enable ccache for developers
option(${PROJECT_NAME}_USE_CCACHE "Turn on ccache" ${${PROJECT_NAME}_DEVELOPER_MODE})
mark_as_advanced(${PROJECT_NAME}_USE_CCACHE)
if (NOT ${PROJECT_NAME}_USE_CCACHE)
  return()
endif()

find_program(CCACHE_EXECUTABLE ccache)
if (CCACHE_EXECUTABLE)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_EXECUTABLE}")
endif()
