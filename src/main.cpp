#include <QApplication>
#include <QItemSelectionModel>
#include <QSettings>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "model/ElfFile.hpp"
#include "presenter/MainPresenter.hpp"
#include "presenter/SectionHeaderItemModel.hpp"
#include "presenter/SettingsPresenter.hpp"
#include "presenter/SymbolTableItemModel.hpp"
#include "view/MainWindow.hpp"

int main(int argc, char* argv[])
{
  // Qt Core Application
  QApplication app(argc, argv);
  QApplication::setOrganizationName("jontheburger");
  QApplication::setApplicationName("Elf Excavator");

  // Models
  ElfFile elf_file;
  elf_file.load("/home/vagrant/Projects/qobjdump-gui/build/Desktop-Debug/bin/ElfExcavator");

  // Presenters
  SectionHeaderItemModel section_header_item_model(elf_file);
  SettingsPresenter      settings_presenter(elf_file);
  SymbolTableItemModel   symbol_table_item_model(elf_file);

  MainPresenter main_presenter(section_header_item_model,
                               settings_presenter,
                               symbol_table_item_model);

  // Views
  MainWindow main_window(main_presenter);
  main_window.show();

  return QApplication::exec();
}
