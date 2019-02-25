#include "SymbolItemModel.hxx"

#include <QMetaEnum>

SymbolItemModel::SymbolItemModel(std::vector<Symbol>& symbolTable, QObject* parent)
    : QAbstractTableModel(parent)
    , _symbolTable(symbolTable)
{
}

QVariant SymbolItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Orientation::Vertical) { return QVariant(); }
  const auto column = static_cast<SymbolItemModel::Columns>(section);

  if (role == Qt::DisplayRole)
  {
    switch (column)
    {
      case INDEX:
        return tr("Index");
      case NAME:
        return tr("Name");
      case SECTION_NAME:
        return tr("Section");
      case ADDRESS:
        return tr("Address/Value");
      case SIZE:
        return tr("Size/Alignment");
      case TYPES:
        return tr("Type Flags");
      default:
        break;
    }
  }

  return QVariant();
}

int SymbolItemModel::rowCount(const QModelIndex& parent) const
{
  (void)parent;
  return _symbolTable.size();
}

int SymbolItemModel::columnCount(const QModelIndex& parent) const
{
  (void)parent;
  return SymbolItemModel::Columns::MAX;
}

QVariant SymbolItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) { return QVariant(); }

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case INDEX:
        return QVariant::fromValue(_symbolTable[index.row()].Index);
      case NAME:
        return _symbolTable[index.row()].Name;
      case SECTION_NAME:
        return _symbolTable[index.row()].SectionName;
      case ADDRESS:
        return QVariant::fromValue<qulonglong>(_symbolTable[index.row()].Address);
      case SIZE:
        return QVariant::fromValue<qulonglong>(_symbolTable[index.row()].Size);
      case TYPES:
      {
        const auto metaEnum = QMetaEnum::fromType<Symbol::Flag>();
        return QString(metaEnum.valueToKeys(_symbolTable[index.row()].Types));
      }
      default:
        break;
    }
  }
  else if ((role == Qt::CheckStateRole) && (index.column() == Columns::INDEX))
  {
    return _symbolTable[index.row()].Display ? Qt::Checked : Qt::Unchecked;
  }

  return QVariant();
}

Qt::ItemFlags SymbolItemModel::flags(const QModelIndex& index) const
{
  if (index.column() == Columns::INDEX)
  {
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QAbstractTableModel::flags(index);
}

bool SymbolItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::CheckStateRole)
  {
    _symbolTable[index.row()].Display = (Qt::Checked == static_cast<Qt::CheckState>(value.toInt()));
    emit dataChanged(index, index, { role });
    return true;
  }
  return false;
}
