#include "SymbolTableItemModel.hpp"

#include <QAbstractItemModel>

#include "model/ElfFile.hpp"

struct SymbolTableItemModel::Impl {
  Q_DISABLE_COPY(Impl)

  SymbolTableItemModel& self;
  ElfFile&              elf_file;

  explicit Impl(SymbolTableItemModel& that, ElfFile& elf_file_) noexcept
      : self{ that }
      , elf_file{ elf_file_ }
  {
  }
};

SymbolTableItemModel::SymbolTableItemModel(ElfFile& elf_file, QObject* parent)
    : QAbstractTableModel(parent)
    , _self{ std::make_unique<Impl>(*this, elf_file) }
{
}

void SymbolTableItemModel::onElfFileLoaded()
{
  beginResetModel();
  endResetModel();
}

SymbolTableItemModel::~SymbolTableItemModel() = default;

QVariant SymbolTableItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Orientation::Vertical) { return {}; }

  if (role == Qt::DisplayRole)
  {
    switch (static_cast<Columns>(section))
    {
      case INDEX:
        return tr("Index");
      case NAME:
        return tr("Name");
      case MANGLED_NAME:
        return tr("Mangled Name");
      case VALUE:
        return tr("Value");
      case SIZE:
        return tr("Size");
      case BIND:
        return tr("Bind");
      case TYPE:
        return tr("Type");
      case SECTION_INDEX:
        return tr("Section Index");
      case OTHER:
        return tr("Other");
      default:
        break;
    }
  }

  return {};
}

int SymbolTableItemModel::rowCount(const QModelIndex& parent) const
{
  static_cast<void>(parent);
  return static_cast<int>(_self->elf_file.symbols().size());
}

int SymbolTableItemModel::columnCount(const QModelIndex& parent) const
{
  static_cast<void>(parent);
  return ARRAY_SIZE_;
}

QVariant SymbolTableItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || (index.row() >= _self->elf_file.symbols().size())) { return {}; }

  if (role == Qt::DisplayRole)
  {
    const auto idx = static_cast<unsigned>(index.row());
    switch (static_cast<Columns>(index.column()))
    {
      case INDEX:
        return static_cast<quint16>(_self->elf_file.symbols()[idx].index);
      case NAME:
        return QString::fromStdString(_self->elf_file.symbols()[idx].name);
      case MANGLED_NAME:
        return QString::fromStdString(_self->elf_file.symbols()[idx].mangled_name);
      case VALUE:
        return static_cast<quint64>(_self->elf_file.symbols()[idx].value);
      case SIZE:
        return static_cast<quint64>(_self->elf_file.symbols()[idx].size);
      case BIND:
        return _self->elf_file.symbols()[idx].bind;
      case TYPE:
        return _self->elf_file.symbols()[idx].type;
      case SECTION_INDEX:
        return _self->elf_file.symbols()[idx].section_index;
      case OTHER:
        return _self->elf_file.symbols()[idx].other;
      default:
        break;
    }
  }
  return QVariant();
}
