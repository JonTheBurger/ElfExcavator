#include <QApplication>
#include <QPushButton>
#include <QSettings>
#include <iostream>

#include "BinUtils.hxx"
#include "BinUtilsDialog.hxx"
#include "MainWindow.hxx"
#include "SectionHeader.hxx"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QApplication::setOrganizationName("jontheburger");
  QApplication::setApplicationName("elf-browser");
  QSettings settings;

  BinUtils       binUtils;
  MainWindow     window(&binUtils);
  BinUtilsDialog dialog(&binUtils);

  dialog.show();
  QObject::connect(&dialog, &QDialog::accepted, [&window]() { window.show(); });

  return QApplication::exec();
}
