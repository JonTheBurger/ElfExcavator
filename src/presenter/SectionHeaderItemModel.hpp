#pragma once

#include <QAbstractTableModel>
#include <memory>

#include "model/ElfFile.hpp"

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
    TYPE,
    FLAGS,

    ARRAY_SIZE_,
  };

  enum Role {
    Raw = Qt::UserRole,
  };

  enum Flag {
    WRITE            = Section::Flags_WRITE,
    ALLOC            = Section::Flags_ALLOC,
    EXECINSTR        = Section::Flags_EXECINSTR,
    MERGE            = Section::Flags_MERGE,
    STRINGS          = Section::Flags_STRINGS,
    INFO_LINK        = Section::Flags_INFO_LINK,
    LINK_ORDER       = Section::Flags_LINK_ORDER,
    OS_NONCONFORMING = Section::Flags_OS_NONCONFORMING,
    GROUP            = Section::Flags_GROUP,
    TLS              = Section::Flags_TLS,
  };
  Q_DECLARE_FLAGS(Flags, Flag)
  Q_FLAG(Flag)

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SectionHeaderItemModel::Flags)
