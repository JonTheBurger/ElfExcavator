#ifndef TABLEVIEW_HXX
#define TABLEVIEW_HXX

#include <QTableView>
#include <memory>

class FilterHeaderView;
class HexNumberDelegate;
class MultiFilterProxyModel;
class TableView : public QTableView {
public:
  explicit TableView(QWidget* parent = nullptr);
  ~TableView();
  void ResizeColumns();
  void setModel(QAbstractItemModel* model) override;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  std::unique_ptr<FilterHeaderView>      _header;
  std::unique_ptr<HexNumberDelegate>     _delegate;
  std::unique_ptr<MultiFilterProxyModel> _model;
};

#endif  // TABLEVIEW_HXX
