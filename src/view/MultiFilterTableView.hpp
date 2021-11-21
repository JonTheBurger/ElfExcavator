#pragma once

#include <QTableView>
#include <memory>

class MultiFilterProxyModel;

class MultiFilterTableView final : public QTableView {
  Q_OBJECT
  Q_DISABLE_COPY(MultiFilterTableView)

public:
  explicit MultiFilterTableView(QWidget* parent = nullptr);
  ~MultiFilterTableView() override;

  void                   resizeColumns();
  MultiFilterProxyModel* model() noexcept;
  void                   setModel(QAbstractItemModel* model) override;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
