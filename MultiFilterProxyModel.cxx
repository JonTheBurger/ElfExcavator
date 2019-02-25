#include "MultiFilterProxyModel.hxx"

#include <QRegularExpression>

MultiFilterProxyModel::MultiFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

MultiFilterProxyModel::~MultiFilterProxyModel() = default;

void MultiFilterProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
  _filters.clear();
  for (auto i = 0; i < sourceModel->columnCount(); ++i)
  {
    _filters.push_back(std::make_unique<QRegularExpression>());
  }
  QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool MultiFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  if (sourceModel() == nullptr) { return false; }

  bool accepted = true;

  for (auto col = 0; col < sourceModel()->columnCount(); ++col)
  {
    const auto     index = sourceModel()->index(sourceRow, col, sourceParent);
    const QVariant data  = sourceModel()->data(index, filterRole());
    if (data.canConvert<QString>())
    {
      accepted = accepted && _filters.at(col)->match(data.toString()).hasMatch();
    }
  }

  return accepted;
}

void MultiFilterProxyModel::setColumnFilter(int column, const QString& text)
{
  _filters.at(column)->setPattern(text);
  invalidateFilter();
}
