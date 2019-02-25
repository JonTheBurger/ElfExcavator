#ifndef MULTIFILTERPROXYMODEL_HXX
#define MULTIFILTERPROXYMODEL_HXX

#include <QSortFilterProxyModel>
#include <memory>
#include <vector>

class QRegularExpression;
class MultiFilterProxyModel : public QSortFilterProxyModel {
public:
  explicit MultiFilterProxyModel(QObject* parent = nullptr);
  ~MultiFilterProxyModel();
  void setSourceModel(QAbstractItemModel* sourceModel) override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:
  void setColumnFilter(int column, const QString& text);

private:
  std::vector<std::unique_ptr<QRegularExpression>> _filters;
};

#endif  // MULTIFILTERPROXYMODEL_HXX
