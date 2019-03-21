#include "TableView.hxx"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <algorithm>

#include "FilterHeaderView.hxx"
#include "HexNumberDelegate.hxx"
#include "MultiFilterProxyModel.hxx"

TableView::TableView(QWidget* parent)
    : QTableView(parent)
    , _header(nullptr)
    , _delegate(std::make_unique<HexNumberDelegate>())
    , _model(std::make_unique<MultiFilterProxyModel>())
{
  QTableView::setModel(_model.get());
  setSortingEnabled(true);

  setItemDelegate(_delegate.get());
}

TableView::~TableView() = default;

void TableView::setModel(QAbstractItemModel* model)
{
  _model->setSourceModel(model);

  auto newHeader = std::make_unique<FilterHeaderView>(this);
  std::swap(_header, newHeader);
  _header->setVisible(true);
  setHorizontalHeader(_header.get());
  connect(_header.get(), &FilterHeaderView::filterTextChanged, _model.get(), &MultiFilterProxyModel::setColumnFilter);

  sortByColumn(0, Qt::AscendingOrder);
}

void TableView::keyPressEvent(QKeyEvent* event)
{
  if ((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier))
  {
    QModelIndexList indices = selectionModel()->selectedIndexes();
    if (!indices.empty())
    {
      QString clipboardText = "";

      auto index = indices.begin();
      for (int row = indices[0].row(), col = indices[0].column(); index != indices.end(); ++index)
      {
        if (index->row() > row)
        {
          clipboardText.append('\n');
          clipboardText.append(index->data().toString());
        }
        else
        {
          clipboardText.append(index->data().toString());
          clipboardText.append(' ');
        }
        row = index->row();
      }

      QApplication::clipboard()->setText(clipboardText);
    }
  }
  else
  {
    QTableView::keyPressEvent(event);
  }
}

void TableView::resizeEvent(QResizeEvent* event)
{
  if (_header)
  {
    _header->setSectionResizeMode(QHeaderView::Stretch);
  }

  QTableView::resizeEvent(event);

  if (_header)
  {
    _header->setSectionResizeMode(QHeaderView::Interactive);
  }
}
