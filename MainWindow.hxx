#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
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
  explicit MainWindow(BinUtils* binUtils, QWidget* parent = nullptr);
  ~MainWindow() final;

protected:
  void showEvent(QShowEvent* e) final;
  void changeEvent(QEvent* e) final;

private:
  Ui::MainWindow*            _ui;
  BinUtils*                  _binUtils;
  std::vector<SectionHeader> _sectionHeaders;
  std::vector<Symbol>        _symbolTable;
};

#endif  // MAINWINDOW_HXX
