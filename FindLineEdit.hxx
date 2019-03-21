#ifndef FINDLINEEDIT_HXX
#define FINDLINEEDIT_HXX

#include <QLineEdit>

class QTextBrowser;
class FindLineEdit : public QLineEdit {
public:
  explicit FindLineEdit(QWidget* parent = nullptr);
  void setTextBrowser(QTextBrowser* browser);

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  QTextBrowser* _browser;
};

#endif  // FINDLINEEDIT_HXX
