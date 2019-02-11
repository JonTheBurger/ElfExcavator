#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

class BinUtils;
class SectionHeaderParser;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY(MainWindow)

public:
  explicit MainWindow(BinUtils* binUtils, SectionHeaderParser* sectionParser, QWidget* parent = nullptr);
  ~MainWindow() final;

protected:
  void changeEvent(QEvent* e) final;

private:
  Ui::MainWindow*      _ui;
  BinUtils*            _binUtils;
  SectionHeaderParser* _sectionParser;
};

#endif  // MAINWINDOW_HXX
