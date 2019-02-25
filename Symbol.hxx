#ifndef SYMBOL_HXX
#define SYMBOL_HXX

#include <QObject>
#include <QString>

class Symbol {
  Q_GADGET
public:
  enum class Flag : uint16_t {
    NORMAL = 0,

    LOCAL         = 1 << 0,
    GLOBAL        = 1 << 1,
    UNIQUE_GLOBAL = 1 << 2,
    LOCAL_GLOBAL  = 1 << 3,
    WEAK          = 1 << 4,
    CONSTRUCTOR   = 1 << 5,
    WARNING       = 1 << 6,
    INDIRECT_REF  = 1 << 7,
    RELOC         = 1 << 8,
    DEBUG         = 1 << 9,
    DYNAMIC       = 1 << 10,
    FUNCTION      = 1 << 11,
    FILE          = 1 << 12,
    OBJECT        = 1 << 13,
  };
  Q_DECLARE_FLAGS(Flags, Flag)
  Q_FLAG(Flag)

  operator QString() const;
  static Flag                ParseFromObjdumpChar(char in);
  static std::vector<Symbol> ParseFromObjdump(const QString& output);

  QString  Name{};
  QString  SectionName{};
  size_t   Address{};  /// Or Value
  size_t   Size{};     /// Or Alignment
  uint32_t Index{};
  Flags    Types{};
  bool     Display{};
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Symbol::Flags)

#endif  // SYMBOL_HXX
