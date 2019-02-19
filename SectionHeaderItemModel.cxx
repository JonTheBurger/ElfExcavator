#include "SectionHeaderItemModel.hxx"

#include <QMetaEnum>

SectionHeaderItemModel::SectionHeaderItemModel(std::vector<SectionHeader>& sectionHeaders, std::vector<Qt::CheckState>& sectionCheckState, QObject* parent)
    : QAbstractTableModel(parent)
    , _sectionHeaders(sectionHeaders)
    , _sectionCheckState(sectionCheckState)
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
  else if ((role == Qt::CheckStateRole) && (index.column() == NAME))
  {
    return _sectionCheckState[index.row()];
  }
  return QVariant();
}

Qt::ItemFlags SectionHeaderItemModel::flags(const QModelIndex& index) const
{
  if (index.column() == Columns::NAME)
  {
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QAbstractItemModel::flags(index);
}

bool SectionHeaderItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::CheckStateRole)
  {
    _sectionCheckState[index.row()] = static_cast<Qt::CheckState>(value.toInt());
    emit dataChanged(index, index, { role });
    return true;
  }
  return false;
}
