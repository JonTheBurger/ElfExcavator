#pragma once

#include <QtCharts>  // including <QChartView> doesn't work for whatever reason
#include <memory>

class QAbstractItemModel;

class PieChartForm final : public QChartView {
  Q_OBJECT
  Q_DISABLE_COPY(PieChartForm)

public:
  explicit PieChartForm(QAbstractItemModel& model, int labels_column, int values_column, QWidget* parent = nullptr);
  ~PieChartForm() override;
  void explodeSlice(const QString& label);

signals:
  void sliceSelected(const QString& label);

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
