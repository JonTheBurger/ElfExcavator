#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

class BinUtils;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)

public:
  explicit MainWindow(BinUtils* binUtils, QWidget* parent = nullptr);
  ~MainWindow() final;

protected:
  void changeEvent(QEvent* e) final;

private:
  Ui::MainWindow* _ui;
  BinUtils*       _binUtils;
};

#endif  // MAINWINDOW_HXX
