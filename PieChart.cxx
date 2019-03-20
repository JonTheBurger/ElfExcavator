#include "PieChart.hxx"

static constexpr double ZOOM_RATIO = 1.1;
static constexpr int    PAN_PX     = 10;

PieChart::PieChart(QWidget* parent)
    : PieChart(nullptr, parent)
{
}

PieChart::PieChart(QChart* chart, QWidget* parent)
    : QChartView(chart, parent)
{
  setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void PieChart::zoomIn()
{
  scale(ZOOM_RATIO, ZOOM_RATIO);
}

void PieChart::zoomOut()
{
  scale(1 / ZOOM_RATIO, 1 / ZOOM_RATIO);
  if (matrix().m11() < 1) { zoomReset(); }
}

void PieChart::wheelEvent(QWheelEvent* event)
{
  if (event->modifiers() & Qt::ControlModifier)
  {
    if (event->delta() < 0)
    {
      zoomOut();
    }
    else
    {
      zoomIn();
    }
  }
  else
  {
    QChartView::wheelEvent(event);
  }
}

void PieChart::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Delete:
      zoomReset();
      break;
    case Qt::Key_Plus:
      zoomIn();
      break;
    case Qt::Key_Minus:
      zoomOut();
      break;
    case Qt::Key_A:
      Q_FALLTHROUGH();
    case Qt::Key_Left:
      horizontalScrollBar()->setValue(horizontalScrollBar()->value() - PAN_PX);
      break;
    case Qt::Key_D:
      Q_FALLTHROUGH();
    case Qt::Key_Right:
      horizontalScrollBar()->setValue(horizontalScrollBar()->value() + PAN_PX);
      break;
    case Qt::Key_W:
      Q_FALLTHROUGH();
    case Qt::Key_Up:
      verticalScrollBar()->setValue(verticalScrollBar()->value() - PAN_PX);
      break;
    case Qt::Key_S:
      Q_FALLTHROUGH();
    case Qt::Key_Down:
      verticalScrollBar()->setValue(verticalScrollBar()->value() + PAN_PX);
      break;
    default:
      QGraphicsView::keyPressEvent(event);
      break;
  }
}
