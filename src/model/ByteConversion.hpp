#pragma once

#include <QByteArray>
#include <QString>
#include <cstdint>

enum class DataType {
  U8,
  I8,
  U16,
  I16,
  U32,
  I32,
  U64,
  I64,
  FLOAT,
  DOUBLE,
  BYTES,
  UTF8,

  ARRAY_SIZE_,
  MAX_ = ARRAY_SIZE_ - 1,
  MIN_ = U8,
};

/** Checks if a DataType enumeration refers to a numeric type (integer or floating point).
 * @param type DataType to inspect.
 * @returns true if @p type refers to a numeric type, false otherwise. Unrecognized values return false.
 */
inline constexpr bool isNumber(DataType type) noexcept
{
  return ((DataType::U8 <= type) && (type <= DataType::DOUBLE));
}

/** Determines the static size of a data type in bytes. If the type is variable length, 0 is returned.
 * @param type DataType to inspect.
 * @return 0 if the type is variable length (such as bytes or a string), otherwise returns the type's size.
 */
inline constexpr int sizeOf(DataType type) noexcept
{
  switch (type)
  {
    case DataType::U8:
      return 1;
    case DataType::I8:
      return 1;
    case DataType::U16:
      return 2;
    case DataType::I16:
      return 2;
    case DataType::U32:
      return 4;
    case DataType::I32:
      return 4;
    case DataType::U64:
      return 8;
    case DataType::I64:
      return 8;
    case DataType::FLOAT:
      return 4;
    case DataType::DOUBLE:
      return 8;
    default:
      break;
  }
  return 0;
}

/** Returns a string representation describing the type.
 * @param type DataType to describe.
 * @return String name. An empty string is returned if out of bounds.
 */
QString nameof(DataType type) noexcept;

/** Interprets a series of bytes as the given DataType.
 * If the input byte size exceeds the size of the provided @p type, the @p bytes is truncated.
 * @param bytes Byte array to interpret.
 * @param type DataType to interpret @p bytes as.
 * @param byte_swap Set to true if the endianness of @p bytes does not match the host.
 * @return String representation of @p bytes interpreted as @p type. Empty string returned on failure.
 */
QString toString(QByteArray bytes, DataType type, bool byte_swap = false);

/** Interprets a string as the given DataType.
 * @param repr String to interpret.
 * @param type DataType to interpret @p repr as.
 * @param byte_swap Set to true if the desired endianness of the output does not match the host.
 * @return Serialized byte array of @p repr interpreted as @p type. Empty byte array returned on failure.
 */
QByteArray toBytes(const QString& repr, DataType type, bool byte_swap = false);
