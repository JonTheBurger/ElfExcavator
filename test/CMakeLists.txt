add_executable(ByteConversion.test ByteConversion.test.cpp)
target_link_libraries(ByteConversion.test PRIVATE Catch2::Catch2WithMain elfexcavator.core)
add_test(NAME ByteConversion.test COMMAND ByteConversion.test)

add_executable(FilterExpressionEvaluator.test FilterExpressionEvaluator.test.cpp)
target_link_libraries(FilterExpressionEvaluator.test PRIVATE Catch2::Catch2WithMain elfexcavator.core)
add_test(NAME FilterExpressionEvaluator.test COMMAND FilterExpressionEvaluator.test)
