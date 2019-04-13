#ifndef MULTIFILTERPROXYMODEL_HXX
#define MULTIFILTERPROXYMODEL_HXX

#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <memory>
#include <vector>

class QJSEngine;
/**
 * Filters are stored as regexp instead of QString despite numeric columns not using regexp.
 * QRegularExpression will optimize itself after first usage, which is preferrable to maintaining
 * a separate QRegularExpression vector or constructing new instances on each use.
 */
class MultiFilterProxyModel : public QSortFilterProxyModel {
public:
  explicit MultiFilterProxyModel(QObject* parent = nullptr);
  ~MultiFilterProxyModel();
  void setSourceModel(QAbstractItemModel* sourceModel) override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
  bool FilterAcceptsNumber(qlonglong number, const QString& str) const;

public slots:
  void setColumnFilter(int column, const QString& text);

private:
  std::unique_ptr<QJSEngine>      _engine;
  std::vector<QRegularExpression> _filters;
};

#endif  // MULTIFILTERPROXYMODEL_HXX
