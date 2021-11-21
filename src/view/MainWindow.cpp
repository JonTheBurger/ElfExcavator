#include "MainWindow.hpp"

#include <DockManager.h>
#include <QPlainTextEdit>
#include <spdlog/sinks/qt_sinks.h>
#include <spdlog/spdlog.h>

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
  }

  /// Takes ownership of widget
  void addDock(const QString& name, QWidget* widget, ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* dock_widget = new ads::CDockWidget(name);
    dock_widget->setWidget(widget);
    dock->addDockWidget(location, dock_widget);
    ui.menu_view->addAction(dock_widget->toggleViewAction());
  }

  void initLogOutputDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget          = new QPlainTextEdit();
    auto  widget_log_sink = std::make_shared<spdlog::sinks::qt_sink_mt>(widget, "appendPlainText");
    spdlog::get("")->sinks().push_back(std::move(widget_log_sink));
    addDock("Log Output", widget, location);
  }

  void initSettingsDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget = new SettingsForm(presenter.settingsPresenter(), &self);
    addDock("Settings", widget, location);
  }

  void initSectionHeaderDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget = new MultiFilterTableView(&self);
    widget->setModel(&presenter.sectionHeaderItemModel());
    addDock("Section Headers", widget, location);
  }

  void initSymbolTableDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget = new MultiFilterTableView(&self);
    widget->setModel(&presenter.symbolTableItemModel());
    addDock("Symbol Table", widget, location);
  }

  void initSectionHeaderChartDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget = new PieChartForm(presenter.selectedSectionHeaderItemModel(),
                                    SectionHeaderItemModel::NAME,
                                    SectionHeaderItemModel::SIZE,
                                    &self);
    addDock("Section Headers", widget, location);
  }

  void initSymbolTableChartDock(ads::DockWidgetArea location = ads::TopDockWidgetArea)
  {
    auto* widget = new PieChartForm(presenter.selectedSymbolTableItemModel(),
                                    SymbolTableItemModel::NAME,
                                    SymbolTableItemModel::SIZE,
                                    &self);
    addDock("Symbol Table", widget, location);
  }
};

MainWindow::MainWindow(MainPresenter& present, QWidget* parent)
    : QMainWindow(parent)
    , _self{ std::make_unique<Impl>(*this, present) }
{
  //  _self->initSettingsDock();
  //  _self->initLogOutputDock();
  _self->initSectionHeaderDock();
  _self->initSymbolTableDock();
  _self->initSectionHeaderChartDock();
  _self->initSymbolTableChartDock();
}

MainWindow::~MainWindow() = default;
