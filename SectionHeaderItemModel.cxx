#include "SectionHeaderItemModel.hxx"

#include <QMetaEnum>

SectionHeaderItemModel::SectionHeaderItemModel(std::vector<SectionHeader>& sectionHeaders, QObject* parent)
    : QAbstractItemModel(parent)
    , _sectionHeaders(sectionHeaders)
{
}

QVariant SectionHeaderItemModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
  const SectionHeaderItemModel::Columns column = static_cast<SectionHeaderItemModel::Columns>(section);
  if ((role == Qt::DisplayRole) && (orientation == Qt::Orientation::Horizontal))
  {
    switch (column)
    {
      case NAME:
        return tr("Name");
      case SIZE:
        return tr("Size");
      case VMA:
        return tr("Virtual Memory Address");
      case LMA:
        return tr("Load Memory Address");
      case FILE_OFF:
        return tr("File Offset");
      case TYPES:
        return tr("Type Flags");
      case ALIGN:
        return tr("Alignment");
      default:
        break;
    }
  }
  return QVariant();
}

QModelIndex SectionHeaderItemModel::index(int row, int column, const QModelIndex& parent) const
{
  return createIndex(row, column);
}

QModelIndex SectionHeaderItemModel::parent(const QModelIndex& index) const
{
  return QModelIndex();
}

int SectionHeaderItemModel::rowCount(const QModelIndex& parent) const
{
  return _sectionHeaders.size();
}

int SectionHeaderItemModel::columnCount(const QModelIndex& parent) const
{
  return SectionHeaderItemModel::Columns::MAX;
}

QVariant SectionHeaderItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case NAME:
        return _sectionHeaders[index.row()].Name;
      case SIZE:
        return QString::number(_sectionHeaders[index.row()].Size, 16);
      case VMA:
        return QString::number(_sectionHeaders[index.row()].Vma, 16);
      case LMA:
        return QString::number(_sectionHeaders[index.row()].Lma, 16);
      case FILE_OFF:
        return QString::number(_sectionHeaders[index.row()].FileOff, 16);
      case TYPES:
      {
        const auto metaEnum = QMetaEnum::fromType<SectionHeader::Flag>();
        return QString(metaEnum.valueToKeys(_sectionHeaders[index.row()].Types));
      }
      case ALIGN:
        return _sectionHeaders[index.row()].Align;
      default:
        break;
    }
  }
  return QVariant();
}
