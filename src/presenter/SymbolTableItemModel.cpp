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
      case DEMANGLED_NAME:
        return tr("Demangled Name");
      case NAME:
        return tr("Name");
      case VALUE:
        return tr("Value");
      case SIZE:
        return tr("Size");
      case BIND:
        return tr("Bind");
      case TYPE:
        return tr("Type");
      case SECTION:
        return tr("Section");
      case VISIBILITY:
        return tr("Visibility");
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
      case DEMANGLED_NAME:
        return QString::fromStdString(_self->elf_file.symbols()[idx].name);
      case NAME:
        return QString::fromStdString(_self->elf_file.symbols()[idx].mangled_name);
      case VALUE:
        return static_cast<quint64>(_self->elf_file.symbols()[idx].value);
      case SIZE:
        return static_cast<quint64>(_self->elf_file.symbols()[idx].size);
      case BIND:
        switch (_self->elf_file.symbols()[idx].bind)
        {
          case Symbol::Bind_LOCAL:
            return tr("LOCAL");
          case Symbol::Bind_GLOBAL:
            return tr("GLOBAL");
          case Symbol::Bind_WEAK:
            return tr("WEAK");
          case Symbol::Bind_LOOS:
            return tr("LOOS");
          case Symbol::Bind_HIOS:
            return tr("HIOS");
          case Symbol::Bind_MULTIDEF:
            return tr("MULTIDEF/LOPROC");
          case Symbol::Bind_HIPROC:
            return tr("HIPROC");
          default:
            return QString();
        }
      case TYPE:
        switch (_self->elf_file.symbols()[idx].type)
        {
          case Symbol::Type_NOTYPE:
            return tr("NOTYPE");
          case Symbol::Type_OBJECT:
            return tr("OBJECT");
          case Symbol::Type_FUNC:
            return tr("FUNC");
          case Symbol::Type_SECTION:
            return tr("SECTION");
          case Symbol::Type_FILE:
            return tr("FILE");
          case Symbol::Type_COMMON:
            return tr("COMMON");
          case Symbol::Type_TLS:
            return tr("TLS");
          case Symbol::Type_AMDGPU_HSA_KERNEL:
            return tr("LOOS/AMDGPU_HSA_KERNEL");
          case Symbol::Type_HIOS:
            return tr("HIOS");
          case Symbol::Type_LOPROC:
            return tr("LOPROC");
          case Symbol::Type_HIPROC:
            return tr("HIPROC");
          default:
            return QString();
        }
      case SECTION: {
        auto section = _self->elf_file.symbols()[idx].section_index;
        if (section < _self->elf_file.sections().size())
        {
          return QString::fromStdString(_self->elf_file.sections()[section].name);
        }
        return QString();
      }
      case VISIBILITY:
        switch (_self->elf_file.symbols()[idx].visibility)
        {
          case Symbol::Visibility_DEFAULT:
            return tr("DEFAULT");
          case Symbol::Visibility_INTERNAL:
            return tr("INTERNAL");
          case Symbol::Visibility_HIDDEN:
            return tr("HIDDEN");
          case Symbol::Visibility_PROTECTED:
            return tr("PROTECTED");
          default:
            return QString();
        }
      default:
        break;
    }
  }
  return QVariant();
}
