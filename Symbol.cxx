#include "Symbol.hxx"

#include <QDebug>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QStringList>
#include <tuple>

#include "MaxStore.hxx"

Symbol::operator QString() const
{
  const auto metaEnum = QMetaEnum::fromType<Symbol::Flag>();
  return QString("%1 0x%2 0x%3 %4 %5")
    .arg(Name)
    .arg(Address)
    .arg(Size)
    .arg(SectionName)
    .arg(QString(metaEnum.valueToKeys(Types)));
}

Symbol::Flag Symbol::ParseFromObjdumpChar(const char in)
{
  Symbol::Flag out = Symbol::Flag::NORMAL;

  switch (in)
  {
    case 'l':
      out = Symbol::Flag::LOCAL;
      break;

    case 'g':
      out = Symbol::Flag::GLOBAL;
      break;

    case 'u':
      out = Symbol::Flag::UNIQUE_GLOBAL;
      break;

    case '!':
      out = Symbol::Flag::LOCAL_GLOBAL;
      break;

    case 'w':
      out = Symbol::Flag::WEAK;
      break;

    case 'C':
      out = Symbol::Flag::CONSTRUCTOR;
      break;

    case 'W':
      out = Symbol::Flag::WARNING;
      break;

    case 'I':
      out = Symbol::Flag::INDIRECT_REF;
      break;

    case 'i':
      out = Symbol::Flag::RELOC;
      break;

    case 'd':
      out = Symbol::Flag::DEBUG;
      break;

    case 'D':
      out = Symbol::Flag::DYNAMIC;
      break;

    case 'F':
      out = Symbol::Flag::FUNCTION;
      break;

    case 'f':
      out = Symbol::Flag::FILE;
      break;

    case 'O':
      out = Symbol::Flag::OBJECT;
      break;

    default:
      break;
  }

  return out;
}

static QStringList::const_iterator SkipObjdumpPreface(const QStringList& lines)
{
  static constexpr int OBJDUMP_PREFACE_LINE_COUNT = 2;
  return lines.cbegin() + OBJDUMP_PREFACE_LINE_COUNT;
}

static Symbol ParseObjdumpLine(const QString& str)
{
  /// Example:
  ///  000000000000d1c0 u     O .rodata	0000000000000008              QtPrivate::ConnectionTypes<QtPrivate::List<QString const&>, true>::types()::t
  static constexpr int RADIX              = 16;
  static constexpr int NUMBER_STR_LEN     = 16;
  static constexpr int SPACES_BEFORE_NAME = 14;

  Symbol symbol;
  bool   ok;

  auto leftSplitPoint  = str.indexOf(' ');
  auto rightSplitPoint = str.indexOf('\t');

  symbol.Address = str.leftRef(leftSplitPoint).toULongLong(&ok, RADIX);
  Q_ASSERT(ok);

  auto flagSectionStrings = str.midRef(leftSplitPoint, rightSplitPoint - leftSplitPoint).split(' ', QString::SplitBehavior::SkipEmptyParts);
  symbol.SectionName      = flagSectionStrings.last().toString();
  flagSectionStrings.pop_back();
  for (auto&& flagStr : flagSectionStrings)
  {
    // BUG: Contiguous flags not parsed
    symbol.Types |= Symbol::ParseFromObjdumpChar(flagStr[0].toLatin1());
  }

  const auto sizeNameString = str.rightRef(str.size() - rightSplitPoint - sizeof('\t'));
  symbol.Size               = sizeNameString.left(NUMBER_STR_LEN).toULongLong(&ok, RADIX);
  Q_ASSERT(ok);
  symbol.Name = sizeNameString.mid(NUMBER_STR_LEN + SPACES_BEFORE_NAME).toString();

  //  qDebug() << symbol;
  return symbol;
}

std::vector<Symbol> Symbol::ParseFromObjdump(const QString& output)
{
  std::vector<Symbol>                  symbolTable;
  MaxStore<std::tuple<size_t, size_t>> maxStore(25);

  QStringList lines = output.split(QRegularExpression(QLatin1String("\n|\r\n|\r")), QString::SkipEmptyParts);
  for (auto it = SkipObjdumpPreface(lines); it != lines.cend(); ++it)
  {
    symbolTable.push_back(ParseObjdumpLine(*it));
    symbolTable.back().Index = symbolTable.size() - 1;
    maxStore.push(std::make_tuple(symbolTable.back().Size, symbolTable.back().Index));
  }

  while (!maxStore.empty())
  {
    const auto index           = std::get<1>(maxStore.front());
    symbolTable[index].Display = true;
    maxStore.pop();
  }

  return symbolTable;
}
