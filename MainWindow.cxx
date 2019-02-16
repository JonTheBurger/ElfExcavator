#include "MainWindow.hxx"

#include <QProcess>
#include <QSettings>

#include "BinUtils.hxx"
#include "MakeQChild.hxx"
#include "SectionHeader.hxx"
#include "Symbol.hxx"
#include "ui_MainWindow.h"

MainWindow::MainWindow(BinUtils* binUtils, QWidget* parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _binUtils(binUtils)
{
  Q_ASSERT(_binUtils);
  _ui->setupUi(this);

  QSettings settings;
  resize(settings.value("MainWindow::size", size()).toSize());
  move(settings.value("MainWindow::pos", pos()).toPoint());

  auto* p = MakeQChild<QProcess>(this);
  connect(p,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
            &QProcess::finished),
          [this, p](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
            {
              QString output  = p->readAllStandardOutput();
              auto    headers = SectionHeader::ParseFromObjdump(output);
            }
          });
  //  Qt::ConnectionType::AutoConnection | Qt::ConnectionType::UniqueConnection
  p->start(_binUtils->Objdump(), { "-hw", _binUtils->ElfFile() });

  auto* syms = MakeQChild<QProcess>(this);
  connect(syms,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
            &QProcess::finished),
          [this, syms](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
            {
              QString output = syms->readAllStandardOutput();
              Symbol::ParseFromObjdump(output);
            }
          });
  syms->start(_binUtils->Objdump(), { "-Ctw", _binUtils->ElfFile() });
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue("MainWindow::size", size());
  settings.setValue("MainWindow::pos", pos());
  delete _ui;
}

void MainWindow::changeEvent(QEvent* e)
{
  QMainWindow::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      _ui->retranslateUi(this);
      break;
    default:
      break;
  }
}
