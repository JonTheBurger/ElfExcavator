#include "SymbolItemModel.hxx"

#include <QMetaEnum>

SymbolItemModel::SymbolItemModel(std::vector<Symbol>& symbolTable, QObject* parent)
    : QAbstractItemModel(parent)
    , _symbolTable(symbolTable)
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

QModelIndex SymbolItemModel::index(int row, int column, const QModelIndex& parent) const
{
  return createIndex(row, column);
}

QModelIndex SymbolItemModel::parent(const QModelIndex& index) const
{
  return QModelIndex();
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
        return QString::number(_symbolTable[index.row()].Address, 16);
      case SIZE:
        return QString::number(_symbolTable[index.row()].Size, 16);
      case TYPES:
      {
        const auto metaEnum = QMetaEnum::fromType<Symbol::Flag>();
        return QString(metaEnum.valueToKeys(_symbolTable[index.row()].Types));
      }
      default:
        break;
    }
  }
  return QVariant();
}
