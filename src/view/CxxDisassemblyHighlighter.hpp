#pragma once

#include <QColor>
#include <QSyntaxHighlighter>
#include <memory>

class QTextDocument;
class CxxDisassemblyHighlighter final : public QSyntaxHighlighter {
  Q_OBJECT
  Q_DISABLE_COPY(CxxDisassemblyHighlighter)
  Q_PROPERTY(QColor numberColor READ numberColor WRITE setNumberColor)
  Q_PROPERTY(QColor keywordColor READ keywordColor WRITE setKeywordColor)
  Q_PROPERTY(QColor stringColor READ stringColor WRITE setStringColor)
  Q_PROPERTY(QColor commentColor READ commentColor WRITE setCommentColor)
  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

public:
  explicit CxxDisassemblyHighlighter(QTextDocument* parent = nullptr);
  ~CxxDisassemblyHighlighter() override;

  const QColor& numberColor() const;
  void          setNumberColor(const QColor& number_color);
  const QColor& keywordColor() const;
  void          setKeywordColor(const QColor& keyword_color);
  const QColor& stringColor() const;
  void          setStringColor(const QColor& string_color);
  const QColor& commentColor() const;
  void          setCommentColor(const QColor& comment_color);
  bool          isEnabled() const;
  void          setEnabled(bool is_enabled);

protected:
  void highlightBlock(const QString& text) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
