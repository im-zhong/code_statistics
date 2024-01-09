// 2024/1/3
// zhangzhong

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "conf/conf.hpp"
#include "driver/driver.hpp"
#include "stats/analysis_result.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include <doctest/doctest.h>
#include <fmt/core.h>
#include <memory>
#include <sstream>

auto ResultToString(std::shared_ptr<stats::AnalysisResult> result) {
    auto ss = std::stringstream{};
    for (size_t i = 0; i < result->line_categories.size(); i++) {
        ss << fmt::format(
            "{} : {}\n", i + 1,
            stats::LineCategoryToString(
                static_cast<stats::LineCategory>(result->line_categories[i])));
    }
    return ss.str();
}

void PrintResult(std::shared_ptr<stats::AnalysisResult> result) {
    fmt::print(ResultToString(result));
}

auto TestGolden(std::string const& result, std::string const& golden_path) {
    auto fin = std::ifstream{golden_path};
    auto golden_str = std::string{(std::istreambuf_iterator<char>(fin)),
                                  std::istreambuf_iterator<char>()};
    CHECK(result == golden_str);
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

TEST_CASE("test block comment") {
    auto code = std::string{R"(/*
xxx
xxx
*/

int main() {}
)"};
    auto analyzer = stats::MakeCppAnalyzer();
    std::stringstream ss(code);
    auto result = analyzer->AnalyzeFile(ss);
    result->Statistics();
    CHECK(result->line_count == 6);
    CHECK(result->code_count == 1);
    CHECK(result->blank_count == 1);
    CHECK(result->comment_count == 4);
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
}

TEST_CASE("simple cpp file") {
    std::string path{"../../../../src/stats/src/"
                     "cpp_analyzer.cpp"};
    auto analyzer = stats::MakeCppAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
}

TEST_CASE("test raw string") {
    std::string path{"../../../../src/stats/test/"
                     "test_stats.cpp"};
    auto analyzer = stats::MakeCppAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
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
}

TEST_CASE("test simple python file") {
    std::string path{"../../../../conanfile.py"};
    auto analyzer = stats::MakePythonAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
}

TEST_CASE("#bug1: test python file") {
    std::string path{"../../../../src/stats/test/goldens/"
                     "stance_predict.py"};
    auto analyzer = stats::MakePythonAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    TestGolden(ResultToString(result), "../../../../src/stats/test/goldens/"
                                       "stance_predict.txt");
}

TEST_CASE("#bug2: test python file") {
    std::string path{"../../../../src/stats/test/"
                     "goldens/file_process.py"};
    auto analyzer = stats::MakePythonAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    TestGolden(ResultToString(result), "../../../../src/stats/test/goldens/"
                                       "file_process.txt");
}

TEST_CASE("#bug3: test python file") {
    std::string path{"../../../../src/stats/test/"
                     "goldens/lispy.py"};
    auto analyzer = stats::MakePythonAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    TestGolden(ResultToString(result), "../../../../src/stats/test/goldens/"
                                       "lispy.txt");
}

TEST_CASE("#bug4: test cpp file") {
    std::string path{"../../../../src/stats/test/"
                     "goldens/string.hpp"};
    auto analyzer = stats::MakeCppAnalyzer();
    auto result = analyzer->Analyze(path);
    result->Statistics();
    // PrintResult(result);
    TestGolden(ResultToString(result), "../../../../src/stats/test/goldens/"
                                       "string.txt");
}

TEST_CASE("test all analyzer") {
    std::string path{"../../../../"};
    auto language = std::string{"all"};
    auto conf = conf::MakeConf();
    conf->AddLoadPath(path);
    conf->AddLanguage(language);

    auto driver = driver::Driver(conf, stats::MakeCodeAnalyzer(language));
    driver.Run();
}
