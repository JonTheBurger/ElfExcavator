#ifndef SECTIONHEADERPARSER_HXX
#define SECTIONHEADERPARSER_HXX

#include <QString>
#include <cstdint>
#include <vector>

struct SectionHeader {
  enum Flags {
    NONE = 0,

    UNKNOWN  = 1 << 0,
    CONTENTS = 1 << 1,
    ALLOC    = 1 << 2,
    LOAD     = 1 << 3,
    READONLY = 1 << 4,
    CODE     = 1 << 5,
    DATA     = 1 << 6,
  };

  QString        Name{};
  QString        Type{};
  std::uintptr_t Address{};
  std::uintptr_t Offset{};
  std::uintptr_t Size{};
  std::uintptr_t EntSize{};
  Flags          Flags{};
  int            Link{};
  int            Info{};
  int            Alignment{};
};

class SectionHeaderParser {
public:
  static std::vector<SectionHeader> ParseFromObjdump(const QString& output);
};

#endif  // SECTIONHEADERPARSER_HXX
