#include <QApplication>
#include <QPushButton>
#include <QSettings>

#include "BinUtils.hxx"
#include "BinUtilsDialog.hxx"
#include "MainWindow.hxx"
#include "SectionHeaderParser.hxx"

template <typename T, typename... Args>
T* MakeQObject(Args... args, QObject* parent)
{
  return new T(std::forward<Args>(args)..., parent);
}

template <typename T>
T* MakeQObject(QObject* parent)
{
  return new T(parent);
}

template <typename T>
T* MakeQObject(QWidget* parent)
{
  return new T(parent);
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QApplication::setOrganizationName("jontheburger");
  QApplication::setApplicationName("elf-browser");
  QSettings settings;

  SectionHeaderParser headerParser;
  BinUtils            binUtils;
  MainWindow          window(&binUtils, &headerParser);
  BinUtilsDialog      dialog(&binUtils);

  dialog.show();
  QObject::connect(&dialog, &QDialog::accepted, [&window]() { window.show(); });

  return QApplication::exec();
}
