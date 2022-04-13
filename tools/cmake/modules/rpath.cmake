# Set relative .so/.dylib lookup, enabling relocatable binaries
file(RELATIVE_PATH rpath
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
)
if (APPLE)
  set(originPath @loader_path)
else()
  set(originPath $ORIGIN)
endif()
set(CMAKE_INSTALL_RPATH ${originPath} ${originPath}/${rpath})
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
