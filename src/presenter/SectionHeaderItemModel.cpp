#include "SectionHeaderItemModel.hpp"

#include <QAbstractItemModel>
#include <unordered_map>

#include "model/ElfFile.hpp"

struct SectionHeaderItemModel::Impl {
  Q_DISABLE_COPY(Impl)

  SectionHeaderItemModel& self;
  ElfFile&                elf_file;

  explicit Impl(SectionHeaderItemModel& that, ElfFile& elf_file_) noexcept
      : self{ that }
      , elf_file{ elf_file_ }
  {
  }
};

SectionHeaderItemModel::SectionHeaderItemModel(ElfFile& elf_file, QObject* parent)
    : QAbstractTableModel(parent)
    , _self{ std::make_unique<Impl>(*this, elf_file) }
{
}

SectionHeaderItemModel::~SectionHeaderItemModel() = default;

void SectionHeaderItemModel::onElfFileLoaded()
{
  beginResetModel();
  endResetModel();
}

QVariant SectionHeaderItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
      case SIZE:
        return tr("Size");
      case ADDRESS:
        return tr("Address");
      case LOAD_ADDRESS:
        return tr("Load Address");
      case ALIGNMENT:
        return tr("Alignment");
      case FLAGS:
        return tr("Flags");
      default:
        break;
    }
  }

  return {};
}

int SectionHeaderItemModel::rowCount(const QModelIndex& parent) const
{
  static_cast<void>(parent);
  return static_cast<int>(_self->elf_file.sections().size());
}

int SectionHeaderItemModel::columnCount(const QModelIndex& parent) const
{
  static_cast<void>(parent);
  return ARRAY_SIZE_;
}

QVariant SectionHeaderItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || (index.row() >= _self->elf_file.sections().size())) { return {}; }

  if (role == Qt::DisplayRole)
  {
    const auto idx = static_cast<unsigned>(index.row());
    switch (static_cast<Columns>(index.column()))
    {
      case INDEX:
        return _self->elf_file.sections()[idx].index;
      case NAME:
        return QString::fromStdString(_self->elf_file.sections()[idx].name);
      case SIZE:
        return static_cast<quint64>(_self->elf_file.sections()[idx].size);
      case ADDRESS:
        return static_cast<quint64>(_self->elf_file.sections()[idx].address);
      case LOAD_ADDRESS:
        // TODO: Determine
        return static_cast<quint64>(_self->elf_file.sections()[idx].address);
      case ALIGNMENT:
        return static_cast<quint16>(_self->elf_file.sections()[idx].addr_align);
      case FLAGS:
        return static_cast<quint64>(_self->elf_file.sections()[idx].flags);
      default:
        break;
    }
  }
  return QVariant();
}
