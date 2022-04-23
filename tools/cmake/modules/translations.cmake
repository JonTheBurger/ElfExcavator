# qt5_create_translation doesn't work in a subdirectory
set(TS_FILES
  ${PROJECT_SOURCE_DIR}/data/i18n/elfexcavator.de_DE.ts
)
qt5_create_translation(QM_FILES ${PROJECT_SOURCE_DIR}/src ${TS_FILES})
set(I18N_QRC "${PROJECT_BINARY_DIR}/i18n.qrc")
file(WRITE ${I18N_QRC} "<!DOCTYPE RCC><RCC version=\"1.0\">\n  <qresource prefix=\"i18n\">\n")
foreach(file IN LISTS QM_FILES)
  file(RELATIVE_PATH filename ${PROJECT_BINARY_DIR} ${file})
  file(APPEND ${I18N_QRC} "    <file alias=\"${filename}\">${file}</file>\n")
endforeach()
file(APPEND ${I18N_QRC} "  </qresource>\n</RCC>\n")

list(APPEND QM_FILES "${I18N_QRC}")
