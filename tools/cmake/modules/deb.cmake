# Add support for generating .deb package
#include(InstallRequiredSystemLibraries)
#install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT Libraries)
#set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5charts5, libqt5gui5, libqt5widgets5, libqt5qml5")
#include(CPack)
