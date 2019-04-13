#ifndef DISASSEMBLYHIGHLIGHTER_HXX
#define DISASSEMBLYHIGHLIGHTER_HXX

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <vector>

class Palette;
class DisassemblyHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
public:
  explicit DisassemblyHighlighter(Palette* palette, QTextDocument* parent = nullptr);
  bool Enabled{ true };

protected:
  void highlightBlock(const QString& text) override;

private:
  struct HighlightingRule {
    QRegularExpression Pattern;
    QTextCharFormat    Format;
  };

  std::vector<HighlightingRule> _rules;
};

#endif  // DISASSEMBLYHIGHLIGHTER_HXX
