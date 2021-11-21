#pragma once

#include <QSortFilterProxyModel>
#include <memory>

class MultiFilterProxyModel final : public QSortFilterProxyModel {
  Q_OBJECT
  Q_DISABLE_COPY(MultiFilterProxyModel)

public:
  explicit MultiFilterProxyModel(QObject* parent = nullptr);
  ~MultiFilterProxyModel() override;
  void setSourceModel(QAbstractItemModel* sourceModel) override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

signals:
  void filterValidityChanged(int column, bool is_valid) const;

public slots:
  void setColumnFilter(int column, const QString& text);

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
