#pragma once

#include <QMainWindow>
#include <memory>

class MainPresenter;

class MainWindow final : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)

public:
  explicit MainWindow(MainPresenter& present, QWidget* parent = nullptr);
  ~MainWindow() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
