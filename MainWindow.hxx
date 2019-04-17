#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <memory>
#include <vector>

#include "CircularIterator.hxx"
#include "Palette.hxx"
#include "SectionHeader.hxx"
#include "Symbol.hxx"

class BinUtils;
class DisassemblyHighlighter;
class QItemSelection;
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
  void changeEvent(QEvent* e) final;

private:
  void OnTabChanged(const MainWindow::Tab tab);
  void OnSelectedSectionHeaderChanged(const QItemSelection& selected, const QItemSelection& deselected);
  void OnShowHeaderChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
  void OnSelectedSymbolChanged(const QItemSelection& selected, const QItemSelection& deselected);
  void OnShowSymbolChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

  std::unique_ptr<Ui::MainWindow>                       _ui;
  BinUtils&                                             _binUtils;
  std::vector<SectionHeader>                            _sectionHeaders;
  std::vector<Symbol>                                   _symbolTable;
  std::unique_ptr<DisassemblyHighlighter>               _highlighter;
  circular_iterator<decltype(Palette::COLORS.cbegin())> _colorIter;
};

#endif  // MAINWINDOW_HXX
