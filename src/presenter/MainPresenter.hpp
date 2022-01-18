#pragma once

#include <QObject>
#include <memory>

class ElfFile;
class QAbstractProxyModel;
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

  SectionHeaderItemModel& sectionHeaderItemModel() noexcept;
  QAbstractProxyModel&    sectionHeaderDisplayModel() noexcept;
  QAbstractProxyModel&    sectionHeaderCheckedModel() noexcept;

  SymbolTableItemModel& symbolTableItemModel() noexcept;
  QAbstractProxyModel&  symbolTableDisplayModel() noexcept;
  QAbstractProxyModel&  symbolTableCheckedModel() noexcept;

  SettingsPresenter& settingsPresenter() noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
