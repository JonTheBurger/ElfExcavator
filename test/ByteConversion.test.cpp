#include "model/ByteConversion.hpp"

#include <catch2/catch.hpp>

/** @note These tests will only pass on a little endian machine and a local that uses dot as the float separator. */
SCENARIO("Stringification of Byte Arrays", "[ByteConversion]")
{
  GIVEN("A numeric series of bytes")
  {
    auto bytes = QByteArray::fromHex("4142434445464748c2a7");

    WHEN("the interpreted type is int8_t")
    {
      auto type = DataType::I8;

      THEN("bytes are interpreted as an int8_t")
      {
        REQUIRE("65" == toString(bytes, type, false));
      }

      THEN("endianness has no effect")
      {
        REQUIRE("65" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is uint16_t")
    {
      auto type = DataType::U16;

      THEN("bytes are interpreted as an uint16_t")
      {
        REQUIRE("16961" == toString(bytes, type, false));
      }

      THEN("endianness swaps the bytes")
      {
        REQUIRE("16706" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is int32_t")
    {
      auto type = DataType::I32;

      THEN("bytes are interpreted as an int32_t")
      {
        REQUIRE("1145258561" == toString(bytes, type, false));
      }

      THEN("endianness swaps the bytes")
      {
        REQUIRE("1094861636" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is uint64_t")
    {
      auto type = DataType::U64;

      THEN("bytes are interpreted as an uint64_t")
      {
        REQUIRE("5208208757389214273" == toString(bytes, type, false));
      }

      THEN("endianness swaps the bytes")
      {
        REQUIRE("4702394921427289928" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is float")
    {
      auto type = DataType::FLOAT;

      THEN("bytes are interpreted as float")
      {
        REQUIRE("781.035" == toString(bytes, type, false));
      }

      THEN("endianness swaps the bytes")
      {
        REQUIRE("12.1414" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is BYTES")
    {
      auto type = DataType::BYTES;

      THEN("bytes are interpreted as Hexadecimal")
      {
        REQUIRE("4142434445464748c2a7" == toString(bytes, type, false));
      }

      THEN("endianness has no effect")
      {
        REQUIRE("4142434445464748c2a7" == toString(bytes, type, true));
      }
    }

    WHEN("the interpreted type is UTF-8")
    {
      auto type = DataType::UTF8;

      THEN("bytes are interpreted as text")
      {
        REQUIRE(QStringLiteral("ABCDEFGH§") == toString(bytes, type, false));
      }

      THEN("endianness has no effect")
      {
        REQUIRE(QStringLiteral("ABCDEFGH§") == toString(bytes, type, false));
      }
    }
  }
}

SCENARIO("Serialization of Strings", "[ByteConversion]")
{
  GIVEN("")
  {
    WHEN("")
    {
      THEN("")
      {
        REQUIRE(QByteArray::fromHex("4241") == toBytes("16706", DataType::U16));
      }
    }
  }
}
