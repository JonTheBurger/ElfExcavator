#pragma once

#include <QObject>
#include <memory>

class QAbstractItemModel;
class SectionHeaderItemModel;
class SettingsPresenter;
class SymbolTableItemModel;

class MainPresenter final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(MainPresenter)

public:
  MainPresenter(SectionHeaderItemModel& section_header_item_model,
                SettingsPresenter&      settings_presenter,
                SymbolTableItemModel&   symbol_table_item_model,
                QObject*                parent = nullptr);
  ~MainPresenter() override;

  QAbstractItemModel& sectionHeaderItemModel() noexcept;
  QAbstractItemModel& selectedSectionHeaderItemModel() noexcept;
  SettingsPresenter&  settingsPresenter() noexcept;
  QAbstractItemModel& symbolTableItemModel() noexcept;
  QAbstractItemModel& selectedSymbolTableItemModel() noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
