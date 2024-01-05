// 2024/1/3
// zhangzhong

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "stats/cpp_analyzer.hpp"
#include <doctest/doctest.h>
#include <fmt/core.h>
#include <sstream>

void PrintResult(std::shared_ptr<stats::AnalysisResult> result) {
    for (size_t i = 0; i < result->line_categories.size(); i++) {
        fmt::println(
            "{} : {}", i + 1,
            stats::LineCategoryToString(
                static_cast<stats::LineCategory>(result->line_categories[i])));
    }
}

TEST_CASE("test skip string") {
    std::string code = R"(#include "test.hpp"
    int main() {})";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 2);
    CHECK(result->code_count == 2);
    CHECK(result->blank_count == 0);
    CHECK(result->comment_count == 0);
}

TEST_CASE("hello") {
    CHECK(1 == 1);
    CHECK(1 == 1);
    CHECK(1 == 1);
}

TEST_CASE("test empty file") {
    std::string code = "";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 0);
    CHECK(result->code_count == 0);
    CHECK(result->blank_count == 0);
    CHECK(result->comment_count == 0);
}

TEST_CASE("test one line file") {
    std::string code = "int main() { return 0; }\n";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 1);
    CHECK(result->code_count == 1);
    CHECK(result->blank_count == 0);
    CHECK(result->comment_count == 0);
}

TEST_CASE("skip at blank") {
    // tip: 首行代码后面有一些空格
    std::string code = R"(/* xxx */      
    int main() {})";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 2);
    CHECK(result->code_count == 1);
    CHECK(result->blank_count == 0);
    CHECK(result->comment_count == 1);
    PrintResult(result);
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
    result->Statistics();
    CHECK(result->line_count == 10);
    CHECK(result->code_count == 5);
    CHECK(result->blank_count == 3);
    CHECK(result->comment_count == 2);
    PrintResult(result);
}

TEST_CASE("simple cpp file") {

    std::string path{"/data/zhangzhong/src/code_statistics/cpp/src/stats/src/"
                     "cpp_analyzer.cpp"};
    auto analyzer = stats::MakeCppAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    PrintResult(result);
}

TEST_CASE("test raw string") {
    std::string path{"/data/zhangzhong/src/code_statistics/cpp/src/stats/test/"
                     "test_stats.cpp"};
    auto analyzer = stats::MakeCppAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    PrintResult(result);
}

TEST_CASE("test span line comment") {
    std::string code = R"(// 2024/1/3\
test test test

int main() {})";
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 4);
    CHECK(result->code_count == 1);
    CHECK(result->blank_count == 1);
    CHECK(result->comment_count == 2);
    PrintResult(result);
}
