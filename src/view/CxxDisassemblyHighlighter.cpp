#include "CxxDisassemblyHighlighter.hpp"

#include <QRegularExpression>
#include <QStringBuilder>
#include <QStringList>
#include <array>

struct HighlightingRule {
  QRegularExpression pattern;
  QTextCharFormat    format;
};

enum SyntacticElement {
  SyntacticElement_NUMBER,
  SyntacticElement_KEYWORD,
  SyntacticElement_STRING,
  SyntacticElement_COMMENT,

  SyntacticElement_ARRAY_SIZE_,
};

static const QColor DefaultSyntaxColor[SyntacticElement_ARRAY_SIZE_]{
  QColor::fromRgb(0xB5CEA8),  // Number
  QColor::fromRgb(0x569CD6),  // Keyword
  QColor::fromRgb(0xCE9178),  // String
  QColor::fromRgb(0x7CA668),  // Comment
};

static const QStringList Keywords = {
  QStringLiteral("alignas"),
  QStringLiteral("alignof"),
  QStringLiteral("and"),
  QStringLiteral("and_eq"),
  QStringLiteral("asm"),
  QStringLiteral("atomic_cancel"),
  QStringLiteral("atomic_commit"),
  QStringLiteral("atomic_noexcept"),
  QStringLiteral("auto"),
  QStringLiteral("bitand"),
  QStringLiteral("bitor"),
  QStringLiteral("bool"),
  QStringLiteral("break"),
  QStringLiteral("case"),
  QStringLiteral("catch"),
  QStringLiteral("char"),
  QStringLiteral("char8_t"),
  QStringLiteral("char16_t"),
  QStringLiteral("char32_t"),
  QStringLiteral("class"),
  QStringLiteral("compl"),
  QStringLiteral("concept"),
  QStringLiteral("const"),
  QStringLiteral("consteval"),
  QStringLiteral("constexpr"),
  QStringLiteral("constinit"),
  QStringLiteral("const_cast"),
  QStringLiteral("continue"),
  QStringLiteral("co_await"),
  QStringLiteral("co_return"),
  QStringLiteral("co_yield"),
  QStringLiteral("decltype"),
  QStringLiteral("default"),
  QStringLiteral("delete"),
  QStringLiteral("do"),
  QStringLiteral("double"),
  QStringLiteral("dynamic_cast"),
  QStringLiteral("else"),
  QStringLiteral("enum"),
  QStringLiteral("explicit"),
  QStringLiteral("export"),
  QStringLiteral("extern"),
  QStringLiteral("false"),
  QStringLiteral("float"),
  QStringLiteral("for"),
  QStringLiteral("friend"),
  QStringLiteral("goto"),
  QStringLiteral("if"),
  QStringLiteral("inline"),
  QStringLiteral("int"),
  QStringLiteral("long"),
  QStringLiteral("mutable"),
  QStringLiteral("namespace"),
  QStringLiteral("new"),
  QStringLiteral("noexcept"),
  QStringLiteral("not"),
  QStringLiteral("not_eq"),
  QStringLiteral("nullptr"),
  QStringLiteral("operator"),
  QStringLiteral("or"),
  QStringLiteral("or_eq"),
  QStringLiteral("private"),
  QStringLiteral("protected"),
  QStringLiteral("public"),
  QStringLiteral("reflexpr"),
  QStringLiteral("register"),
  QStringLiteral("reinterpret_cast"),
  QStringLiteral("requires"),
  QStringLiteral("return"),
  QStringLiteral("short"),
  QStringLiteral("signed"),
  QStringLiteral("sizeof"),
  QStringLiteral("static"),
  QStringLiteral("static_assert"),
  QStringLiteral("static_cast"),
  QStringLiteral("struct"),
  QStringLiteral("switch"),
  QStringLiteral("synchronized"),
  QStringLiteral("template"),
  QStringLiteral("this"),
  QStringLiteral("thread_local"),
  QStringLiteral("throw"),
  QStringLiteral("true"),
  QStringLiteral("try"),
  QStringLiteral("typedef"),
  QStringLiteral("typeid"),
  QStringLiteral("typename"),
  QStringLiteral("union"),
  QStringLiteral("unsigned"),
  QStringLiteral("using"),
  QStringLiteral("virtual"),
  QStringLiteral("void"),
  QStringLiteral("volatile"),
  QStringLiteral("wchar_t"),
  QStringLiteral("while"),
  QStringLiteral("xor"),
  QStringLiteral("xor_eq"),
};

struct CxxDisassemblyHighlighter::Impl {
  Q_DISABLE_COPY(Impl)

  explicit Impl(CxxDisassemblyHighlighter& that) noexcept
      : self{ that }
      , rules{ {
          // Number
          {
            QRegularExpression{ QStringLiteral("\\b(0x|0X|0b|0B)?([A-F]|[a-f]|[0-9])+\\b") },
            {} },
          // Keyword
          { QRegularExpression{},
            {} },
          // String
          { QRegularExpression{ QStringLiteral("\".*\"") },
            {} },
          // Comment
          { QRegularExpression{ QStringLiteral("(\\/\\/.*$)|(\\/\\*((.|\\n|\\r)*)\\*\\/)") },
            {} },
        } }
      , is_enabled{ true }
  {
    // Keywords
    rules[SyntacticElement_KEYWORD].pattern = QRegularExpression{ "\\b(" % Keywords.join('|') % ")\\b" };

    // Colors
    for (size_t i = 0; i < SyntacticElement_ARRAY_SIZE_; ++i)
    {
      rules[i].format.setForeground(DefaultSyntaxColor[i]);
    }
  }

  CxxDisassemblyHighlighter&                                 self;
  std::array<HighlightingRule, SyntacticElement_ARRAY_SIZE_> rules;
  bool                                                       is_enabled;
};

CxxDisassemblyHighlighter::CxxDisassemblyHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , _self{ std::make_unique<Impl>(*this) }
{
}

CxxDisassemblyHighlighter::~CxxDisassemblyHighlighter() = default;

const QColor& CxxDisassemblyHighlighter::numberColor() const
{
  return _self->rules[SyntacticElement_NUMBER].format.foreground().color();
}

void CxxDisassemblyHighlighter::setNumberColor(const QColor& number_color)
{
  _self->rules[SyntacticElement_NUMBER].format.setForeground(number_color);
}

const QColor& CxxDisassemblyHighlighter::keywordColor() const
{
  return _self->rules[SyntacticElement_KEYWORD].format.foreground().color();
}

void CxxDisassemblyHighlighter::setKeywordColor(const QColor& keyword_color)
{
  _self->rules[SyntacticElement_KEYWORD].format.setForeground(keyword_color);
}

const QColor& CxxDisassemblyHighlighter::stringColor() const
{
  return _self->rules[SyntacticElement_STRING].format.foreground().color();
}

void CxxDisassemblyHighlighter::setStringColor(const QColor& string_color)
{
  _self->rules[SyntacticElement_STRING].format.setForeground(string_color);
}

const QColor& CxxDisassemblyHighlighter::commentColor() const
{
  return _self->rules[SyntacticElement_COMMENT].format.foreground().color();
}

void CxxDisassemblyHighlighter::setCommentColor(const QColor& comment_color)
{
  _self->rules[SyntacticElement_COMMENT].format.setForeground(comment_color);
}

bool CxxDisassemblyHighlighter::isEnabled() const
{
  return _self->is_enabled;
}

void CxxDisassemblyHighlighter::setEnabled(bool is_enabled)
{
  _self->is_enabled = is_enabled;
}

void CxxDisassemblyHighlighter::highlightBlock(const QString& text)
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
