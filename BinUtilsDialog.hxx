#ifndef BINUTILSDIALOG_HXX
#define BINUTILSDIALOG_HXX

#include <QDialog>

class BinUtils;
namespace Ui {
class BinUtilsDialog;
}

class BinUtilsDialog : public QDialog {
  Q_OBJECT
  Q_DISABLE_COPY(BinUtilsDialog)

public:
  explicit BinUtilsDialog(BinUtils* binUtils, QWidget* parent = nullptr);
  ~BinUtilsDialog() final;

protected:
  void changeEvent(QEvent* e) final;

private:
  Ui::BinUtilsDialog* _ui;
  BinUtils*           _binUtils;
};

#endif  // BINUTILSDIALOG_HXX
