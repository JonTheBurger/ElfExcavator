#pragma once

#include <QWidget>
#include <memory>

class SettingsPresenter;

class SettingsForm final : public QWidget {
  Q_OBJECT
  Q_DISABLE_COPY(SettingsForm)

public:
  explicit SettingsForm(SettingsPresenter& presenter, QWidget* parent = nullptr);
  ~SettingsForm() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
