#include "MainPresenter.hpp"

#include <KCheckableProxyModel>
#include <KSelectionProxyModel>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <vector>

#include "SectionHeaderItemModel.hpp"
#include "SettingsPresenter.hpp"
#include "SymbolTableItemModel.hpp"
#include "model/ElfFile.hpp"

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
        checkBoxesOfNLargest(section_header_check_proxy, SectionHeaderItemModel::SIZE, 10, filterNonAllocSections);
        checkBoxesOfNLargest(symbol_table_check_proxy, SymbolTableItemModel::SIZE, 10);
      }
    });
  }

  using FilterFunction = bool (&)(QAbstractItemModel&, int);

  static bool filterNonAllocSections(QAbstractItemModel& model, int row)
  {
    return !model.data(model.index(row, SectionHeaderItemModel::FLAGS)).toString().contains("ALLOC");
  }

  static bool filterNone(QAbstractItemModel&, int)
  {
    return false;
  }

  static void checkBoxesOfNLargest(
    QAbstractItemModel& model,
    int                 col,
    size_t              n_largest,
    FilterFunction      filter = filterNone)
  {
    const auto       rows = model.rowCount();
    std::vector<int> largest_rows;

    const auto cmp = [&model, col](int lhs_row, int rhs_row) { return modelRowLarger(model, col, lhs_row, rhs_row); };
    for (auto row = 0; row < rows; ++row)
    {
      if (filter(model, row))
      {
        continue;
      }

      largest_rows.push_back(row);
      std::push_heap(largest_rows.begin(), largest_rows.end(), cmp);

      if (largest_rows.size() > n_largest)
      {
        std::pop_heap(largest_rows.begin(), largest_rows.end(), cmp);
        largest_rows.pop_back();
      }
    }

    for (auto row : largest_rows)
    {
      model.setData(
        model.index(row, SymbolTableItemModel::INDEX),
        Qt::CheckState::Checked,
        Qt::CheckStateRole);
    }
  }

  template <typename T = qulonglong>
  static bool modelRowLarger(QAbstractItemModel& model, int col, int left_row, int right_row)
  {
    auto left_idx  = model.index(left_row, col);
    auto right_idx = model.index(right_row, col);
    return model.data(left_idx).value<T>() > model.data(right_idx).value<T>();
  }
};

MainPresenter::MainPresenter(SectionHeaderItemModel& section_header_item_model, SettingsPresenter& settings_presenter, SymbolTableItemModel& symbol_table_item_model, QObject* parent)
    : QObject(parent)
    , _self{ std::make_unique<Impl>(*this,
                                    section_header_item_model,
                                    settings_presenter,
                                    symbol_table_item_model) }
{
}

SectionHeaderItemModel& MainPresenter::sectionHeaderItemModel() noexcept
{
  return _self->section_header_item_model;
}

MainPresenter::~MainPresenter() = default;

QAbstractProxyModel& MainPresenter::sectionHeaderDisplayModel() noexcept
{
  return _self->section_header_check_proxy;
}

QAbstractProxyModel& MainPresenter::sectionHeaderCheckedModel() noexcept
{
  return _self->section_header_selection_proxy;
}

SymbolTableItemModel& MainPresenter::symbolTableItemModel() noexcept
{
  return _self->symbol_table_item_model;
}

QAbstractProxyModel& MainPresenter::symbolTableDisplayModel() noexcept
{
  return _self->symbol_table_check_proxy;
}

QAbstractProxyModel& MainPresenter::symbolTableCheckedModel() noexcept
{
  return _self->symbol_table_selection_proxy;
}

SettingsPresenter& MainPresenter::settingsPresenter() noexcept
{
  return _self->settings_presenter;
}
