#include "HexNumberDelegate.hxx"

#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QStringBuilder>
#include <QStyle>

HexNumberDelegate::HexNumberDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void HexNumberDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  if (index.data(Qt::DisplayRole).userType() == QVariant::ULongLong)
  {
    if (option.state & QStyle::State_Selected)
    {
      painter->fillRect(option.rect, option.palette.highlight());
    }

    const QStyle* const style    = QApplication::style();
    QRect               textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &option);

    const int margin = (option.rect.height() - option.fontMetrics.height()) / 2;
    textRect.setTop(textRect.top() + margin);

    const auto value = index.data().value<qulonglong>();
    painter->drawText(textRect, " 0x" % QString("%1").arg(value, 0, 16).toUpper());
  }
  else
  {
    QStyledItemDelegate::paint(painter, option, index);
  }
}
