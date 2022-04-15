# Set relative .so/.dylib lookup, enabling relocatable binaries
set(origin $ORIGIN)
if (APPLE)
  set(origin @loader_path)
endif()
file(RELATIVE_PATH rpath
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_INSTALL_RPATH ${origin} ${origin}/${rpath})
