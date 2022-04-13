#include "MultiFilterProxyModel.hpp"

#include <QRegularExpression>

#include "model/TextFilter.hpp"

struct MultiFilterProxyModel::Impl {
  Q_DISABLE_COPY(Impl)

  MultiFilterProxyModel&          self;
  std::vector<QRegularExpression> filters;

  explicit Impl(MultiFilterProxyModel& that) noexcept
      : self{ that }
      , filters{}
  {
  }
};

MultiFilterProxyModel::MultiFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , _self{ std::make_unique<Impl>(*this) }
{
}

MultiFilterProxyModel::~MultiFilterProxyModel() = default;

void MultiFilterProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
  const auto columns = static_cast<size_t>(sourceModel->columnCount());
  _self->filters.clear();
  _self->filters.reserve(columns);

  for (auto i = 0u; i < columns; ++i)
  {
    _self->filters.emplace_back();
  }

  QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool MultiFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
  if (sourceModel() == nullptr) { return false; }

  bool accepted = true;

  for (auto col = 0; (accepted) && (col < sourceModel()->columnCount()); ++col)
  {
    const auto idx = static_cast<size_t>(col);
    if (_self->filters[idx].pattern().isEmpty())
    {
      continue;
    }

    const QVariant data = sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent), filterRole());

    if (data.userType() == QMetaType::QString)
    {
      emit filterValidityChanged(col, _self->filters[idx].isValid());
      if (_self->filters[idx].isValid())
      {
        accepted = accepted && _self->filters[idx].match(data.toString()).hasMatch();
      }
    }
    else if (data.canConvert<qulonglong>())
    {
      auto filter_result = filterValueWithExpression(_self->filters[idx].pattern().toStdString(), data.value<qulonglong>());
      emit filterValidityChanged(col, filter_result != FilterResult_SYNTAX_ERROR);
      accepted = accepted && (filter_result != FilterResult_FAIL);
    }
  }

  return accepted;
}

void MultiFilterProxyModel::setColumnFilter(int column, const QString& text)
{
  const auto idx = static_cast<size_t>(column);
  if ((idx < _self->filters.size()) && (_self->filters[idx].pattern() != text))
  {
    _self->filters[idx].setPattern(text);
    invalidateFilter();

    if (text.isEmpty())
    {
      emit filterValidityChanged(static_cast<uint8_t>(idx), true);
    }
  }
}
