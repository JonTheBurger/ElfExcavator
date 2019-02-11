#include "SectionHeaderParser.hxx"

#include <QRegExp>
#include <QStringList>

static QStringList::const_iterator SkipObjdumpHeader(const QStringList& lines)
{
  auto it = lines.begin();
  it += 6;
  return it;
}

std::vector<SectionHeader> SectionHeaderParser::ParseFromObjdump(const QString& output)
{
  std::vector<SectionHeader> headers;

  QStringList lines =
  output.split(QRegExp("\n|\r\n|\r"), QString::SkipEmptyParts);

  auto lineIter = SkipObjdumpHeader(lines);

  return headers;
}
