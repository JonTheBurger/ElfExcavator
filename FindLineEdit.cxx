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
  const QString query = text();

  if (_browser && !query.isEmpty())
  {
    if ((event->key() == Qt::Key_F3) ||
        (event->key() == Qt::Key_Return) ||
        (event->key() == Qt::Key_Enter))
    {
      const bool                 searchBackwards = event->modifiers() == Qt::ShiftModifier;
      QTextDocument::FindFlag    findDirection   = (searchBackwards ? QTextDocument::FindBackward : QTextDocument::FindFlag{});
      QTextCursor::MoveOperation wrapLocation    = (searchBackwards ? QTextCursor::End : QTextCursor::Start);

      const bool found = _browser->find(query, findDirection);
      if (!found)
      {
        _browser->moveCursor(wrapLocation);
        _browser->find(query, findDirection);
      }
    }
  }

  QLineEdit::keyPressEvent(event);
}
