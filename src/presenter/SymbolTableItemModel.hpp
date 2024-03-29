#pragma once

#include <QAbstractTableModel>
#include <memory>

class ElfFile;

class SymbolTableItemModel final : public QAbstractTableModel {
  Q_OBJECT
  Q_DISABLE_COPY(SymbolTableItemModel)

public:
  explicit SymbolTableItemModel(ElfFile& elf_file, QObject* parent = nullptr);
  ~SymbolTableItemModel() override;
  void        onElfFileLoaded();
  QModelIndex indexOfSymbol(QStringView demangled_name);

  // Overrides
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  int      rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int      columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  enum Columns {
    INDEX,
    DEMANGLED_NAME,
    NAME,
    VALUE,
    SIZE,
    BIND,
    TYPE,
    SECTION,
    VISIBILITY,

    ARRAY_SIZE_,
  };

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
