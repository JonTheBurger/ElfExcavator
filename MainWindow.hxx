#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <memory>
#include <vector>

#include "SectionHeader.hxx"
#include "Symbol.hxx"

class BinUtils;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)

public:
  enum Tab {
    SECTIONS,
    SYMBOLS,

    MAX,
  };

  explicit MainWindow(BinUtils* binUtils, QWidget* parent = nullptr);
  ~MainWindow() final;

protected:
  void showEvent(QShowEvent* e) final;
  void changeEvent(QEvent* e) final;

private:
  class MainWindowPrivate;
  const std::unique_ptr<MainWindowPrivate> _impl;
};

#endif  // MAINWINDOW_HXX
