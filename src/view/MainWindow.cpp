#include "MainWindow.hpp"

#include <DockManager.h>
#include <QProcess>
#include <QTextBrowser>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/spdlog.h>

#include "CxxDisassemblyHighlighter.hpp"
#include "HexView.hpp"
#include "LogHighlighter.hpp"
#include "MultiFilterTableView.hpp"
#include "PieChartForm.hpp"
#include "SettingsForm.hpp"
#include "presenter/MainPresenter.hpp"
#include "presenter/MultiFilterProxyModel.hpp"
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
                 { QStringLiteral("--start-address"),
                   QString::number(address),
                   QStringLiteral("--stop-address"),
                   QString::number(address + size),
                   QStringLiteral("-CSj"),  // Demangle, Display Source, Only for section
                   section,
                   executable });
}

struct MainWindow::Impl {
  Q_DISABLE_COPY(Impl)

  MainWindow&           self;
  Ui::MainWindow        ui;
  MainPresenter&        presenter;
  ads::CDockManager*    dock;
  SettingsForm*         settings_form;
  QTextBrowser*         log_output_form;
  MultiFilterTableView* section_headers_form;
  MultiFilterTableView* symbol_table_form;
  PieChartForm*         section_headers_chart_form;
  PieChartForm*         symbol_table_chart_form;
  HexView*              section_contents_form;
  QTextBrowser*         disassembly_form;

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
  ads::CDockWidget* addDock(const QString& name, QWidget* widget, ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* dock_widget = new ads::CDockWidget(name);
    dock_widget->setWidget(widget);
    dock->addDockWidget(location, dock_widget);
    ui.menu_view->addAction(dock_widget->toggleViewAction());
    return dock_widget;
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
    settings_form = new SettingsForm(presenter.settingsPresenter(), &self);
  }

  void initLogOutputDock()
  {
    log_output_form = new QTextBrowser();
    log_output_form->setFontFamily(QStringLiteral("Monospace"));
    new LogHighlighter(log_output_form->document());
    auto widget_log_sink = std::make_shared<spdlog::sinks::qt_sink_mt>(log_output_form, "append");
    spdlog::get("")->sinks().push_back(std::move(widget_log_sink));
  }

  MultiFilterTableView& initSectionHeaderDock()
  {
    section_headers_form = new MultiFilterTableView(&self);
    // Using QAbstractItemView::SelectionBehavior::SelectRows forces the row of QModelIndex in the
    // QAbstractItemView::selectionChanged signal to be indexable by SectionHeaderItemModel::Columns.
    section_headers_form->setModel(&presenter.sectionHeaderDisplayModel());
    section_headers_form->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    return *section_headers_form;
  }

  MultiFilterTableView& initSymbolTableDock()
  {
    symbol_table_form = new MultiFilterTableView(&self);
    symbol_table_form->setModel(&presenter.symbolTableDisplayModel());
    symbol_table_form->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    return *symbol_table_form;
  }

  void initSectionHeaderChartDock(MultiFilterTableView& table)
  {
    section_headers_chart_form = new PieChartForm(presenter.sectionHeaderCheckedModel(),
                                                  SectionHeaderItemModel::NAME,
                                                  SectionHeaderItemModel::SIZE,
                                                  &self);
    connect(table.selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section_name = selected.indexes()[SectionHeaderItemModel::NAME].data().toString();
        section_headers_chart_form->explodeSlice(section_name);
      }
    });
    connect(section_headers_chart_form, &PieChartForm::sliceSelected, [this, &table](const QString& label) {
      // FIXME: This index juggling should be done through main presenter
      auto idx = presenter.sectionHeaderItemModel().indexOfSection(label);
      idx      = presenter.sectionHeaderDisplayModel().mapFromSource(idx);
      idx      = table.model()->mapFromSource(idx);
      table.selectRow(idx.row());
      table.scrollTo(idx);
    });
  }

  void initSymbolTableChartDock(MultiFilterTableView& table)
  {
    symbol_table_chart_form = new PieChartForm(presenter.symbolTableCheckedModel(),
                                               SymbolTableItemModel::DEMANGLED_NAME,
                                               SymbolTableItemModel::SIZE,
                                               &self);
    connect(table.selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section_name = selected.indexes()[SymbolTableItemModel::DEMANGLED_NAME].data().toString();
        symbol_table_chart_form->explodeSlice(section_name);
      }
    });
    connect(symbol_table_chart_form, &PieChartForm::sliceSelected, [this, &table](const QString& label) {
      // FIXME: This index juggling should be done through main presenter
      auto idx = presenter.symbolTableItemModel().indexOfSymbol(label);
      idx      = presenter.symbolTableDisplayModel().mapFromSource(idx);
      idx      = table.model()->mapFromSource(idx);
      table.selectRow(idx.row());
      table.scrollTo(idx);
    });
  }

  void initSectionHeaderHexDock(QItemSelectionModel* selection_model)
  {
    section_contents_form = new HexView(&self);
    connect(selection_model, &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto contents = selected.indexes()[0].data(SectionHeaderItemModel::Role::SECTION_CONTENTS).toByteArray();
        section_contents_form->setSource(contents);
      }
    });
  }

  void initSymbolTableDisassemblyDock(QItemSelectionModel* selection_model)
  {
    disassembly_form = new QTextBrowser(&self);
    disassembly_form->setFontFamily(QStringLiteral("Monospace"));
    new CxxDisassemblyHighlighter(disassembly_form->document());
    connect(selection_model, &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection&) {
      if (!selected.indexes().empty())
      {
        auto section = selected.indexes()[SymbolTableItemModel::Columns::SECTION].data().toString();
        auto address = selected.indexes()[SymbolTableItemModel::Columns::VALUE].data().toULongLong();
        auto size    = selected.indexes()[SymbolTableItemModel::Columns::SIZE].data().toULongLong();
        disassembleToAsync(
          presenter.settingsPresenter().objdumpPath(),
          presenter.settingsPresenter().elfFile(),
          disassembly_form,
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

  auto* top_dock = _self->addDock(tr("Section Headers"), _self->section_headers_chart_form, ads::TopDockWidgetArea);
  _self->addDockTab(tr("Symbol Table"), _self->symbol_table_chart_form, ads::TopDockWidgetArea);
  top_dock->raise();

  auto* right_dock = _self->addDock(tr("Section Contents"), _self->section_contents_form, ads::RightDockWidgetArea);
  _self->addDockTab(tr("Disassembly"), _self->disassembly_form, ads::RightDockWidgetArea);
  right_dock->raise();

  auto* bottom_dock = _self->addDock(tr("Section Headers"), _self->section_headers_form, ads::BottomDockWidgetArea);
  _self->addDockTab(tr("Symbol Table"), _self->symbol_table_form, ads::BottomDockWidgetArea);
  _self->addDockTab(tr("Settings"), _self->settings_form, ads::BottomDockWidgetArea);
  _self->addDockTab(tr("Log Output"), _self->log_output_form, ads::BottomDockWidgetArea);
  bottom_dock->raise();
}

MainWindow::~MainWindow() = default;
