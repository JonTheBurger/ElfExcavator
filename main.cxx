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

  BinUtils                    binUtils;
  BinUtilsDialog              dialog(&binUtils);
  std::unique_ptr<MainWindow> window;

  dialog.show();
  QObject::connect(&dialog, &QDialog::accepted, [&window, &binUtils]() {
    auto w = std::make_unique<MainWindow>(&binUtils);
    std::swap(window, w);
    window->show();
  });

  return QApplication::exec();
}
