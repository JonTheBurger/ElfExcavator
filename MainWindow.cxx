#include "MainWindow.hxx"

#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QtCharts>
#include <algorithm>

#include "BinUtils.hxx"
#include "DisassemblyHighlighter.hxx"
#include "FilterHeaderView.hxx"
#include "HexNumberDelegate.hxx"
#include "MultiFilterProxyModel.hxx"
#include "SectionHeader.hxx"
#include "SectionHeaderItemModel.hxx"
#include "Symbol.hxx"
#include "SymbolItemModel.hxx"
#include "ui_MainWindow.h"

// https://doc.qt.io/qt-5.9/classes.html
/// TODO: Space datagrid normally
/// TODO: Ctrl+Shift+F Find in disassembly view (derived class)
/// TODO: Chart colors
/// TODO: Forward/back button support (goto last symbol)
/// TODO: View Menu (show dock widget)
/// TODO: Plug the leaks
/// TODO: Evaluate memory/perf of DisassemblyHighlighter
/// BIG TODO: Filter by hex number
/// BIG TODO: Click on pieslice to highlight symbol?
/// BIG TODO: Options Menu (25 count, chart theme, chart animations, regex options)
/// BIG TODO: Installer
/// BIG TODO: All others chart item
/// BIG TODO: Uncheck all
/// BIG TODO: Background thread
/// BIG TODO: Split assembly and source?
/// BIG TODO: Click on function to navigate
class MainWindow::MainWindowPrivate {
  Q_DISABLE_COPY(MainWindowPrivate)

public:
  explicit MainWindowPrivate(BinUtils* binUtils)
      : Ui{ std::make_unique<Ui::MainWindow>() }
      , BinUtil{ *binUtils }
      , HexDelegate{ std::make_unique<HexNumberDelegate>() }
      , Highlighter{ std::make_unique<DisassemblyHighlighter>() }
  {
    Q_ASSERT(binUtils);
  }

  ~MainWindowPrivate() = default;

  void SetupChart()
  {
    auto* chart = new QChart();  /// Owned by chartView when setChart is called
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartTheme::ChartThemeDark);
    Ui->chartView->setChart(chart);
    Ui->chartView->setRenderHint(QPainter::Antialiasing);
  }

  void OnTabChanged(const MainWindow::Tab tab)
  {
    auto* series = new QPieSeries();  /// Owned by QChart when addSeries is called
    series->setUseOpenGL(true);

    if (tab == MainWindow::Tab::SECTIONS)
    {
      OnSelectedSectionHeaderChanged();

      for (auto& section : SectionHeaders)
      {
        if (section.Display)
        {
          QPieSlice* const slice = series->append(section.Name, section.Size);
        }
      }

      Ui->chartView->chart()->setTitle(tr("Section Headers"));
      Highlighter->Enabled = false;
      connect(series, &QPieSeries::clicked, [this](QPieSlice* slice) {
        auto it = std::find_if(SectionHeaders.begin(), SectionHeaders.end(), [&slice](auto&& header) { return header.Display && (header.Name == slice->label()); });
        if (it != SectionHeaders.end())
        {
          auto index = SectionModel->sourceModel()->index(it->Index, 0);
          index      = SectionModel->mapFromSource(index);
          Ui->sectionHeaderTableView->setCurrentIndex(index);
        }
      });
    }
    else if (tab == MainWindow::Tab::SYMBOLS)
    {
      OnSelectedSymbolChanged();

      for (auto& symbol : SymbolTable)
      {
        if (symbol.Display)
        {
          QPieSlice* const slice = series->append(symbol.Name, symbol.Size);
        }
      }

      Ui->chartView->chart()->setTitle(tr("Symbols"));
      Highlighter->Enabled = true;
      connect(series, &QPieSeries::clicked, [this](QPieSlice* slice) {
        auto it = std::find_if(SymbolTable.begin(), SymbolTable.end(), [&slice](auto&& header) { return header.Display && (header.Name == slice->label()); });
        if (it != SymbolTable.end())
        {
          auto index = SymbolModel->sourceModel()->index(it->Index, 0);
          index      = SymbolModel->mapFromSource(index);
          Ui->symbolTableTableView->setCurrentIndex(index);
        }
      });
    }

    Ui->chartView->chart()->removeAllSeries();
    Ui->chartView->chart()->addSeries(series);  // WARNING: Memory leak?
    connect(series, &QPieSeries::hovered, [](QPieSlice* slice, bool state) {
      //      slice->setExploded(state);
      slice->setLabelVisible(state);
    });
  }

  void OnSelectedSectionHeaderChanged()
  {
    const auto*  selection = Ui->sectionHeaderTableView->selectionModel();
    const auto&& indices   = selection->selectedIndexes();
    if (indices.empty()) { return; }
    Ui->statusbar->showMessage(tr("%1 rows selected").arg(selection->selectedRows().count()));

    const size_t   row         = SectionModel->mapToSource(indices.first()).row();
    const QString& sectionName = SectionHeaders.at(row).Name;

    //    if (SectionHeaders[row].Display)
    //    {
    //            SectionModel->mapToSource(indices.first()).data(Qt::CheckStateRole);
    //            auto&& slices = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first())->slices();
    //            auto   it     = std::find_if(slices.begin(), slices.end(), [&sectionName](QPieSlice* s) { return s->label() == sectionName; });
    //            if (it != slices.end())
    //            {
    //              (*it)->setExploded(true);
    //            }
    //    }

    BinUtil.ExecObjdump(
      { "-sj", sectionName, BinUtil.ElfFile() },
      [this](const QString& out) {
        Ui->disassemblyTextBrowser->setText(out);
      });
  }

  void OnShowHeaderChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
  {
    (void)roles;
    auto* const series = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first());
    auto&&      slices = series->slices();
    for (int i = bottomRight.row(); i >= topLeft.row(); --i)
    {
      if (SectionHeaders.at(i).Display)  // add
      {
        auto* slice = series->append(SectionHeaders[i].Name, SectionHeaders[i].Size);
        slice->setLabelVisible();
      }
      else  // remove
      {
        auto it = std::find_if(slices.begin(), slices.end(), [this, i](QPieSlice* s) { return s->label() == SectionHeaders[i].Name; });
        if (it != slices.end())
        {
          series->remove(*it);
        }
      }
    }
    Ui->chartView->update();
  }

  void OnSelectedSymbolChanged()
  {
    const auto*  selection = Ui->symbolTableTableView->selectionModel();
    const auto&& indices   = selection->selectedIndexes();
    if (indices.empty()) { return; }
    Ui->statusbar->showMessage(tr("%1 rows selected").arg(selection->selectedRows().count()));

    const size_t  row    = SymbolModel->mapToSource(indices.first()).row();
    const Symbol& symbol = SymbolTable.at(row);

    //    if (SymbolTable[row].Display)
    //    {
    //      auto&& slices = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first())->slices();
    //      auto   it     = std::find_if(slices.begin(), slices.end(), [&symbol](QPieSlice* s) { return s->label() == symbol.Name; });
    //      if (it != slices.end())
    //      {
    //        (*it)->setExploded(true);
    //      }
    //    }

    BinUtil.ExecObjdump(
      { "--start-address",
        QString::number(symbol.Address),
        "--stop-address",
        QString::number(symbol.Address + symbol.Size),
        "-CSj",
        symbol.SectionName,
        BinUtil.ElfFile() },
      [this](const QString& out) {
        Ui->disassemblyTextBrowser->setText(out);
      });
  }

  void OnShowSymbolChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
  {
    auto* const series = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first());
    auto&&      slices = series->slices();
    for (int i = bottomRight.row(); i >= topLeft.row(); --i)
    {
      if (SymbolTable.at(i).Display)  // add
      {
        auto* slice = series->append(SymbolTable[i].Name, SymbolTable[i].Size);
        slice->setLabelVisible();
      }
      else  // remove
      {
        auto it = std::find_if(slices.begin(), slices.end(), [this, i](QPieSlice* s) { return s->label() == SymbolTable[i].Name; });
        if (it != slices.end())
        {
          series->remove(*it);
        }
      }
    }
    Ui->chartView->update();
  }

  std::unique_ptr<Ui::MainWindow>         Ui;
  BinUtils&                               BinUtil;
  std::unique_ptr<MultiFilterProxyModel>  SectionModel;
  std::unique_ptr<MultiFilterProxyModel>  SymbolModel;
  std::vector<SectionHeader>              SectionHeaders;
  std::vector<Symbol>                     SymbolTable;
  std::unique_ptr<HexNumberDelegate>      HexDelegate;
  std::unique_ptr<DisassemblyHighlighter> Highlighter;
  bool                                    IsInit{ false };
};

MainWindow::MainWindow(BinUtils* binUtils, QWidget* parent)
    : QMainWindow{ parent }
    , _impl{ std::make_unique<MainWindowPrivate>(binUtils) }
{
  _impl->Ui->setupUi(this);
  _impl->SetupChart();
  _impl->Highlighter->setDocument(_impl->Ui->disassemblyTextBrowser->document());
  QSettings settings;
  resize(settings.value("MainWindow::size", size()).toSize());
  move(settings.value("MainWindow::pos", pos()).toPoint());

  connect(_impl->Ui->tabWidget, &QTabWidget::currentChanged, [this](int tab) {
    _impl->OnTabChanged(static_cast<Tab>(tab));
  });
  connect(_impl->Ui->zoomInButton, &QPushButton::pressed, _impl->Ui->chartView, &PieChart::zoomIn);
  connect(_impl->Ui->zoomOutButton, &QPushButton::pressed, _impl->Ui->chartView, &PieChart::zoomOut);
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue("MainWindow::size", size());
  settings.setValue("MainWindow::pos", pos());
}

void MainWindow::showEvent(QShowEvent* e)
{
  QMainWindow::showEvent(e);
  if (!_impl->IsInit)
  {
    connect(_impl->Ui->disassemblySearchLineEdit, &QLineEdit::returnPressed, [this]() {
      QString query = _impl->Ui->disassemblySearchLineEdit->text();
      if (!query.isEmpty())
      {
        const bool found = _impl->Ui->disassemblyTextBrowser->find(query);
        if (!found)
        {
          _impl->Ui->disassemblyTextBrowser->moveCursor(QTextCursor::Start);
          _impl->Ui->disassemblyTextBrowser->find(query);
        }
      }
    });
    connect(_impl->Ui->disassemblySearchLineEdit, &QLineEdit::textChanged, [this](const QString& query) {
      if (!query.isEmpty())
      {
        QList<QTextEdit::ExtraSelection> extraSelections;

        _impl->Ui->disassemblyTextBrowser->moveCursor(QTextCursor::Start);
        QColor color = QColor(Qt::gray).lighter(130);

        while (_impl->Ui->disassemblyTextBrowser->find(query))
        {
          QTextEdit::ExtraSelection extra;
          extra.format.setBackground(color);
          extra.cursor = _impl->Ui->disassemblyTextBrowser->textCursor();
          extraSelections.append(extra);
        }

        _impl->Ui->disassemblyTextBrowser->setExtraSelections(extraSelections);
      }
    });

    _impl->BinUtil.ExecObjdump(
      { "-hw", _impl->BinUtil.ElfFile() },
      [this](const QString& out) {
        _impl->SectionHeaders    = SectionHeader::ParseFromObjdump(out);
        auto* sectionHeaderModel = new SectionHeaderItemModel(_impl->SectionHeaders, this);
        _impl->SectionModel      = std::make_unique<MultiFilterProxyModel>();
        _impl->SectionModel->setSourceModel(sectionHeaderModel);
        _impl->Ui->sectionHeaderTableView->setModel(_impl->SectionModel.get());
        _impl->Ui->sectionHeaderTableView->setItemDelegate(_impl->HexDelegate.get());
        auto* header = new FilterHeaderView(_impl->Ui->sectionHeaderTableView);
        _impl->Ui->sectionHeaderTableView->setHorizontalHeader(header);
        header->setVisible(true);
        _impl->Ui->sectionHeaderTableView->setSortingEnabled(true);
        _impl->Ui->sectionHeaderTableView->sortByColumn(SectionHeaderItemModel::INDEX, Qt::AscendingOrder);
        QObject::connect(header, &FilterHeaderView::filterTextChanged, _impl->SectionModel.get(), &MultiFilterProxyModel::setColumnFilter);

        connect(
          sectionHeaderModel,
          &QAbstractItemModel::dataChanged,
          [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
            _impl->OnShowHeaderChanged(topLeft, bottomRight, roles);
          });

        connect(
          _impl->Ui->sectionHeaderTableView->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          [this]() {
            this->_impl->OnSelectedSectionHeaderChanged();
          });

        _impl->OnTabChanged(static_cast<Tab>(_impl->Ui->tabWidget->currentIndex()));
      });

    _impl->BinUtil.ExecObjdump(
      { "-Ctw", _impl->BinUtil.ElfFile() },
      [this](const QString& out) {
        _impl->SymbolTable     = Symbol::ParseFromObjdump(out);
        auto* symbolTableModel = new SymbolItemModel(_impl->SymbolTable, this);
        _impl->SymbolModel     = std::make_unique<MultiFilterProxyModel>();
        _impl->SymbolModel->setSourceModel(symbolTableModel);
        _impl->Ui->symbolTableTableView->setModel(_impl->SymbolModel.get());
        _impl->Ui->symbolTableTableView->setItemDelegate(_impl->HexDelegate.get());
        auto* header = new FilterHeaderView(_impl->Ui->symbolTableTableView);
        _impl->Ui->symbolTableTableView->setHorizontalHeader(header);
        header->setVisible(true);
        _impl->Ui->symbolTableTableView->setSortingEnabled(true);
        _impl->Ui->symbolTableTableView->sortByColumn(SymbolItemModel::INDEX, Qt::AscendingOrder);
        QObject::connect(header, &FilterHeaderView::filterTextChanged, _impl->SymbolModel.get(), &MultiFilterProxyModel::setColumnFilter);

        connect(
          symbolTableModel,
          &QAbstractItemModel::dataChanged,
          [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
            _impl->OnShowSymbolChanged(topLeft, bottomRight, roles);
          });

        connect(
          _impl->Ui->symbolTableTableView->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          [this]() {
            this->_impl->OnSelectedSymbolChanged();
          });
      });

    _impl->IsInit = true;
  }
}

void MainWindow::changeEvent(QEvent* e)
{
  QMainWindow::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      _impl->Ui->retranslateUi(this);
      break;
    default:
      break;
  }
}
