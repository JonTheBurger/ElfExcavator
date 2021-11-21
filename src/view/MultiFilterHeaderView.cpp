#include "MultiFilterHeaderView.hpp"

#include <QHeaderView>
#include <QLineEdit>
#include <QScrollBar>

#include "MultiFilterTableView.hpp"
#include "presenter/MultiFilterProxyModel.hpp"

static const QString INVALID_FILTER_STYLESHEET = QStringLiteral("border: 1px solid red");

struct MultiFilterHeaderView::Impl {
  Q_DISABLE_COPY(Impl)

  MultiFilterHeaderView&  self;
  std::vector<QLineEdit*> line_edits;  ///< Must use pointer (instead of unique_ptr) because parent most own widget to draw

  explicit Impl(MultiFilterHeaderView& that) noexcept
      : self{ that }
      , line_edits{}
  {
    auto&      table_view = *static_cast<MultiFilterTableView*>(self.parent());
    auto*      model      = table_view.model();
    const auto columns    = static_cast<size_t>(model->columnCount());
    line_edits.reserve(columns);

    for (size_t col = 0; col < columns; ++col)
    {
      line_edits.push_back(new QLineEdit(&self));  // self takes ownership
      line_edits[col]->setClearButtonEnabled(true);
      line_edits[col]->setPlaceholderText(tr("Filter..."));
      connect(line_edits[col], &QLineEdit::textChanged, [this, col](const QString& text) {
        emit self.filterTextChanged(static_cast<int>(col), text);
      });
    }
    connect(&self, &QHeaderView::sectionResized, [this]() { drawLabels(); });
    connect(&self, &QHeaderView::sectionMoved, [this]() { drawLabels(); });
    connect(self.horizontalScrollBar(), &QScrollBar::valueChanged, [this]() { drawLabels(); });
    connect(model, &MultiFilterProxyModel::filterValidityChanged, [this](int column, bool is_valid) {
      auto idx = static_cast<size_t>(column);
      if (!(line_edits.size() > idx)) { return; }
      line_edits[idx]->setStyleSheet(is_valid ? "" : INVALID_FILTER_STYLESHEET);
    });

    self.setSectionsClickable(true);
    self.setSectionsMovable(true);
  }

  void drawLabels()
  {
    const auto columns = static_cast<int>(line_edits.size());
    for (auto col = 0; col < columns; ++col)
    {
      const auto i = static_cast<size_t>(col);
      line_edits[i]->move(self.sectionPosition(col) - self.offset(), self.height() / 2);
      line_edits[i]->resize(self.sectionSize(col), self.height() / 2);
    }
  }
};

MultiFilterHeaderView::MultiFilterHeaderView(MultiFilterTableView* parent)
    : QHeaderView(Qt::Orientation::Horizontal, parent)
    , _self{ std::make_unique<Impl>(*this) }
{
}

MultiFilterHeaderView::~MultiFilterHeaderView() = default;

QSize MultiFilterHeaderView::sizeHint() const
{
  QSize size = QHeaderView::sizeHint();
  if (!_self->line_edits.empty())
  {
    size.setHeight(size.height() + _self->line_edits[0]->sizeHint().height());
  }
  return size;
}

void MultiFilterHeaderView::updateGeometries()
{
  if (!_self->line_edits.empty())
  {
    setViewportMargins(0, 0, 0, _self->line_edits[0]->sizeHint().height());
    QHeaderView::updateGeometries();
    _self->drawLabels();
  }
}
