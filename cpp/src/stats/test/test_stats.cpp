// 2024/1/3
// zhangzhong

#include <sstream>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "stats/cpp_analyzer.hpp"
#include <doctest/doctest.h>

TEST_CASE("hello") {
    CHECK(1 == 1);
    CHECK(1 == 1);
    CHECK(1 == 1);
}

TEST_CASE("simple cpp code without raw string") {
    std::string code = R"(
// 2024/1/2
// zhangzhong

#include <iostream>

int main() {
    std::cout << "hello world" << std::endl;
    return 0;
}
)";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->statistics();
    CHECK(result->line_count == 10);
    CHECK(result->code_count == 5);
    CHECK(result->blank_count == 3);
    CHECK(result->annotation_count == 2);
}