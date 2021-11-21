#pragma once

#include <QHeaderView>
#include <memory>

class MultiFilterTableView;

class MultiFilterHeaderView final : public QHeaderView {
  Q_OBJECT
  Q_DISABLE_COPY(MultiFilterHeaderView)

public:
  explicit MultiFilterHeaderView(MultiFilterTableView* parent);
  ~MultiFilterHeaderView() override;
  QSize sizeHint() const override;
  void  updateGeometries() override;

signals:
  void filterTextChanged(int column, const QString& text);

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
