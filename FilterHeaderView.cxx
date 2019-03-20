#include "FilterHeaderView.hxx"

#include <QLineEdit>
#include <QScrollBar>

FilterHeaderView::FilterHeaderView(QTableView* parent)
    : QHeaderView(Qt::Orientation::Horizontal, parent)
{
  for (auto col = 0; col < parent->model()->columnCount(); ++col)
  {
    _lineEdits.push_back(new QLineEdit(this));
    _lineEdits[col]->setClearButtonEnabled(true);
    _lineEdits[col]->setPlaceholderText(tr("Filter..."));
    connect(_lineEdits[col], &QLineEdit::textChanged, [this, col](const QString& text) {
      filterTextChanged(col, text);
    });
  }
  connect(this, &QHeaderView::sectionResized, [this]() { drawLabels(); });
  connect(this, &QHeaderView::sectionMoved, [this]() { drawLabels(); });
  connect(parent->horizontalScrollBar(), &QScrollBar::valueChanged, [this]() { drawLabels(); });

  setSectionsClickable(true);
  setSectionsMovable(true);
}

FilterHeaderView::~FilterHeaderView() = default;

QSize FilterHeaderView::sizeHint() const
{
  QSize size = QHeaderView::sizeHint();
  size.setHeight(size.height() + _lineEdits.at(0)->sizeHint().height());
  return size;
}

void FilterHeaderView::updateGeometries()
{
  setViewportMargins(0, 0, 0, _lineEdits.at(0)->sizeHint().height());
  QHeaderView::updateGeometries();
  drawLabels();
}

void FilterHeaderView::drawLabels()
{
  for (auto i = 0; i < _lineEdits.size(); ++i)
  {
    _lineEdits[i]->move(sectionPosition(i) - offset(), height() / 2);
    _lineEdits[i]->resize(sectionSize(i), height() / 2);
  }
}
