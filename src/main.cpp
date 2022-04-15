#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QItemSelectionModel>
#include <QSettings>
#include <QTranslator>
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
  Q_INIT_RESOURCE(Resources);
  Q_INIT_RESOURCE(i18n);

  QTranslator translator;
  if (translator.load(QLocale(), QLatin1String("elfexcavator"), QLatin1String("."), QLatin1String(":/i18n")))
  {
    app.installTranslator(&translator);
  }

  QCommandLineParser parser;
  parser.addPositionalArgument(
    QStringLiteral("file"),
    QCoreApplication::tr("ELF file to visualize"),
    QStringLiteral(""));
  parser.setApplicationDescription(
    QCoreApplication::tr("Visualizes ELF files with an easy to use graphical interface"));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(app);

  QString elf_file_path = argv[0];
  if (!parser.positionalArguments().empty())
  {
    elf_file_path = parser.positionalArguments()[0];
  }

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
    main_window.setWindowTitle(elf_file_path);
  }
  main_window.show();

  return QApplication::exec();
}
