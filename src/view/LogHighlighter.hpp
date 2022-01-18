#pragma once

#include <QSyntaxHighlighter>
#include <memory>

class QTextDocument;
class LogHighlighter final : public QSyntaxHighlighter {
  Q_OBJECT
  Q_DISABLE_COPY(LogHighlighter)

public:
  explicit LogHighlighter(QTextDocument* parent = nullptr);
  ~LogHighlighter() override;

protected:
  void highlightBlock(const QString& text) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _self;
};
