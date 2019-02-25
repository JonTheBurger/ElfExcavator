#ifndef FILTERHEADERVIEW_HXX
#define FILTERHEADERVIEW_HXX

#include <QHeaderView>
#include <QTableView>
#include <vector>

class QTableView;
class QAbstractItemModel;
class QLineEdit;
class FilterHeaderView : public QHeaderView {
  Q_OBJECT
public:
  explicit FilterHeaderView(QTableView* parent);
  ~FilterHeaderView();
  QSize sizeHint() const override;
  void  updateGeometries() override;

signals:
  void filterTextChanged(int column, const QString& text);

private:
  std::vector<QLineEdit*> _lineEdits;

  void drawLabels();
};

#endif  // FILTERHEADERVIEW_HXX
