#ifndef SYMBOLITEMMODEL_HXX
#define SYMBOLITEMMODEL_HXX

#include <QAbstractTableModel>
#include <vector>

#include "Symbol.hxx"

class SymbolItemModel : public QAbstractTableModel {
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

  explicit SymbolItemModel(std::vector<Symbol>& symbolTable, std::vector<Qt::CheckState>& symbolCheckState, QObject* parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  // Edit
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  bool          setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
  std::vector<Symbol>&         _symbolTable;
  std::vector<Qt::CheckState>& _symbolCheckState;
};

#endif  // SYMBOLITEMMODEL_HXX
