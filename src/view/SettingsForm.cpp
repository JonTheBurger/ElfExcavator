#include "SettingsForm.hpp"

#include <QFileDialog>
#include <QPushButton>
#include <QTimer>
#include <chrono>

#include "presenter/SettingsPresenter.hpp"
#include "ui_SettingsForm.h"

struct SettingsForm::Impl {
  Q_DISABLE_COPY(Impl)

  SettingsForm&      self;
  Ui::SettingsForm   ui;
  SettingsPresenter& presenter;
  QTimer             elf_file_debounce_timer;
  QTimer             objdump_debounce_timer;

  static constexpr std::chrono::milliseconds DEBOUNCE_TIME{ 1000 };

  explicit Impl(SettingsForm& that, SettingsPresenter& present) noexcept
      : self{ that }
      , ui{}
      , presenter{ present }
      , elf_file_debounce_timer{}
      , objdump_debounce_timer{}
  {
    ui.setupUi(&self);

    // Show File Browser on elf_file_browse_button click
    connect(ui.elf_file_browse_button, &QPushButton::clicked, [this]() {
      ui.elf_file_lineedit->setText(QFileDialog::getOpenFileName(&self, tr("ELF File"), "", tr("All File Types (*);;ELF files (*.elf *.o *.so *.out *.axf *.pxf *.puff *.ko *.mod)")));
    });

    // Show File Browser on objdump_browse_button click
    connect(ui.objdump_browse_button, &QPushButton::clicked, [this]() {
      ui.objdump_lineedit->setText(QFileDialog::getOpenFileName(&self, tr("objdump executable"), QStringLiteral("objdump"), tr("All File Types (*)")));
    });

    // Wait until 100ms after the user has stopped modifying the elf_file_lineedit to register a change
    connect(ui.elf_file_lineedit, &QLineEdit::textChanged, [this]() {
      elf_file_debounce_timer.start(DEBOUNCE_TIME);
    });

    elf_file_debounce_timer.setSingleShot(true);
    connect(&elf_file_debounce_timer, &QTimer::timeout, [this]() {
      presenter.setElfFile(ui.elf_file_lineedit->text());
    });

    // Wait until 100ms after the user has stopped modifying the objdump_lineedit to register a change
    connect(ui.objdump_lineedit, &QLineEdit::textChanged, [this]() {
      objdump_debounce_timer.start(DEBOUNCE_TIME);
    });

    objdump_debounce_timer.setSingleShot(true);
    connect(&objdump_debounce_timer, &QTimer::timeout, [this]() {
      presenter.setElfFile(ui.objdump_lineedit->text());
    });

    //
    connect(&presenter, &SettingsPresenter::elfFileChanged, [this](const QString& elf_file, bool is_valid) {
      static_cast<void>(elf_file);
      static const QString WARNING_CHAR = QStringLiteral("⚠ ");

      QString text = ui.elf_file_label->text();

      if ((is_valid) && (text.startsWith(WARNING_CHAR)))
      {
        text.remove(WARNING_CHAR);
      }
      else if ((!is_valid) && (!text.startsWith(WARNING_CHAR)))
      {
        text.insert(0, WARNING_CHAR);
      }

      ui.elf_file_label->setText(std::move(text));
    });
  }
};

SettingsForm::SettingsForm(SettingsPresenter& presenter, QWidget* parent)
    : QWidget(parent)
    , _self{ std::make_unique<Impl>(*this, presenter) }
{
}

SettingsForm::~SettingsForm() = default;
