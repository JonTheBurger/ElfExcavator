#ifndef DISASSEMBLYHIGHLIGHTER_HXX
#define DISASSEMBLYHIGHLIGHTER_HXX

#include <QSyntaxHighlighter>

class DisassemblyHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
public:
  explicit DisassemblyHighlighter(QTextDocument* parent = nullptr);
  bool Enabled{ true };

protected:
  void highlightBlock(const QString& text) override;
};

#endif  // DISASSEMBLYHIGHLIGHTER_HXX
