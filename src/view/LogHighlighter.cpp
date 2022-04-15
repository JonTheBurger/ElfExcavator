#include "LogHighlighter.hpp"

#include <QRegularExpression>
#include <vector>

struct HighlightingRule {
  QRegularExpression pattern;
  QTextCharFormat    format;
};

struct LogHighlighter::Impl {
  Q_DISABLE_COPY(Impl)

  explicit Impl(LogHighlighter& that) noexcept
      : self{ that }
      , rules{}
      , is_enabled{ true }
  {
    HighlightingRule& timestamp_rule = rules.emplace_back();
    timestamp_rule.pattern.setPattern(QStringLiteral(R"(^\[.*\])"));
    timestamp_rule.format.setForeground(QColor::fromRgb(0x6A0DAD));  // purple

    HighlightingRule& info_rule = rules.emplace_back();
    info_rule.pattern.setPattern(QStringLiteral(R"(\[info\])"));
    info_rule.format.setForeground(Qt::blue);

    HighlightingRule& warn_rule = rules.emplace_back();
    warn_rule.pattern.setPattern(QStringLiteral(R"(\[warn.*\])"));
    warn_rule.format.setForeground(Qt::yellow);

    HighlightingRule& error_rule = rules.emplace_back();
    error_rule.pattern.setPattern(QStringLiteral(R"(\[error\])"));
    error_rule.format.setForeground(Qt::red);

    HighlightingRule& critical_rule = rules.emplace_back();
    critical_rule.pattern.setPattern(QStringLiteral(R"(\[error\])"));
    critical_rule.format.setForeground(QColor::fromRgb(0xFFA500));  // orange

    HighlightingRule& start_rule = rules.emplace_back();
    start_rule.pattern.setPattern(QStringLiteral(R"(\[start\])"));
    start_rule.format.setForeground(Qt::green);

    HighlightingRule& end_rule = rules.emplace_back();
    end_rule.pattern.setPattern(QStringLiteral(R"(\[end\])"));
    end_rule.format.setForeground(QColor::fromRgb(0xFFA500));  // orange
  }

  LogHighlighter&               self;
  std::vector<HighlightingRule> rules;
  bool                          is_enabled;
};

LogHighlighter::LogHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , _self{ std::make_unique<Impl>(*this) }
{
}

LogHighlighter::~LogHighlighter() = default;

void LogHighlighter::highlightBlock(const QString& text)
{
  if (!_self->is_enabled) { return; }
  for (auto&& rule : _self->rules)
  {
    QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);

    while (it.hasNext())
    {
      auto match = it.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
}
