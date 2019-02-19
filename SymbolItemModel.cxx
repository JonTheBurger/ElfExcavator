#include "SymbolItemModel.hxx"

#include <QMetaEnum>

SymbolItemModel::SymbolItemModel(std::vector<Symbol>& symbolTable, std::vector<Qt::CheckState>& symbolCheckState, QObject* parent)
    : QAbstractTableModel(parent)
    , _symbolTable(symbolTable)
    , _symbolCheckState(symbolCheckState)
{
}

QVariant SymbolItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  const SymbolItemModel::Columns column = static_cast<SymbolItemModel::Columns>(section);
  if ((role == Qt::DisplayRole) && (orientation == Qt::Orientation::Horizontal))
  {
    switch (column)
    {
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
  return _symbolTable.size();
}

int SymbolItemModel::columnCount(const QModelIndex& parent) const
{
  return SymbolItemModel::Columns::MAX;
}

QVariant SymbolItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    switch (index.column())
    {
      case NAME:
        return _symbolTable[index.row()].Name;
      case SECTION_NAME:
        return _symbolTable[index.row()].SectionName;
      case ADDRESS:
        return static_cast<qulonglong>(_symbolTable[index.row()].Address);
      case SIZE:
        return static_cast<qulonglong>(_symbolTable[index.row()].Size);
      case TYPES:
      {
        const auto metaEnum = QMetaEnum::fromType<Symbol::Flag>();
        return QString(metaEnum.valueToKeys(_symbolTable[index.row()].Types));
      }
      default:
        break;
    }
  }
  else if ((role == Qt::CheckStateRole) && (index.column() == NAME))
  {
    return _symbolCheckState[index.row()];
  }
  return QVariant();
}

Qt::ItemFlags SymbolItemModel::flags(const QModelIndex& index) const
{
  if (index.column() == Columns::NAME)
  {
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
  }
  return QAbstractItemModel::flags(index);
}

bool SymbolItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == Qt::CheckStateRole)
  {
    _symbolCheckState[index.row()] = static_cast<Qt::CheckState>(value.toInt());
    emit dataChanged(index, index, { role });
    return true;
  }
  return false;
}
