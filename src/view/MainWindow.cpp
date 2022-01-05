#include "MainWindow.hpp"

#include <DockManager.h>
#include <QPlainTextEdit>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/spdlog.h>

#include "HexView.hpp"
#include "MultiFilterTableView.hpp"
#include "PieChartForm.hpp"
#include "SettingsForm.hpp"
#include "presenter/MainPresenter.hpp"
#include "presenter/SectionHeaderItemModel.hpp"
#include "presenter/SymbolTableItemModel.hpp"
#include "ui_MainWindow.h"

struct MainWindow::Impl {
  Q_DISABLE_COPY(Impl)

  MainWindow&        self;
  Ui::MainWindow     ui;
  MainPresenter&     presenter;
  ads::CDockManager* dock;

  explicit Impl(MainWindow& that, MainPresenter& present) noexcept
      : self{ that }
      , ui{}
      , presenter{ present }
      , dock{}
  {
    ui.setupUi(&self);
    // Dock constructor must come AFTER setupUi()
    dock = new ads::CDockManager(&self);
    dock->setConfigFlag(ads::CDockManager::FloatingContainerForceNativeTitleBar, true);
  }

  /// Takes ownership of widget
  void addDock(const QString& name, QWidget* widget, ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* dock_widget = new ads::CDockWidget(name);
    dock_widget->setWidget(widget);
    dock->addDockWidget(location, dock_widget);
    ui.menu_view->addAction(dock_widget->toggleViewAction());
  }

  /// Takes ownership of widget
  void addDockTab(const QString& name, QWidget* widget, ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* dock_widget = new ads::CDockWidget(name);
    dock_widget->setWidget(widget);
    dock->addDockWidgetTab(location, dock_widget);
    ui.menu_view->addAction(dock_widget->toggleViewAction());
  }

  void initSettingsDock()
  {
    auto* widget = new SettingsForm(presenter.settingsPresenter(), &self);
    addDock("Settings", widget, ads::BottomDockWidgetArea);
  }

  void initLogOutputDock()
  {
    auto* widget          = new QPlainTextEdit();
    auto  widget_log_sink = std::make_shared<spdlog::sinks::qt_sink_mt>(widget, "appendPlainText");
    spdlog::get("")->sinks().push_back(std::move(widget_log_sink));
    addDockTab("Log Output", widget, ads::BottomDockWidgetArea);
  }

  MultiFilterTableView& initSectionHeaderDock()
  {
    auto* widget = new MultiFilterTableView(&self);
    // Using QAbstractItemView::SelectionBehavior::SelectRows forces the row of QModelIndex in the
    // QAbstractItemView::selectionChanged signal to be indexable by SectionHeaderItemModel::Columns.
    widget->setModel(&presenter.sectionHeaderDisplayModel());
    widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    addDock("Section Headers", widget, ads::CenterDockWidgetArea);
    return *widget;
  }

  MultiFilterTableView& initSymbolTableDock()
  {
    auto* widget = new MultiFilterTableView(&self);
    widget->setModel(&presenter.symbolTableDisplayModel());
    widget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    addDockTab("Symbol Table", widget, ads::CenterDockWidgetArea);
    return *widget;
  }

  void initSectionHeaderChartDock(MultiFilterTableView& table)
  {
    auto* widget = new PieChartForm(presenter.sectionHeaderCheckedModel(),
                                    SectionHeaderItemModel::NAME,
                                    SectionHeaderItemModel::SIZE,
                                    &self);
    connect(table.selectionModel(), &QItemSelectionModel::selectionChanged, [widget](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section_name = selected.indexes()[SectionHeaderItemModel::NAME].data().toString();
        widget->explodeSlice(section_name);
      }
    });
    connect(widget, &PieChartForm::sliceSelected, [this, &table](const QString& label) {
      auto idx = presenter.sectionHeaderItemModel().indexOfSection(label);
      table.selectRow(idx.row());
      table.scrollTo(idx);
    });
    addDock("Section Headers", widget, ads::TopDockWidgetArea);
  }

  void initSymbolTableChartDock(MultiFilterTableView& table)
  {
    auto* widget = new PieChartForm(presenter.symbolTableCheckedModel(),
                                    SymbolTableItemModel::DEMANGLED_NAME,
                                    SymbolTableItemModel::SIZE,
                                    &self);
    connect(table.selectionModel(), &QItemSelectionModel::selectionChanged, [widget](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section_name = selected.indexes()[SymbolTableItemModel::DEMANGLED_NAME].data().toString();
        widget->explodeSlice(section_name);
      }
    });
    connect(widget, &PieChartForm::sliceSelected, [this, &table](const QString& label) {
      auto idx = presenter.symbolTableItemModel().indexOfSymbol(label);
      table.selectRow(idx.row());
      table.scrollTo(idx);
    });
    addDockTab("Symbol Table", widget, ads::TopDockWidgetArea);
  }

  void initSectionHeaderHexDock(QItemSelectionModel* selection_model)
  {
    auto* widget = new HexView(&self);
    addDock("Section Contents", widget, ads::RightDockWidgetArea);
    connect(selection_model, &QItemSelectionModel::selectionChanged, [this, widget](const QItemSelection& selected, const QItemSelection& deselected) {
      if (!selected.indexes().empty())
      {
        auto contents = selected.indexes()[0].data(SectionHeaderItemModel::Role::SECTION_CONTENTS).toByteArray();
        widget->setSource(contents);
      }
    });
  }

  void initSymbolTableHexDock()
  {
    auto* widget = new HexView(&self);
    addDockTab("Disassembly", widget, ads::RightDockWidgetArea);
  }
};

MainWindow::MainWindow(MainPresenter& present, QWidget* parent)
    : QMainWindow(parent)
    , _self{ std::make_unique<Impl>(*this, present) }
{
  auto&& section_header_table = _self->initSectionHeaderDock();
  auto&& symbol_table         = _self->initSymbolTableDock();

  _self->initSettingsDock();
  _self->initLogOutputDock();

  _self->initSectionHeaderHexDock(section_header_table.selectionModel());
  _self->initSymbolTableHexDock();

  _self->initSectionHeaderChartDock(section_header_table);
  _self->initSymbolTableChartDock(symbol_table);
}

MainWindow::~MainWindow() = default;
