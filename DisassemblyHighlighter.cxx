#include "DisassemblyHighlighter.hxx"

#include <QRegularExpression>
#include <QString>
#include <QTextDocument>
#include <vector>

static struct HighlighterFormat {
  HighlighterFormat()
  {
    Number.setForeground(QColor(0xFD971F));
    Keyword.setForeground(QColor(0x0484FF));
    String.setForeground(QColor(0x1EAA1E));
    Type.setFontWeight(QFont::Bold);
    Function.setForeground(QColor(0xAE81FF));
    Comment.setForeground(QColor(0x808080));
    Braces.setForeground(QColor(0x4EF278));
    Instruction.setForeground(QColor(0xCC0000));
    Dollar.setForeground(QColor(0xFFC66D));
    Percent.setForeground(QColor(0x66D9EF));
    Hash.setForeground(QColor(0xEE72BA));
  }

  QTextCharFormat Number;
  QTextCharFormat Keyword;
  QTextCharFormat String;
  QTextCharFormat Type;
  QTextCharFormat Function;
  QTextCharFormat Comment;
  QTextCharFormat Braces;
  QTextCharFormat Instruction;
  QTextCharFormat Dollar;
  QTextCharFormat Percent;
  QTextCharFormat Hash;
} Formats;

static struct HighlighterPatterns {
  const std::vector<QString> Number = { QLatin1String("\\b(0x|0X)?([A-F]|[a-f]|[0-9])+\\b") };
  // clang-format off
  const std::vector<QString> Keyword = { QLatin1String("\\balignas\\b"), QLatin1String("\\balignof\\b"), QLatin1String("\\band\\b"), QLatin1String("\\band_eq\\b"),
    QLatin1String("\\basm\\b"), QLatin1String("\\batomic_cancel\\b"), QLatin1String("\\batomic_commit\\b"), QLatin1String("\\batomic_noexcept\\b"),
    QLatin1String("\\baudit\\b"), QLatin1String("\\bauto\\b"), QLatin1String("\\baxiom\\b"), QLatin1String("\\bbitand\\b"), QLatin1String("\\bbitor\\b"),
    QLatin1String("\\bbool\\b"), QLatin1String("\\bbreak\\b"), QLatin1String("\\bcase\\b"), QLatin1String("\\bcatch\\b"), QLatin1String("\\bchar\\b"),
    QLatin1String("\\bchar16_t\\b"), QLatin1String("\\bchar32_t\\b"), QLatin1String("\\bchar8_t\\b"), QLatin1String("\\bclass\\b"), QLatin1String("\\bco_await\\b"),
    QLatin1String("\\bco_return\\b"), QLatin1String("\\bco_yield\\b"), QLatin1String("\\bcompl\\b"), QLatin1String("\\bconcept\\b"), QLatin1String("\\bconst\\b"),
    QLatin1String("\\bconst_cast\\b"), QLatin1String("\\bconsteval\\b"), QLatin1String("\\bconstexpr\\b"), QLatin1String("\\bcontinue\\b"), QLatin1String("\\bdecltype\\b"),
    QLatin1String("\\bdefault\\b"), QLatin1String("\\bdelete\\b"), QLatin1String("\\bdo\\b"), QLatin1String("\\bdouble\\b"), QLatin1String("\\bdynamic_cast\\b"),
    QLatin1String("\\belse\\b"), QLatin1String("\\benum\\b"), QLatin1String("\\bexplicit\\b"), QLatin1String("\\bexport\\b"), QLatin1String("\\bextern\\b"),
    QLatin1String("\\bfalse\\b"), QLatin1String("\\bfinal\\b"), QLatin1String("\\bfloat\\b"), QLatin1String("\\bfor\\b"), QLatin1String("\\bfriend\\b"),
    QLatin1String("\\bgoto\\b"), QLatin1String("\\bif\\b"), QLatin1String("\\bimport\\b"), QLatin1String("\\binline\\b"), QLatin1String("\\bint\\b"),
    QLatin1String("\\blong\\b"), QLatin1String("\\bmodule\\b"), QLatin1String("\\bmutable\\b"), QLatin1String("\\bnamespace\\b"), QLatin1String("\\bnew\\b"),
    QLatin1String("\\bnoexcept\\b"), QLatin1String("\\bnot\\b"), QLatin1String("\\bnot_eq\\b"), QLatin1String("\\bnullptr\\b"), QLatin1String("\\boperator\\b"),
    QLatin1String("\\bor\\b"), QLatin1String("\\bor_eq\\b"), QLatin1String("\\boverride\\b"), QLatin1String("\\bprivate\\b"), QLatin1String("\\bprotected\\b"),
    QLatin1String("\\bpublic\\b"), QLatin1String("\\breflexpr\\b"), QLatin1String("\\bregister\\b"), QLatin1String("\\breinterpret_cast\\b"), QLatin1String("\\brequires\\b"),
    QLatin1String("\\breturn\\b"), QLatin1String("\\bshort\\b"), QLatin1String("\\bsigned\\b"), QLatin1String("\\bsizeof\\b"), QLatin1String("\\bstatic\\b"),
    QLatin1String("\\bstatic_assert\\b"), QLatin1String("\\bstatic_cast\\b"), QLatin1String("\\bstruct\\b"), QLatin1String("\\bswitch\\b"), QLatin1String("\\bsynchronized\\b"),
    QLatin1String("\\btemplate\\b"), QLatin1String("\\bthis\\b"), QLatin1String("\\bthread_local\\b"), QLatin1String("\\bthrow\\b"), QLatin1String("\\btransaction_safe\\b"),
    QLatin1String("\\btransaction_safe_dynamic\\b"), QLatin1String("\\btrue\\b"), QLatin1String("\\btry\\b"), QLatin1String("\\btypedef\\b"), QLatin1String("\\btypeid\\b"),
    QLatin1String("\\btypename\\b"), QLatin1String("\\bunion\\b"), QLatin1String("\\bunsigned\\b"), QLatin1String("\\busing\\b"), QLatin1String("\\bvirtual\\b"),
    QLatin1String("\\bvoid\\b"), QLatin1String("\\bvolatile\\b"), QLatin1String("\\bwchar_t\\b"), QLatin1String("\\bwhile\\b"), QLatin1String("\\bxor\\b"), QLatin1String("\\bxor_eq\\b"),
  };
  // clang-format on
  const std::vector<QString> String      = { QLatin1String("\".*\"") };
  const std::vector<QString> Type        = { QLatin1String("\\w+::") };
  const std::vector<QString> Function    = { QLatin1String("::([A-z]|[0-9]|_)+\\(") };
  const std::vector<QString> Comment     = { QLatin1String("\\/\\/.*$"), QLatin1String("\\/\\*((.|\\n|\\r)*)\\*\\/") };
  const std::vector<QString> Braces      = { QLatin1String("\\(|\\)|\\[|\\]|<|>|{|}") };
  const std::vector<QString> Instruction = { QLatin1String(" \\t.+?\\b") };
  const std::vector<QString> Dollar      = { QLatin1String("\\$([A-z]|[0-9])+") };
  const std::vector<QString> Percent     = { QLatin1String("%([A-z]|[0-9])+") };
  const std::vector<QString> Hash        = { QLatin1String("#([A-z]|[0-9])+") };
} Patterns;

struct HighlightingRule {
  HighlightingRule(const QTextCharFormat& format, const std::vector<QString>& patterns)
      : Format(format)
      , Patterns(patterns)
  {
  }

  const QTextCharFormat&      Format;
  const std::vector<QString>& Patterns;
} Rules[] = {
  // If there are multiple matches, the last rule wins
  { Formats.Number, Patterns.Number },
  { Formats.Keyword, Patterns.Keyword },
  { Formats.String, Patterns.String },
  { Formats.Type, Patterns.Type },
  { Formats.Function, Patterns.Function },
  { Formats.Comment, Patterns.Comment },
  { Formats.Braces, Patterns.Braces },
  { Formats.Instruction, Patterns.Instruction },
  { Formats.Dollar, Patterns.Dollar },
  { Formats.Percent, Patterns.Percent },
  { Formats.Hash, Patterns.Hash },
};

DisassemblyHighlighter::DisassemblyHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
}

void DisassemblyHighlighter::highlightBlock(const QString& text)
{
  if (!Enabled) { return; }
  for (auto&& rule : Rules)
  {
    for (auto&& str : rule.Patterns)
    {
      const QRegularExpression        expression(str);
      QRegularExpressionMatchIterator it = expression.globalMatch(text);

      while (it.hasNext())
      {
        auto match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), rule.Format);
      }
    }
  }
}
