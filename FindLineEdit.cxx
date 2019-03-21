#include "FindLineEdit.hxx"

#include <QKeyEvent>
#include <QTextBrowser>

FindLineEdit::FindLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , _browser(nullptr)
{
  connect(this, &QLineEdit::textChanged, [this](const QString& query) {
    if (_browser && !query.isEmpty())
    {
      QList<QTextEdit::ExtraSelection> extraSelections;

      _browser->moveCursor(QTextCursor::Start);
      QColor color = QColor(Qt::gray).lighter(130);

      while (_browser->find(query))
      {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = _browser->textCursor();
        extraSelections.append(extra);
      }

      _browser->setExtraSelections(extraSelections);
    }
  });
}

void FindLineEdit::setTextBrowser(QTextBrowser* browser)
{
  _browser = browser;
}

void FindLineEdit::keyPressEvent(QKeyEvent* event)
{
  if (_browser)
  {
    if ((event->key() == Qt::Key_F3) || (event->key() == Qt::Key_Return))
    {
      if (event->modifiers() == Qt::ShiftModifier)
      {
      }
      else
      {
        const QString query = text();
        if (!query.isEmpty())
        {
          const bool found = _browser->find(query);
          if (!found)
          {
            _browser->moveCursor(QTextCursor::Start);
            _browser->find(query);
          }
        }
      }
    }
  }
  QLineEdit::keyPressEvent(event);
}
