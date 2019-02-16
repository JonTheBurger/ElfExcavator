#ifndef SYMBOLITEMMODEL_HXX
#define SYMBOLITEMMODEL_HXX

#include <QtCore/QAbstractItemModel>
#include <vector>

#include "Symbol.hxx"

class SymbolItemModel : public QAbstractItemModel {
  Q_OBJECT

public:
  enum Columns {
    NAME,
    SECTION_NAME,
    ADDRESS,
    SIZE,
    TYPES,

    MAX,
  };

  explicit SymbolItemModel(std::vector<Symbol>& symbolTable, QObject* parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  std::vector<Symbol>& _symbolTable;
};

#endif  // SYMBOLITEMMODEL_HXX
