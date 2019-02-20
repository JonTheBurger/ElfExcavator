#include "SectionHeaderItemModel.hxx"

#include <QMetaEnum>

SectionHeaderItemModel::SectionHeaderItemModel(std::vector<SectionHeader>& sectionHeaders, QObject* parent)
    : QAbstractTableModel(parent)
    , _sectionHeaders(sectionHeaders)
{
}

QVariant SectionHeaderItemModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
  if (orientation == Qt::Orientation::Vertical) { return QVariant(); }
  const auto column = static_cast<SectionHeaderItemModel::Columns>(section);

  if (role == Qt::DisplayRole)
  {
    switch (column)
    {
      case INDEX:
        return tr("Index");
      case NAME:
        return tr("Name");
      case SIZE:
        return tr("Size");
      case VMA:
        return tr("Virtual Address");
      case LMA:
        return tr("Load Address");
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

int SectionHeaderItemModel::rowCount(const QModelIndex& parent) const
{
  (void)parent;
  return _sectionHeaders.size();
}

int SectionHeaderItemModel::columnCount(const QModelIndex& parent) const
{
  (void)parent;
  return SectionHeaderItemModel::Columns::MAX;
}

QVariant SectionHeaderItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) { return QVariant(); }

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case INDEX:
        return static_cast<qulonglong>(_sectionHeaders[index.row()].Index);
      case NAME:
        return _sectionHeaders[index.row()].Name;
      case SIZE:
        return static_cast<qulonglong>(_sectionHeaders[index.row()].Size);
      case VMA:
        return static_cast<qulonglong>(_sectionHeaders[index.row()].Vma);
      case LMA:
        return static_cast<qulonglong>(_sectionHeaders[index.row()].Lma);
      case FILE_OFF:
        return static_cast<qulonglong>(_sectionHeaders[index.row()].FileOff);
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
  else if ((role == Qt::CheckStateRole) && (index.column() == INDEX))
  {
    return _sectionHeaders[index.row()].Display ? Qt::Checked : Qt::Unchecked;
  }
  return QVariant();
}

Qt::ItemFlags SectionHeaderItemModel::flags(const QModelIndex& index) const
{
  if (index.column() == Columns::INDEX)
  {
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QAbstractTableModel::flags(index);
}

bool SectionHeaderItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::CheckStateRole)
  {
    _sectionHeaders[index.row()].Display = (Qt::Checked == static_cast<Qt::CheckState>(value.toInt()));
    emit dataChanged(index, index, { role });
    return true;
  }
  return false;
}
