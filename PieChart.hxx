#ifndef PIECHART_HXX
#define PIECHART_HXX

#include <QtCharts>

class PieChart : public QChartView {
  Q_OBJECT
public:
  explicit PieChart(QWidget* parent = nullptr);
  explicit PieChart(QChart* chart, QWidget* parent = nullptr);

  void insert(const QString& label, qreal value);
  void erase(const QString& label);

  void        zoomIn();
  void        zoomOut();
  inline void zoomReset() { resetMatrix(); }

protected:
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
};

#endif  // PIECHART_HXX
