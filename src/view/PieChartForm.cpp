#include "PieChartForm.hpp"

#include <QChart>
#include <QPieSeries>
#include <QVPieModelMapper>

#include "ui_PieChartForm.h"

struct PieChartForm::Impl {
  Q_DISABLE_COPY(Impl)

  PieChartForm&    self;
  Ui::PieChartForm ui;
  QChart           chart;
  QVPieModelMapper mapper;

  explicit Impl(PieChartForm& that, QAbstractItemModel& model, int labels_column, int values_column) noexcept
      : self{ that }
      , ui{}
      , chart{}
      , mapper{}
  {
    ui.setupUi(&self);
    self.setChart(&chart);
    self.setRenderHint(QPainter::Antialiasing);
    self.setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    self.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    self.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    auto* series = new QPieSeries();
    chart.addSeries(series);  // [Takes ownership of `series`](https://doc.qt.io/qt-5/qchart.html#addSeries)
    chart.legend()->hide();
    chart.setAnimationOptions(QChart::SeriesAnimations);
    chart.setBackgroundBrush(self.palette().color(QPalette::Normal, QPalette::Window));
    connect(series, &QPieSeries::hovered, [this](QPieSlice* slice, bool is_hovered) {
      slice->setLabelVisible(is_hovered);
      slice->setLabelBrush(self.palette().color(QPalette::Normal, QPalette::Text));
    });
    connect(series, &QPieSeries::clicked, [this](QPieSlice* slice) {
      emit self.sliceSelected(slice->label());
    });
    mapper.setModel(&model);
    mapper.setLabelsColumn(labels_column);
    mapper.setValuesColumn(values_column);
    mapper.setSeries(series);
  }
};

PieChartForm::PieChartForm(QAbstractItemModel& model, int labels_column, int values_column, QWidget* parent)
    : QChartView(parent)
    , _self{ std::make_unique<Impl>(*this, model, labels_column, values_column) }
{
}

PieChartForm::~PieChartForm() = default;
