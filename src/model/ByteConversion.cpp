#include "ByteConversion.hpp"

#include <type_traits>

static constexpr int RADIX = 10;

template <typename T>
static QString toString(QByteArray bytes, bool byte_swap)
{
  QString repr{};
  bytes.truncate(sizeof(T));

  if (static_cast<size_t>(bytes.size()) == sizeof(T))
  {
    if (byte_swap)
    {
      std::reverse(bytes.begin(), bytes.end());
    }
    T number;
    static_assert(std::is_trivially_copyable_v<T>);
    memcpy(&number, bytes.data(), sizeof(number));

    if constexpr (std::is_floating_point_v<T>)
    {
      repr = QString::number(static_cast<double>(number));
    }
    else
    {
      repr = QString::number(number, RADIX);
    }
  }

  return repr;
}

template <typename T>
static QByteArray toBytes(const QString& repr, bool byte_swap)
{
  static_assert(std::is_trivially_copyable_v<T>);

  QByteArray bytes;

  if constexpr (std::is_floating_point_v<T>)
  {
    double x = repr.toDouble();
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }
  else if constexpr (std::is_signed_v<T>)
  {
    T x = static_cast<T>(repr.toLongLong(nullptr, RADIX));
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }
  else
  {
    T x = static_cast<T>(repr.toULongLong(nullptr, RADIX));
    bytes.append(reinterpret_cast<char*>(&x), sizeof(x));
  }

  if (byte_swap)
  {
    std::reverse(bytes.begin(), bytes.end());
  }

  return bytes;
}

QString nameof(DataType type) noexcept
{
  switch (type)
  {
    case DataType::U8:
      return QStringLiteral("uint8_t");
    case DataType::I8:
      return QStringLiteral("uint8_t");
    case DataType::U16:
      return QStringLiteral("uint16_t");
    case DataType::I16:
      return QStringLiteral("int16_t");
    case DataType::U32:
      return QStringLiteral("uint32_t");
    case DataType::I32:
      return QStringLiteral("int32_t");
    case DataType::U64:
      return QStringLiteral("uint64_t");
    case DataType::I64:
      return QStringLiteral("int64_t");
    case DataType::FLOAT:
      return QStringLiteral("float");
    case DataType::DOUBLE:
      return QStringLiteral("double");
    case DataType::BYTES:
      return QStringLiteral("bytes");
    case DataType::UTF8:
      return QStringLiteral("utf-8");
    default:
      break;
  }
  return QString();
}

QString toString(QByteArray bytes, DataType type, bool byte_swap)
{
  if (type == DataType::BYTES)
  {
    return bytes.toHex();
  }
  else if (type == DataType::UTF8)
  {
    return QString::fromUtf8(bytes.data(), bytes.size());
  }

  switch (type)
  {
    case DataType::U8:
      return toString<uint8_t>(std::move(bytes), byte_swap);
    case DataType::I8:
      return toString<int8_t>(std::move(bytes), byte_swap);
    case DataType::U16:
      return toString<uint16_t>(std::move(bytes), byte_swap);
    case DataType::I16:
      return toString<int16_t>(std::move(bytes), byte_swap);
    case DataType::U32:
      return toString<uint32_t>(std::move(bytes), byte_swap);
    case DataType::I32:
      return toString<int32_t>(std::move(bytes), byte_swap);
    case DataType::U64:
      return toString<uint64_t>(std::move(bytes), byte_swap);
    case DataType::I64:
      return toString<int64_t>(std::move(bytes), byte_swap);
    case DataType::FLOAT:
      return toString<float>(std::move(bytes), byte_swap);
    case DataType::DOUBLE:
      return toString<double>(std::move(bytes), byte_swap);
    default:
      break;
  }
  return QString();
}

QByteArray toBytes(const QString& repr, DataType type, bool byte_swap)
{
  if (type == DataType::BYTES)
  {
    QByteArray hex;
    hex += repr;
    return QByteArray::fromHex(hex);
  }
  else if (type == DataType::UTF8)
  {
    return repr.toUtf8();
  }

  switch (type)
  {
    case DataType::U8:
      return toBytes<uint8_t>(repr, byte_swap);
    case DataType::I8:
      return toBytes<int8_t>(repr, byte_swap);
    case DataType::U16:
      return toBytes<uint16_t>(repr, byte_swap);
    case DataType::I16:
      return toBytes<int16_t>(repr, byte_swap);
    case DataType::U32:
      return toBytes<uint32_t>(repr, byte_swap);
    case DataType::I32:
      return toBytes<int32_t>(repr, byte_swap);
    case DataType::U64:
      return toBytes<uint64_t>(repr, byte_swap);
    case DataType::I64:
      return toBytes<int64_t>(repr, byte_swap);
    case DataType::FLOAT:
      return toBytes<float>(repr, byte_swap);
    case DataType::DOUBLE:
      return toBytes<double>(repr, byte_swap);
    default:
      break;
  }

  return QByteArray();
}
