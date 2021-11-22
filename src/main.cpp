#include <QApplication>
#include <QCommandLineParser>
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
  // Qt Framework
  QApplication app(argc, argv);
  QApplication::setOrganizationName("jontheburger");
  QApplication::setApplicationName("Elf Excavator");
  QApplication::setApplicationVersion("alpha");

  QCommandLineParser parser;
  QCommandLineOption file_opt{
    "file",
    QCoreApplication::tr("ELF file to visualize"),
    QCoreApplication::tr("ELF File Path")
  };
  parser.setApplicationDescription("Visualizes ELF files with an easy to use graphical interface");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOption(file_opt);
  parser.process(app);
  QString elf_file_path = parser.isSet(file_opt) ? parser.value(file_opt) : argv[0];

  // Models
  ElfFile elf_file;

  // Presenters
  SectionHeaderItemModel section_header_item_model(elf_file);
  SettingsPresenter      settings_presenter(elf_file);
  SymbolTableItemModel   symbol_table_item_model(elf_file);

  MainPresenter main_presenter(section_header_item_model,
                               settings_presenter,
                               symbol_table_item_model);

  // Views
  MainWindow main_window(main_presenter);

  // Finalize
  if (!elf_file_path.isEmpty())
  {
    settings_presenter.setElfFile(elf_file_path);
  }
  main_window.show();

  return QApplication::exec();
}
