#pragma once

#include <cstdint>
#include <optional>
#include <string>

enum FilterResult {
  FilterResult_PASS         = 0,
  FilterResult_FAIL         = 1,
  FilterResult_SYNTAX_ERROR = 2,
};

/** Checks to see if a value yields true when evaluated against every expression in a compound expression.
 * @param compound_expression A comma-delimited string containing a comparator and a value.
 *
 * Acceptable comparators include:
 * - `>`
 * - `>=`
 * - `<`
 * - `<=`
 * - `!=`
 * - `==`
 * - `` (empty, equivalent to `==`)
 *
 * The provided value may be in decimal or hex, provided the value begins with `0x`.
 *
 * @code{.cpp}
   filterValueWithExpression("== 100", 100);        // PASS
   filterValueWithExpression("== 100", 101);        // FAIL
   filterValueWithExpression("> 10, < 1000", 101);  // PASS
   filterValueWithExpression("== 0x100", 0x100);    // PASS
   filterValueWithExpression(">>> 100", 100);       // SYNTAX_ERROR
 * @endcode
 *
 * @param value Value inserted to the beginning of each expression in the @p compound_expression.
 * @returns FilterResult_PASS if the evaluated expression yields true, or if @p compound_expression was empty.
 * @returns FilterResult_FAIL if the evaluated expression yields false.
 * @returns FilterResult_SYNTAX_ERROR if the provided expression could not be parsed.
 */
FilterResult filterValueWithExpression(std::string compound_expression, std::uint64_t value);
