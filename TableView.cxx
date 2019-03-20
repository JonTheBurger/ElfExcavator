#include "TableView.hxx"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

#include "FilterHeaderView.hxx"
#include "MultiFilterProxyModel.hxx"

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{
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
