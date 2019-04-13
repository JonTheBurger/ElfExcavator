#include "DisassemblyHighlighter.hxx"

#include <QRegularExpression>
#include <QString>
#include <QTextDocument>
#include <vector>

#include "CircularIterator.hxx"
#include "Palette.hxx"

DisassemblyHighlighter::DisassemblyHighlighter(Palette* palette, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , _rules{
      { QRegularExpression{ QLatin1String("\\b(0x|0X)?([A-F]|[a-f]|[0-9])+\\b") }, {} },  // number
      { QRegularExpression{ QLatin1String(
          "\\b(alignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|audit|auto|axiom|"
          "bitand|bitor|bool|break|case|catch|char|char16_t|char32_t|char8_t|class|co_await|co_return|"
          "co_yield|compl|concept|const|const_cast|consteval|constexpr|continue|decltype|default|delete|"
          "do|double|dynamic_cast|else|enum|explicit|export|extern|false|final|float|for|friend|goto|if|"
          "import|inline|int|long|module|mutable|namespace|new|noexcept|not|not_eq|nullptr|operator|or|"
          "or_eq|override|private|protected|public|reflexpr|register|reinterpret_cast|requires|return|"
          "short|signed|sizeof|static|static_assert|static_cast|struct|switch|synchronized|template|this|"
          "thread_local|throw|transaction_safe|transaction_safe_dynamic|true|try|typedef|typeid|typename|"
          "union|unsigned|using|virtual|void|volatile|wchar_t|while|xor|xor_eq)\\b") },
        {} },                                                                                   // keyword
      { QRegularExpression{ QLatin1String("\\(|\\)|\\[|\\]|<|>|{|}") }, {} },                   // braces
      { QRegularExpression{ QLatin1String(" \\t.+?\\b") }, {} },                                // instruction
      { QRegularExpression{ QLatin1String("\\$([A-z]|[0-9])+") }, {} },                         // dollar
      { QRegularExpression{ QLatin1String("%([A-z]|[0-9])+") }, {} },                           // percent
      { QRegularExpression{ QLatin1String("#([A-z]|[0-9])+") }, {} },                           // hash
      { QRegularExpression{ QLatin1String("(\\/\\/.*$)|(\\/\\*((.|\\n|\\r)*)\\*\\/)") }, {} },  // comment
      { QRegularExpression{ QLatin1String("\".*\"") }, {} },                                    // string
    }
{
  circular_iterator<decltype(Palette::COLORS.begin())> coloriter(Palette::COLORS.begin(), Palette::COLORS.end());
  for (auto& rule : _rules)
  {
    rule.Format.setForeground(**coloriter++);
  }
}

void DisassemblyHighlighter::highlightBlock(const QString& text)
{
  if (!Enabled) { return; }
  for (auto&& rule : _rules)
  {
    QRegularExpressionMatchIterator it = rule.Pattern.globalMatch(text);

    while (it.hasNext())
    {
      auto match = it.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.Format);
    }
  }
}
