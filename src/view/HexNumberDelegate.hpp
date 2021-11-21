#pragma once

#include <QStyledItemDelegate>

class HexNumberDelegate final : public QStyledItemDelegate {
  Q_OBJECT
  Q_DISABLE_COPY(HexNumberDelegate)

public:
  explicit HexNumberDelegate(QObject* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
