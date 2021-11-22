#include "SectionHeaderItemModel.hpp"

#include <QAbstractItemModel>
#include <QMetaEnum>
#include <unordered_map>

#include "model/ElfFile.hpp"

struct SectionHeaderItemModel::Impl {
  Q_DISABLE_COPY(Impl)

  SectionHeaderItemModel& self;
  ElfFile&                elf_file;
  QMetaEnum               flags_enum;

  explicit Impl(SectionHeaderItemModel& that, ElfFile& elf_file_) noexcept
      : self{ that }
      , elf_file{ elf_file_ }
      , flags_enum{ QMetaEnum::fromType<SectionHeaderItemModel::Flag>() }
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
      case TYPE:
        return tr("Type");
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
      case TYPE:
        switch (_self->elf_file.sections()[idx].type)
        {
          case Section::Type_NULL:
            return tr("NULL");
          case Section::Type_PROGBITS:
            return tr("PROGBITS");
          case Section::Type_SYMTAB:
            return tr("SYMTAB");
          case Section::Type_STRTAB:
            return tr("STRTAB");
          case Section::Type_RELA:
            return tr("RELA");
          case Section::Type_HASH:
            return tr("HASH");
          case Section::Type_DYNAMIC:
            return tr("DYNAMIC");
          case Section::Type_NOTE:
            return tr("NOTE");
          case Section::Type_NOBITS:
            return tr("NOBITS");
          case Section::Type_REL:
            return tr("REL");
          case Section::Type_SHLIB:
            return tr("SHLIB");
          case Section::Type_DYNSYM:
            return tr("DYNSYM");
          case Section::Type_INIT_ARRAY:
            return tr("INIT_ARRAY");
          case Section::Type_FINI_ARRAY:
            return tr("FINI_ARRAY");
          case Section::Type_PREINIT_ARRAY:
            return tr("PREINIT_ARRAY");
          case Section::Type_GROUP:
            return tr("GROUP");
          case Section::Type_SYMTAB_SHNDX:
            return tr("SYMTAB_SHNDX");
          default:
            return QString();
        }
      case FLAGS:
        return _self->flags_enum.valueToKeys(static_cast<uint8_t>(_self->elf_file.sections()[idx].flags));
      default:
        break;
    }
  }
  return QVariant();
}
