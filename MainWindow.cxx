#include "MainWindow.hxx"

#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QtCharts>
#include <algorithm>

#include "BinUtils.hxx"
#include "DisassemblyHighlighter.hxx"
#include "MultiFilterProxyModel.hxx"
#include "SectionHeader.hxx"
#include "SectionHeaderItemModel.hxx"
#include "Symbol.hxx"
#include "SymbolItemModel.hxx"
#include "ui_MainWindow.h"

// https://doc.qt.io/qt-5.9/classes.html
/// TODO: Forward/back button support (goto last symbol)
/// TODO: Plug the leaks
/// TODO: Refactory into PieChart
/// TODO: Options Menu (25 count, chart theme, chart animations, regex options)
/// TODO: Uncheck all
/// TODO: Installer
/// TODO: Help, manual, video, etc.https://www.walletfox.com/course/qhelpengineexample.php
/// TODO: Translations
/// TODO: Clean up cmake structure
/// ? Split assembly and source
/// ? Click on function to navigate
/// ? Call graph using node editor

MainWindow::MainWindow(BinUtils* binUtils, QWidget* parent)
    : QMainWindow{ parent }
    , _ui{ std::make_unique<Ui::MainWindow>() }
    , _binUtils{ *binUtils }
    , _highlighter{ std::make_unique<DisassemblyHighlighter>(nullptr) }
    , _colorIter{ Palette::COLORS.cbegin(), Palette::COLORS.cend() }
{
  Q_ASSERT(binUtils);
  _ui->setupUi(this);

  QSettings settings;
  resize(settings.value("MainWindow::size", size()).toSize());
  move(settings.value("MainWindow::pos", pos()).toPoint());

  SetupChart();
  _highlighter->setDocument(_ui->disassemblyTextBrowser->document());

  connect(_ui->tabWidget, &QTabWidget::currentChanged, [this](int tab) {
    OnTabChanged(static_cast<Tab>(tab));
  });
  connect(_ui->zoomInButton, &QPushButton::pressed, _ui->chartView, &PieChart::zoomIn);
  connect(_ui->zoomOutButton, &QPushButton::pressed, _ui->chartView, &PieChart::zoomOut);

  _ui->disassemblySearchLineEdit->setTextBrowser(_ui->disassemblyTextBrowser);

  _binUtils.ExecObjdump(
    { "-hw", _binUtils.ElfFile() },
    [this](const QString& out) {
      _sectionHeaders          = SectionHeader::ParseFromObjdump(out);
      auto* sectionHeaderModel = new SectionHeaderItemModel(_sectionHeaders, this);
      _ui->sectionHeaderTableView->setModel(sectionHeaderModel);

      connect(sectionHeaderModel, &QAbstractItemModel::dataChanged, this, &MainWindow::OnShowHeaderChanged);
      connect(_ui->sectionHeaderTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::OnSelectedSectionHeaderChanged);

      OnTabChanged(static_cast<Tab>(_ui->tabWidget->currentIndex()));
    });

  _binUtils.ExecObjdump(
    { "-Ctw", _binUtils.ElfFile() },
    [this](const QString& out) {
      _symbolTable           = Symbol::ParseFromObjdump(out);
      auto* symbolTableModel = new SymbolItemModel(_symbolTable, this);
      _ui->symbolTableTableView->setModel(symbolTableModel);

      connect(symbolTableModel, &QAbstractItemModel::dataChanged, this, &MainWindow::OnShowSymbolChanged);
      connect(_ui->symbolTableTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::OnSelectedSymbolChanged);
    });
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue("MainWindow::size", size());
  settings.setValue("MainWindow::pos", pos());
}

void MainWindow::SetupChart()
{
  auto* chart = new QChart();  /// Owned by chartView when setChart is called
  chart->legend()->hide();
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->setBackgroundBrush(palette().color(QPalette::Normal, QPalette::Window));
  _ui->chartView->setChart(chart);
  _ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::OnTabChanged(const MainWindow::Tab tab)
{
  auto* series = new QPieSeries();  /// Owned by QChart when addSeries is called
  series->setUseOpenGL(true);

  if (tab == MainWindow::Tab::SECTIONS)
  {
    for (auto& section : _sectionHeaders)
    {
      if (section.Display)
      {
        QPieSlice* const slice = series->append(section.Name, section.Size);
        slice->setColor(**_colorIter++);
      }
    }

    _ui->chartView->chart()->setTitle(tr("Section Headers"));
    _ui->chartView->chart()->setTitleBrush(palette().color(QPalette::Normal, QPalette::Text));
    _highlighter->Enabled = false;
    connect(series, &QPieSeries::clicked, [this](QPieSlice* slice) {
      auto it = std::find_if(_sectionHeaders.begin(), _sectionHeaders.end(), [&slice](auto&& header) { return header.Display && (header.Name == slice->label()); });
      if (it != _sectionHeaders.end())
      {
        auto index = static_cast<MultiFilterProxyModel*>(_ui->sectionHeaderTableView->model())->sourceModel()->index(it->Index, 0);
        index      = static_cast<MultiFilterProxyModel*>(_ui->sectionHeaderTableView->model())->mapFromSource(index);
        _ui->sectionHeaderTableView->setCurrentIndex(index);
      }
    });
  }
  else if (tab == MainWindow::Tab::SYMBOLS)
  {
    for (auto& symbol : _symbolTable)
    {
      if (symbol.Display)
      {
        QPieSlice* const slice = series->append(symbol.Name, symbol.Size);
        slice->setColor(**_colorIter++);
      }
    }

    _ui->chartView->chart()->setTitle(tr("Symbols"));
    _ui->chartView->chart()->setTitleBrush(palette().color(QPalette::Normal, QPalette::Text));
    _highlighter->Enabled = true;
    connect(series, &QPieSeries::clicked, [this](QPieSlice* slice) {
      auto it = std::find_if(_symbolTable.begin(), _symbolTable.end(), [&slice](auto&& header) { return header.Display && (header.Name == slice->label()); });
      if (it != _symbolTable.end())
      {
        auto index = static_cast<MultiFilterProxyModel*>(_ui->symbolTableTableView->model())->sourceModel()->index(it->Index, 0);
        index      = static_cast<MultiFilterProxyModel*>(_ui->symbolTableTableView->model())->mapFromSource(index);
        _ui->symbolTableTableView->setCurrentIndex(index);
      }
    });
  }

  _ui->chartView->chart()->removeAllSeries();
  _ui->chartView->chart()->addSeries(series);  // WARNING: Memory leak?
  connect(series, &QPieSeries::hovered, [this](QPieSlice* slice, bool state) {
    slice->setLabelVisible(state);
    slice->setLabelBrush(palette().color(QPalette::Normal, QPalette::Text));
  });
}

void MainWindow::OnSelectedSectionHeaderChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  // TODO: Use event args
  const auto*  selection = _ui->sectionHeaderTableView->selectionModel();
  const auto&& indices   = selection->selectedIndexes();
  if (indices.empty()) { return; }
  _ui->statusbar->showMessage(tr("%1 rows selected").arg(selection->selectedRows().count()));

  // WARNING: remove cast? Add mapToSource to TableView.
  const size_t   row         = static_cast<MultiFilterProxyModel*>(_ui->sectionHeaderTableView->model())->mapToSource(indices.first()).row();
  const QString& sectionName = _sectionHeaders.at(row).Name;

  //  if (_sectionHeaders[row].Display)
  //  {
  //    _sectionModel->mapToSource(indices.first()).data(Qt::CheckStateRole);
  //    auto&& slices = static_cast<QPieSeries*>(_ui->chartView->chart()->series().first())->slices();
  //    auto   it     = std::find_if(slices.begin(), slices.end(), [&sectionName](QPieSlice* s) { return s->label() == sectionName; });
  //    if (it != slices.end())
  //    {
  //      (*it)->setExploded(true);
  //    }
  //  }

  _binUtils.ExecObjdump(
    { "-sj", sectionName, _binUtils.ElfFile() },
    [this](const QString& out) {
      _ui->disassemblyTextBrowser->setText(out);
    });
}

void MainWindow::OnShowHeaderChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  (void)roles;
  auto* const series = static_cast<QPieSeries*>(_ui->chartView->chart()->series().first());
  auto&&      slices = series->slices();
  for (int i = bottomRight.row(); i >= topLeft.row(); --i)
  {
    if (_sectionHeaders.at(i).Display)  // add
    {
      auto* slice = series->append(_sectionHeaders[i].Name, _sectionHeaders[i].Size);
      slice->setLabelVisible();
      slice->setColor(**_colorIter++);
    }
    else  // remove
    {
      auto it = std::find_if(slices.begin(), slices.end(), [this, i](QPieSlice* s) { return s->label() == _sectionHeaders[i].Name; });
      if (it != slices.end())
      {
        series->remove(*it);
      }
    }
  }
  _ui->chartView->update();
}

void MainWindow::OnSelectedSymbolChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  const auto*  selection = _ui->symbolTableTableView->selectionModel();
  const auto&& indices   = selection->selectedIndexes();
  if (indices.empty()) { return; }
  _ui->statusbar->showMessage(tr("%1 rows selected").arg(selection->selectedRows().count()));

  const size_t  row    = static_cast<MultiFilterProxyModel*>(_ui->symbolTableTableView->model())->mapToSource(indices.first()).row();
  const Symbol& symbol = _symbolTable.at(row);

  //  if (_symbolTable[row].Display)
  //  {
  //    auto&& slices = static_cast<QPieSeries*>(_ui->chartView->chart()->series().first())->slices();
  //    auto   it     = std::find_if(slices.begin(), slices.end(), [&symbol](QPieSlice* s) { return s->label() == symbol.Name; });
  //    if (it != slices.end())
  //    {
  //      (*it)->setExploded(true);
  //    }
  //  }

  _binUtils.ExecObjdump(
    { "--start-address",
      QString::number(symbol.Address),
      "--stop-address",
      QString::number(symbol.Address + symbol.Size),
      "-CSj",
      symbol.SectionName,
      _binUtils.ElfFile() },
    [this](const QString& out) {
      _ui->disassemblyTextBrowser->setText(out);
    });
}

void MainWindow::OnShowSymbolChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  auto* const series = static_cast<QPieSeries*>(_ui->chartView->chart()->series().first());
  auto&&      slices = series->slices();
  for (int i = bottomRight.row(); i >= topLeft.row(); --i)
  {
    if (_symbolTable.at(i).Display)  // add
    {
      auto* slice = series->append(_symbolTable[i].Name, _symbolTable[i].Size);
      slice->setLabelVisible();
      slice->setColor(**_colorIter++);
    }
    else  // remove
    {
      auto it = std::find_if(slices.begin(), slices.end(), [this, i](QPieSlice* s) { return s->label() == _symbolTable[i].Name; });
      if (it != slices.end())
      {
        series->remove(*it);
      }
    }
  }
  _ui->chartView->update();
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
