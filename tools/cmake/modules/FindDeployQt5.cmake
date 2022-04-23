get_target_property(Qt5_qmake_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
execute_process(
  COMMAND "${Qt5_qmake_EXECUTABLE}" -query QT_INSTALL_PREFIX
  OUTPUT_VARIABLE QT5_INSTALL_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
find_program(Qt5_deployqt_EXECUTABLE
  NAMES
    windeployqt
    macdeployqt
    linuxdeployqt
  HINTS
    "${QT5_INSTALL_PREFIX}"
  PATH_SUFFIXES
    bin
    tools/qt5/bin
  NO_CACHE
)
add_executable(Qt5::deployqt IMPORTED)
get_filename_component(Qt5_BINARY_DIR "${Qt5_deployqt_EXECUTABLE}" DIRECTORY)
set_target_properties(Qt5::deployqt PROPERTIES IMPORTED_LOCATION "${Qt5_deployqt_EXECUTABLE}")

function(deploy_qt5 TARGET)
  set(DEPLOY_SCRIPT [[
    execute_process(
      COMMAND "@Qt5_deployqt_EXECUTABLE@" --release --verbose 2 --ignore-library-errors --dir "${CMAKE_INSTALL_PREFIX}/@CMAKE_INSTALL_BINDIR@" $<TARGET_FILE:@TARGET@>
      WORKING_DIRECTORY "@Qt5_BINARY_DIR@"
      OUTPUT_VARIABLE output
      COMMAND_ECHO STDOUT
    )
    message("${output}")
    file(GET_RUNTIME_DEPENDENCIES
      LIBRARIES $<TARGET_FILE:@TARGET@>
      EXECUTABLES $<TARGET_FILE:@TARGET@>
      RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
      UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
    )
    foreach(filepath ${RESOLVED_DEPS})
      get_filename_component(filename "${filepath}" NAME)
      message(STATUS "Installing: ${CMAKE_INSTALL_PREFIX}/@CMAKE_INSTALL_BINDIR@/${filename}")
      file(INSTALL
        FILES "${filepath}"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/@CMAKE_INSTALL_BINDIR@"
        TYPE SHARED_LIBRARY
        FOLLOW_SYMLINK_CHAIN
      )
    endforeach()
    if (UNRESOLVED_DEPS)
      message(WARNING "Unresolved dependencies: ${UNRESOLVED_DEPS}")
    endif()
  ]])
  string(CONFIGURE "${DEPLOY_SCRIPT}" DEPLOY_SCRIPT @ONLY)
  install(CODE "${DEPLOY_SCRIPT}")
endfunction()