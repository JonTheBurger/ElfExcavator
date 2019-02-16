#ifndef SECTIONHEADER_HXX
#define SECTIONHEADER_HXX

#include <QFlags>
#include <QObject>
#include <QString>
#include <cstdint>
#include <vector>

class SectionHeader {
  Q_GADGET
public:
  enum class Flag : uint8_t {
    NONE = 0,

    UNKNOWN   = 1 << 0,
    CONTENTS  = 1 << 1,
    ALLOC     = 1 << 2,
    LOAD      = 1 << 3,
    READONLY  = 1 << 4,
    CODE      = 1 << 5,
    DATA      = 1 << 6,
    DEBUGGING = 1 << 7,
  };
  Q_DECLARE_FLAGS(Flags, Flag)
  Q_FLAG(Flag)

  operator QString() const;
  static std::vector<SectionHeader> ParseFromObjdump(const QString& output);

  QString Name{};
  size_t  Size{};
  size_t  Vma{};
  size_t  Lma{};
  size_t  FileOff{};
  Flags   Types{};
  uint8_t Align{};
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SectionHeader::Flags)

#endif  // SECTIONHEADER_HXX
