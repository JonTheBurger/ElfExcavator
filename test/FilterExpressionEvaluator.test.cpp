#include <catch2/catch.hpp>

#include "model/TextFilter.hpp"

SCENARIO("The filter expression evaluator parses expressions properly", "[FilterExpressionEvaluator]")
{
  GIVEN("Any value")
  {
    WHEN("The expression is empty")
    {
      auto value = GENERATE(as<uint64_t>{}, 100, 0, 0xFFFFFFFF);
      THEN("The filter accepts the value")
      {
        REQUIRE(FilterResult_PASS == filterValueWithExpression("", value));
      }
    }

    WHEN("The expression is invalid")
    {
      auto syntax_error_expression = GENERATE(">", "-", "><", "=<", "asdf", "0xzzz");
      THEN("The filter returns a syntax error")
      {
        REQUIRE(FilterResult_SYNTAX_ERROR == filterValueWithExpression(syntax_error_expression, 0));
      }
    }
  }

  GIVEN("100")
  {
    auto failed_expression = GENERATE(" > 100 ", " >= 101 ", " < 100 ", " <= 99 ", " == 101 ", " 99 ");
    WHEN(std::string{ "The expression is '" } + failed_expression + "'")
    {
      THEN("The filter rejects the value")
      {
        REQUIRE(FilterResult_FAIL == filterValueWithExpression(failed_expression, 100));
      }
    }

    auto passed_expression = GENERATE(" > 99 ", " >= 99", " >= 100 ", " < 101 ", " <= 101 ", " <= 100 ", " == 100 ", " 100 ");
    WHEN(std::string{ "The expression is '" } + passed_expression + "'")
    {
      THEN("The filter accepts the value")
      {
        REQUIRE(FilterResult_PASS == filterValueWithExpression(passed_expression, 100));
      }
    }
  }

  GIVEN("0x100")
  {
    auto failed_expression = GENERATE(" > 0x100 ", " >= 0x101 ", " < 0x100 ", " <= 0x99 ", " == 0x101 ", " 0x99 ");
    WHEN(std::string{ "The expression is '" } + failed_expression + "'")
    {
      THEN("The filter rejects the value")
      {
        REQUIRE(FilterResult_FAIL == filterValueWithExpression(failed_expression, 0x100));
      }
    }

    auto passed_expression = GENERATE(" > 0x99 ", " >= 0x99", " >= 0x100 ", " < 0x101 ", " <= 0x101 ", " <= 0x100 ", " == 0x100 ", " 0x100 ");
    WHEN(std::string{ "The expression is '" } + passed_expression + "'")
    {
      THEN("The filter accepts the value")
      {
        REQUIRE(FilterResult_PASS == filterValueWithExpression(passed_expression, 0x100));
      }
    }
  }
}
