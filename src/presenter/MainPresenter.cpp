#include "MainPresenter.hpp"

#include <KCheckableProxyModel>
#include <KSelectionProxyModel>

#include "SectionHeaderItemModel.hpp"
#include "SettingsPresenter.hpp"
#include "SymbolTableItemModel.hpp"

struct MainPresenter::Impl {
  Q_DISABLE_COPY(Impl)

  MainPresenter&          self;
  SectionHeaderItemModel& section_header_item_model;
  QItemSelectionModel     section_header_check_model;      ///< Tracks selected items in the view
  KCheckableProxyModel    section_header_check_proxy;      ///< Adds checkboxes to a view
  KSelectionProxyModel    section_header_selection_proxy;  ///< Maps checked items as a selection model
  SettingsPresenter&      settings_presenter;
  SymbolTableItemModel&   symbol_table_item_model;
  QItemSelectionModel     symbol_table_check_model;      ///< Tracks selected items in the view
  KCheckableProxyModel    symbol_table_check_proxy;      ///< Adds checkboxes to a view
  KSelectionProxyModel    symbol_table_selection_proxy;  ///< Maps checked items as a selection model

  explicit Impl(MainPresenter&          that,
                SectionHeaderItemModel& section_header_present,
                SettingsPresenter&      settings_present,
                SymbolTableItemModel&   symbol_table_present) noexcept
      : self{ that }
      , section_header_item_model{ section_header_present }
      , section_header_check_model{ &section_header_item_model }
      , section_header_check_proxy{}
      , section_header_selection_proxy{}
      , settings_presenter{ settings_present }
      , symbol_table_item_model{ symbol_table_present }
      , symbol_table_check_model{ &symbol_table_item_model }
      , symbol_table_check_proxy{}
      , symbol_table_selection_proxy{}
  {
    section_header_check_proxy.setSourceModel(&section_header_item_model);
    section_header_check_proxy.setSelectionModel(&section_header_check_model);
    section_header_selection_proxy.setSelectionModel(&section_header_check_model);
    section_header_selection_proxy.setSourceModel(&section_header_item_model);
    section_header_selection_proxy.setFilterBehavior(KSelectionProxyModel::ExactSelection);

    symbol_table_check_proxy.setSourceModel(&symbol_table_item_model);
    symbol_table_check_proxy.setSelectionModel(&symbol_table_check_model);
    symbol_table_selection_proxy.setSelectionModel(&symbol_table_check_model);
    symbol_table_selection_proxy.setSourceModel(&symbol_table_item_model);
    symbol_table_selection_proxy.setFilterBehavior(KSelectionProxyModel::ExactSelection);

    connect(&settings_presenter, &SettingsPresenter::elfFileChanged, [this](const QString& elf_file, bool is_valid) {
      static_cast<void>(elf_file);
      if (is_valid)
      {
        section_header_item_model.onElfFileLoaded();
        symbol_table_item_model.onElfFileLoaded();
      }
    });
  }
};

MainPresenter::MainPresenter(SectionHeaderItemModel& section_header_item_model,
                             SettingsPresenter&      settings_presenter,
                             SymbolTableItemModel&   symbol_table_item_model,
                             QObject*                parent)
    : QObject(parent)
    , _self{ std::make_unique<Impl>(*this,
                                    section_header_item_model,
                                    settings_presenter,
                                    symbol_table_item_model) }
{
}

MainPresenter::~MainPresenter() = default;

QAbstractItemModel& MainPresenter::sectionHeaderItemModel() noexcept
{
  return _self->section_header_check_proxy;
}

QAbstractItemModel& MainPresenter::selectedSectionHeaderItemModel() noexcept
{
  return _self->section_header_selection_proxy;
}

SettingsPresenter& MainPresenter::settingsPresenter() noexcept
{
  return _self->settings_presenter;
}

QAbstractItemModel& MainPresenter::symbolTableItemModel() noexcept
{
  return _self->symbol_table_check_proxy;
}

QAbstractItemModel& MainPresenter::selectedSymbolTableItemModel() noexcept
{
  return _self->symbol_table_selection_proxy;
}
