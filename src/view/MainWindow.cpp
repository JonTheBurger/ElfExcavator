#include "MainWindow.hpp"

#include <DockManager.h>
#include <QProcess>
#include <QTextBrowser>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/spdlog.h>

#include "CxxDisassemblyHighlighter.hpp"
#include "HexView.hpp"
#include "MultiFilterTableView.hpp"
#include "PieChartForm.hpp"
#include "SettingsForm.hpp"
#include "presenter/MainPresenter.hpp"
#include "presenter/SectionHeaderItemModel.hpp"
#include "presenter/SettingsPresenter.hpp"
#include "presenter/SymbolTableItemModel.hpp"
#include "ui_MainWindow.h"

static void disassembleToAsync(const QString& objdump, const QString& executable, QTextBrowser* widget, const QString& section, quint64 address, quint64 size)
{
  auto* process = new QProcess;
  QObject::connect(process,
                   static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                   [widget, process](int exitCode, QProcess::ExitStatus exitStatus) {
                     if (exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit)
                     {
                       widget->setText(process->readAllStandardOutput());
                     }
                     else
                     {
                       widget->clear();
                     }
                     process->deleteLater();
                   });
  process->start(objdump,
                 { "--start-address",
                   QString::number(address),
                   "--stop-address",
                   QString::number(address + size),
                   "-CSj",  // Demangle, Display Source, Only for section
                   section,
                   executable });
}

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
    connect(selection_model, &QItemSelectionModel::selectionChanged, [this, widget](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto contents = selected.indexes()[0].data(SectionHeaderItemModel::Role::SECTION_CONTENTS).toByteArray();
        widget->setSource(contents);
      }
    });
  }

  void initSymbolTableDisassemblyDock(QItemSelectionModel* selection_model)
  {
    auto* widget = new QTextBrowser(&self);
    new CxxDisassemblyHighlighter(widget->document());
    addDockTab("Disassembly", widget, ads::RightDockWidgetArea);
    connect(selection_model, &QItemSelectionModel::selectionChanged, [this, widget](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section = selected.indexes()[SymbolTableItemModel::Columns::SECTION].data().toString();
        auto address = selected.indexes()[SymbolTableItemModel::Columns::VALUE].data().toULongLong();
        auto size    = selected.indexes()[SymbolTableItemModel::Columns::SIZE].data().toULongLong();
        disassembleToAsync(
          presenter.settingsPresenter().objdumpPath(),
          presenter.settingsPresenter().elfFile(),
          widget,
          section,
          address,
          size);
      }
    });
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
  _self->initSymbolTableDisassemblyDock(symbol_table.selectionModel());

  _self->initSectionHeaderChartDock(section_header_table);
  _self->initSymbolTableChartDock(symbol_table);
}

MainWindow::~MainWindow() = default;
