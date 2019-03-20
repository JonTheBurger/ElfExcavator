#include "BinUtilsDialog.hxx"

#include <QFileDialog>
#include <QSettings>
#include <iostream>

#include "BinUtils.hxx"
#include "ui_BinUtilsDialog.h"

// TODO: Add OK/Not Ok checkmarks for binutils
BinUtilsDialog::BinUtilsDialog(BinUtils* binUtils, QWidget* parent)
    : _binUtils(binUtils)
    , QDialog(parent)
    , _ui(new Ui::BinUtilsDialog)
{
  _ui->setupUi(this);

  Q_ASSERT(_binUtils != nullptr);
  QSettings settings;
  resize(settings.value("BinUtilsDialog::size", size()).toSize());
  move(settings.value("BinUtilsDialog::pos", pos()).toPoint());

  connect(_ui->continueButton, &QPushButton::clicked, [this]() { accept(); });

  _ui->objdumpLineEdit->setText(_binUtils->Objdump());
  connect(_ui->objdumpButton, &QPushButton::clicked, [this]() {
    this->_ui->objdumpLineEdit->setText(QFileDialog::getOpenFileName(this, tr("objdump executable"), "objdump", tr("All File Types (*)")));
  });
  connect(_ui->objdumpLineEdit, &QLineEdit::textChanged, [this](const QString& text) {
    if (_binUtils->SetObjdump(text))
    {
      _ui->continueButton->setEnabled(true);
    }
  });

  _ui->elfLineEdit->setText(_binUtils->ElfFile());
  connect(_ui->elfButton, &QPushButton::clicked, [this]() {
    this->_ui->elfLineEdit->setText(QFileDialog::getOpenFileName(this, tr("ELF file"), "", tr("All File Types (*);;ELF files (*.elf *.o *.so *.out *.axf *.pxf *.puff *.ko *.mod)")));
  });
  connect(_ui->elfLineEdit, &QLineEdit::textChanged, [this](const QString& text) {
    if (_binUtils->SetElfFile(text))
    {
      _ui->continueButton->setEnabled(true);
    }
  });

  // TODO: Check valid any
  _ui->continueButton->setEnabled(!_binUtils->ElfFile().isEmpty() && !_binUtils->Objdump().isEmpty());
}

BinUtilsDialog::~BinUtilsDialog()
{
  QSettings settings;
  settings.setValue("BinUtilsDialog::size", size());
  settings.setValue("BinUtilsDialog::pos", pos());
  delete _ui;
}

void BinUtilsDialog::changeEvent(QEvent* e)
{
  QDialog::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      _ui->retranslateUi(this);
      break;
    default:
      break;
  }
}
