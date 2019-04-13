#include "MultiFilterProxyModel.hxx"

#include <QJSEngine>

MultiFilterProxyModel::MultiFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , _engine(std::make_unique<QJSEngine>())
{
}

MultiFilterProxyModel::~MultiFilterProxyModel() = default;

void MultiFilterProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
  _filters.clear();
  for (auto i = 0; i < sourceModel->columnCount(); ++i)
  {
    _filters.push_back(QRegularExpression{});
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

    if (data.userType() == QMetaType::QString)
    {
      accepted = accepted && _filters.at(col).match(data.toString()).hasMatch();
    }
    else if (data.canConvert<qlonglong>())
    {
      accepted = accepted && FilterAcceptsNumber(data.value<qlonglong>(), _filters.at(col).pattern());
    }
  }

  return accepted;
}

void MultiFilterProxyModel::setColumnFilter(int column, const QString& text)
{
  _filters.at(column).setPattern(text);
  invalidateFilter();
}

bool ParseNumber(const QStringRef& str, qlonglong& number)
{
  bool            decimalParse  = false;
  const qlonglong decimalNumber = str.toLongLong(&decimalParse, 10);

  bool            hexParse  = false;
  const qlonglong hexNumber = str.toLongLong(&hexParse, 16);

  const bool ok = decimalParse || hexParse;
  number        = decimalParse ? decimalNumber : (hexParse ? hexNumber : 0);
  return ok;
}

bool MultiFilterProxyModel::FilterAcceptsNumber(qlonglong number, const QString& str) const
{
  if (str.isEmpty()) { return true; }

  QVector<QStringRef> tokens = str.splitRef(' ', QString::SplitBehavior::SkipEmptyParts);
  if (tokens.empty()) { return true; }

  qlonglong comparator = 0;
  if (ParseNumber(tokens[0], comparator))
  {
    return comparator == number;
  }
  else
  {
    const auto expression = QString("%1 %2").arg(number).arg(str);
    return _engine->evaluate(expression).toBool();
  }
}
