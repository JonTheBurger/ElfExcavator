#include "MultiFilterTableView.hpp"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QScopedPointer>
#include <QTimer>
#include <vector>

#include "HexNumberDelegate.hpp"
#include "MultiFilterHeaderView.hpp"
#include "presenter/MultiFilterProxyModel.hpp"

struct MultiFilterTableView::Impl {
  Q_DISABLE_COPY(Impl)

  using MultiFilterHeaderViewPtr = QScopedPointer<MultiFilterHeaderView, QScopedPointerDeleteLater>;

  MultiFilterTableView&    self;
  MultiFilterHeaderViewPtr header;  ///< self takes ownership
  MultiFilterProxyModel    model;
  HexNumberDelegate        delegate;
  QTimer                   input_debounce_timer;
  std::vector<QString>     input_debounce_cache;

  explicit Impl(MultiFilterTableView& that) noexcept
      : self{ that }
      , header{ nullptr }
      , model{}
      , delegate{}
      , input_debounce_timer{}
      , input_debounce_cache{}
  {
  }
};

MultiFilterTableView::MultiFilterTableView(QWidget* parent)
    : QTableView(parent)
    , _self{ std::make_unique<Impl>(*this) }
{
  QTableView::setModel(&_self->model);  // [View does not take ownership of model unless it is the model's parent](https://doc.qt.io/qt-5/qabstractitemview.html#setModel)
  setItemDelegate(&_self->delegate);    // [View does not take ownership of delegate](https://doc.qt.io/qt-5/qabstractitemview.html#setItemDelegate)
  setSortingEnabled(true);
  _self->input_debounce_timer.setSingleShot(true);
}

MultiFilterTableView::~MultiFilterTableView() = default;

void MultiFilterTableView::resizeColumns()
{
  if (_self->header)
  {
    _self->header->resizeSections(QHeaderView::ResizeMode::Stretch);
    _self->header->resizeSections(QHeaderView::ResizeMode::Interactive);
    _self->header->updateGeometries();
  }
}

MultiFilterProxyModel* MultiFilterTableView::model() noexcept
{
  return &_self->model;
}

void MultiFilterTableView::setModel(QAbstractItemModel* model)
{
  _self->model.setSourceModel(model);

  using std::swap;
  Impl::MultiFilterHeaderViewPtr header{ new MultiFilterHeaderView(this) };
  swap(_self->header, header);

  setHorizontalHeader(_self->header.get());
  _self->header->setVisible(true);

  _self->input_debounce_cache.clear();
  _self->input_debounce_cache.resize(static_cast<uint8_t>(model->columnCount()));

  connect(_self->header.get(),
          &MultiFilterHeaderView::filterTextChanged,
          [this](int column, const QString& text) {
            const auto col = static_cast<size_t>(column);
            if (col < _self->input_debounce_cache.size())
            {
              _self->input_debounce_cache[col] = text;
              _self->input_debounce_timer.start(300);
            }
          });

  connect(&_self->input_debounce_timer,
          &QTimer::timeout,
          [this]() {
            int column = 0;
            for (QString& input : _self->input_debounce_cache)
            {
              if (!input.isEmpty())
              {
                _self->model.setColumnFilter(column, input);
                input.clear();
              }
              ++column;
            }
          });

  sortByColumn(0, Qt::AscendingOrder);
}

void MultiFilterTableView::keyPressEvent(QKeyEvent* event)
{
  // Handle Ctlr+C
  if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C))
  {
    QModelIndexList indices = selectionModel()->selectedIndexes();
    if (!indices.empty())
    {
      QString clipboard_text = "";

      auto index = indices.begin();
      for (int row = indices[0].row(); index != indices.end(); ++index)
      {
        if (index->row() != row)
        {
          // If the selection has spanned a row, replace trailing space with a newline
          clipboard_text.back() = '\n';
        }

        clipboard_text.append(index->data().toString());
        clipboard_text.append(' ');

        // Track row lazily so we know when selection has spanned a row
        row = index->row();
      }
      // We always append a space at the end of each cell, so remove the last space
      clipboard_text.chop(sizeof(' '));

      QApplication::clipboard()->setText(clipboard_text);
    }
  }
  else
  {
    QTableView::keyPressEvent(event);
  }
}

void MultiFilterTableView::showEvent(QShowEvent* event)
{
  resizeColumns();
  QTableView::showEvent(event);
}
