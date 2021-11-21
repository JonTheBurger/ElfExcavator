#include "TextFilter.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

static std::optional<uint64_t> parseNumberFromExpression(std::string_view expression)
{
  size_t first_digit_idx = 0;

  // Skip past comparator (e.g. ">=") to first digit
  while ((first_digit_idx < expression.size()) && (!std::isdigit(expression[first_digit_idx])))
  {
    ++first_digit_idx;
  }

  // Check if first digit is the zero indicator for "0x"
  int radix = 10;
  if (expression.find("0x", first_digit_idx) == first_digit_idx)
  {
    first_digit_idx += std::char_traits<char>::length("0x");  // char_traits::length() is constexpr in C++17
    radix = 16;
  }

  // Attempt to parse string with deduced radix
  auto          string_value = expression.substr(first_digit_idx);
  std::uint64_t parsed_value;
  auto          result = std::from_chars(string_value.begin(), string_value.end(), parsed_value, radix);

  if (result.ec != std::errc{})
  {
    return {};
  }
  return parsed_value;
}

FilterResult filterValueWithExpression(std::string compound_expression, std::uint64_t value)
{
  compound_expression.erase(
    std::remove_if(compound_expression.begin(), compound_expression.end(), ::isspace), compound_expression.end());

  auto view =
    compound_expression |
    ranges::views::split(',') |
    ranges::views::transform([](auto&& rng) {
      return std::string_view(&*rng.begin(), static_cast<size_t>(ranges::distance(rng)));
    });

  for (auto expr : view)
  {
    auto number = parseNumberFromExpression(expr);
    if (!number)
    {
      return FilterResult_SYNTAX_ERROR;
    }
    auto parsed_value = *number;

    if (expr.find(">=") == 0)
    {
      if (!(value >= parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (expr.find("<=") == 0)
    {
      if (!(value <= parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (expr.find(">") == 0)
    {
      if (!(value > parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (expr.find("<") == 0)
    {
      if (!(value < parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (expr.find("!=") == 0)
    {
      if (!(value != parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (expr.find("==") == 0)
    {
      if (!(value == parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else if (std::isdigit(expr[0]))
    {
      if (!(value == parsed_value))
      {
        return FilterResult_FAIL;
      }
    }
    else
    {
      return FilterResult_SYNTAX_ERROR;
    }
  }

  return FilterResult_PASS;
}
