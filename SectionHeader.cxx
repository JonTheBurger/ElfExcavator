#include "SectionHeader.hxx"

#include <QDebug>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QStringList>

static constexpr int RADIX = 16;

SectionHeader::operator QString() const
{
  const auto metaEnum = QMetaEnum::fromType<SectionHeader::Flag>();
  return QString("%1 0x%2 0x%3 0x%4 0x%5 2**%6 %7")
    .arg(Name)
    .arg(Size, 0, RADIX)
    .arg(Vma, 0, RADIX)
    .arg(Lma, 0, RADIX)
    .arg(FileOff, 0, RADIX)
    .arg(Align)
    .arg(QString(metaEnum.valueToKeys(Types)));
}

static QStringList::const_iterator SkipObjdumpPreface(const QStringList& lines)
{
  static constexpr int OBJDUMP_PREFACE_LINE_COUNT = 3;
  return lines.begin() + OBJDUMP_PREFACE_LINE_COUNT;
}

static SectionHeader ParseObjdumpLine(const QString& str)
{
  /// Example:
  /// Idx Name               Size      VMA               LMA               File off  Algn  Flags
  ///   0 .interp            0000001c  0000000000000238  0000000000000238  00000238  2**0  CONTENTS, ALLOC, LOAD, READONLY, DATA
  SectionHeader header;
  const auto    metaEnum = QMetaEnum::fromType<SectionHeader::Flag>();
  bool          ok;

  const int bisectPoint = str.indexOf(' ', str.indexOf(QLatin1String("2**")));

  auto headerStrings = str.leftRef(bisectPoint).split(' ', QString::SplitBehavior::SkipEmptyParts);
  header.Name        = headerStrings[1].toString();
  header.Size        = headerStrings[2].toULongLong(&ok, RADIX);
  Q_ASSERT(ok);
  header.Vma = headerStrings[3].toULongLong(&ok, RADIX);
  Q_ASSERT(ok);
  header.Lma = headerStrings[4].toULongLong(&ok, RADIX);
  Q_ASSERT(ok);
  header.FileOff = headerStrings[5].toULongLong(&ok, RADIX);
  Q_ASSERT(ok);
  header.Align = headerStrings[6].mid(sizeof("2**") - sizeof(' ')).toInt(&ok);
  Q_ASSERT(ok);

  /// The bisect point is always followed by 2 spaces, e.g. "2**0  CONTENTS"
  auto flagStrings = str.rightRef(str.size() - bisectPoint - 2 * sizeof(' ')).split(QLatin1String(", "), QString::SplitBehavior::SkipEmptyParts);
  for (auto&& flagString : flagStrings)
  {
    auto flag = static_cast<SectionHeader::Flag>(metaEnum.keyToValue(flagString.toLatin1(), &ok));
    if (!ok)
    {
      flag = SectionHeader::Flag::UNKNOWN;
    }
    header.Types |= flag;
  }

  //  qDebug() << header;
  return header;
}

std::vector<SectionHeader> SectionHeader::ParseFromObjdump(const QString& output)
{
  std::vector<SectionHeader> headers;

  QStringList lines = output.split(QRegularExpression(QLatin1String("\n|\r\n|\r")), QString::SplitBehavior::SkipEmptyParts);
  for (auto it = SkipObjdumpPreface(lines); it != lines.cend(); ++it)
  {
    headers.push_back(ParseObjdumpLine(*it));
  }

  return headers;
}
