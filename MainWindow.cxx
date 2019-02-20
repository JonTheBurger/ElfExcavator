#include "MainWindow.hxx"

#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QtCharts>
#include <algorithm>

#include "BinUtils.hxx"
#include "SectionHeader.hxx"
#include "SectionHeaderItemModel.hxx"
#include "Symbol.hxx"
#include "SymbolItemModel.hxx"
#include "ui_MainWindow.h"

// https://doc.qt.io/qt-5.9/classes.html
/// TODO: Chart colors
/// TODO: Display Hex QStyledItemDelegate https://doc.qt.io/qt-5/qtwidgets-itemviews-stardelegate-example.html
/// TODO: Zoom Chart
/// TODO: View Menu
/// TODO: Plug the leaks
/// BIG TODO: Filter https://doc.qt.io/qt-5/qsortfilterproxymodel.html#filterAcceptsRow
/// BIG TODO: Syntax Highlighting
/// BIG TODO: Options Menu (25 count, chart theme, chart animations)
/// BIG TODO: Installer
class MainWindow::MainWindowPrivate {
  Q_DISABLE_COPY(MainWindowPrivate)

public:
  explicit MainWindowPrivate(BinUtils* binUtils)
      : Ui{ std::make_unique<Ui::MainWindow>() }
      , BinUtil{ *binUtils }
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
          slice->setLabelVisible();
        }
      }

      Ui->chartView->chart()->setTitle(tr("Section Headers"));
    }
    else if (tab == MainWindow::Tab::SYMBOLS)
    {
      OnSelectedSymbolChanged();

      for (auto& symbol : SymbolTable)
      {
        if (symbol.Display)
        {
          QPieSlice* const slice = series->append(symbol.Name, symbol.Size);
          slice->setLabelVisible();
        }
      }

      Ui->chartView->chart()->setTitle(tr("Symbols"));
    }

    Ui->chartView->chart()->removeAllSeries();
    Ui->chartView->chart()->addSeries(series);
  }

  void OnSelectedSectionHeaderChanged()
  {
    const auto&& indices = Ui->sectionHeaderTableView->selectionModel()->selectedIndexes();
    if (indices.empty()) { return; }

    const size_t   row         = SectionModel->mapToSource(indices.first()).row();
    const QString& sectionName = SectionHeaders.at(row).Name;

    if (SectionHeaders[row].Display)
    {
      SectionModel->mapToSource(indices.first()).data(Qt::CheckStateRole);
      auto&& slices = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first())->slices();
      auto   it     = std::find_if(slices.begin(), slices.end(), [&sectionName](QPieSlice* s) { return s->label() == sectionName; });
      if (it != slices.end())
      {
        (*it)->setExploded(true);
      }
    }

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
    const auto* thing = Ui->symbolTableTableView->selectionModel();
    Q_ASSERT(thing);
    const auto&& indices = thing->selectedIndexes();
    if (indices.empty()) { return; }

    const size_t  row    = SymbolModel->mapToSource(indices.first()).row();
    const Symbol& symbol = SymbolTable.at(row);

    if (SymbolTable[row].Display)
    {
      auto&& slices = static_cast<QPieSeries*>(Ui->chartView->chart()->series().first())->slices();
      auto   it     = std::find_if(slices.begin(), slices.end(), [&symbol](QPieSlice* s) { return s->label() == symbol.Name; });
      if (it != slices.end())
      {
        (*it)->setExploded(true);
      }
    }

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

  std::unique_ptr<Ui::MainWindow>        Ui;
  BinUtils&                              BinUtil;
  std::unique_ptr<QSortFilterProxyModel> SectionModel;
  std::unique_ptr<QSortFilterProxyModel> SymbolModel;
  std::vector<SectionHeader>             SectionHeaders;
  std::vector<Symbol>                    SymbolTable;
  bool                                   IsInit{ false };
};

MainWindow::MainWindow(BinUtils* binUtils, QWidget* parent)
    : QMainWindow{ parent }
    , _impl{ std::make_unique<MainWindowPrivate>(binUtils) }
{
  _impl->Ui->setupUi(this);
  _impl->SetupChart();
  QSettings settings;
  resize(settings.value("MainWindow::size", size()).toSize());
  move(settings.value("MainWindow::pos", pos()).toPoint());

  connect(_impl->Ui->tabWidget, &QTabWidget::currentChanged, [this](int tab) {
    _impl->OnTabChanged(static_cast<Tab>(tab));
  });
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
    _impl->BinUtil.ExecObjdump(
      { "-hw", _impl->BinUtil.ElfFile() },
      [this](const QString& out) {
        _impl->SectionHeaders    = SectionHeader::ParseFromObjdump(out);
        auto* sectionHeaderModel = new SectionHeaderItemModel(_impl->SectionHeaders, this);
        _impl->SectionModel      = std::make_unique<QSortFilterProxyModel>();
        _impl->SectionModel->setSourceModel(sectionHeaderModel);
        _impl->Ui->sectionHeaderTableView->setModel(_impl->SectionModel.get());
        _impl->Ui->sectionHeaderTableView->setSortingEnabled(true);
        _impl->Ui->sectionHeaderTableView->sortByColumn(SectionHeaderItemModel::INDEX, Qt::AscendingOrder);

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
        _impl->SymbolModel     = std::make_unique<QSortFilterProxyModel>();
        _impl->SymbolModel->setSourceModel(symbolTableModel);
        _impl->Ui->symbolTableTableView->setModel(_impl->SymbolModel.get());
        _impl->Ui->symbolTableTableView->setSortingEnabled(true);
        _impl->Ui->symbolTableTableView->sortByColumn(SymbolItemModel::INDEX, Qt::AscendingOrder);

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
