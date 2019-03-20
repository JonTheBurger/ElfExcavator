#ifndef TABLEVIEW_HXX
#define TABLEVIEW_HXX

#include <QTableView>

class TableView : public QTableView {
public:
  explicit TableView(QWidget* parent = nullptr);

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  bool _userHasResized{ false };
};

#endif  // TABLEVIEW_HXX
