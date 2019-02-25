#ifndef HEXNUMBERDELEGATE_HXX
#define HEXNUMBERDELEGATE_HXX

#include <QStyledItemDelegate>

class HexNumberDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit HexNumberDelegate(QObject* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif  // HEXNUMBERDELEGATE_HXX
