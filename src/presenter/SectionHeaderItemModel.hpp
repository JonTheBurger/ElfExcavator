#pragma once

#include <QAbstractTableModel>
#include <memory>

class ElfFile;

class SectionHeaderItemModel : public QAbstractTableModel {
  Q_OBJECT
  Q_DISABLE_COPY(SectionHeaderItemModel)

public:
  explicit SectionHeaderItemModel(ElfFile& elf_file, QObject* parent = nullptr);
  ~SectionHeaderItemModel() override;
  void onElfFileLoaded();

  // Overrides
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  int      rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int      columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  enum Columns {
    INDEX,
    NAME,
    SIZE,
    ADDRESS,
    LOAD_ADDRESS,
    ALIGNMENT,
    FLAGS,

    ARRAY_SIZE_,
  };

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
