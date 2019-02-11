#include "MainWindow.hxx"

#include <QProcess>
#include <QSettings>

#include "BinUtils.hxx"
#include "SectionHeaderParser.hxx"
#include "ui_MainWindow.h"

MainWindow::MainWindow(BinUtils* binUtils, SectionHeaderParser* sectionParser, QWidget* parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _binUtils(binUtils)
    , _sectionParser(sectionParser)
{
  Q_ASSERT(_binUtils);
  Q_ASSERT(_sectionParser);
  _ui->setupUi(this);

  QSettings settings;
  resize(settings.value("MainWindow::size", size()).toSize());
  move(settings.value("MainWindow::pos", pos()).toPoint());

  auto* p = new QProcess();
  connect(p,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
          &QProcess::finished),
          [this, p](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
            {
              QString output  = p->readAllStandardOutput();
              auto    headers = SectionHeaderParser::ParseFromObjdump(output);
            }
          });
  //  Qt::ConnectionType::AutoConnection | Qt::ConnectionType::UniqueConnection
  p->start(_binUtils->Objdump(), { "-hw", "a.out" });
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
