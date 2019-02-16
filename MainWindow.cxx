#include "MainWindow.hxx"

#include <QProcess>
#include <QSettings>

#include "BinUtils.hxx"
#include "MakeQChild.hxx"
#include "SectionHeader.hxx"
#include "SectionHeaderItemModel.hxx"
#include "Symbol.hxx"
#include "SymbolItemModel.hxx"
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
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue("MainWindow::size", size());
  settings.setValue("MainWindow::pos", pos());
  delete _ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
  QMainWindow::showEvent(e);

  auto* headerPs = new QProcess;
  connect(headerPs,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [this, headerPs](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
            {
              _sectionHeaders          = SectionHeader::ParseFromObjdump(headerPs->readAllStandardOutput());
              auto* sectionHeaderModel = MakeQChild<SectionHeaderItemModel, std::vector<SectionHeader>&>(_sectionHeaders, this);
              _ui->sectionHeaderTableView->setModel(sectionHeaderModel);
            }
            headerPs->deleteLater();
          });
  headerPs->start(_binUtils->Objdump(), { "-hw", _binUtils->ElfFile() });

  auto* symbolPs = new QProcess;
  connect(symbolPs,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [this, symbolPs](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
            {
              _symbolTable           = Symbol::ParseFromObjdump(symbolPs->readAllStandardOutput());
              auto* symbolTableModel = MakeQChild<SymbolItemModel, std::vector<Symbol>&>(_symbolTable, this);
              _ui->symbolTableTableView->setModel(symbolTableModel);
            }
            symbolPs->deleteLater();
          });
  symbolPs->start(_binUtils->Objdump(), { "-Ctw", _binUtils->ElfFile() });
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
