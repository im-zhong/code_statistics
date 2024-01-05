// 2024/1/3
// zhangzhong

#include "stats/cpp_analyzer.hpp"
#include "stats/code_analyzer.hpp"

namespace stats {

CppAnalyzer::CppAnalyzer() : CodeAnalyzer("//", "/*", "*/") {}

auto CppAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                size_t offset) -> size_t {
    auto raw_string_tail = GetRawStringTail(line, offset);
    return SkipUntilFindDelimiter(is, line, offset, raw_string_tail,
                                  LineCategory::kCode);
}

auto CppAnalyzer::GetRawStringTail(std::string_view const& line, size_t offset)
    -> std::string {
    // R"de( ... )de";
    auto raw_string_tail = std::string{};
    // offset -> R, begin = d
    auto begin = offset + 2;
    // end ->（
    auto end = line.find('(', begin);

    // construct raw_delimiter: )de"
    raw_string_tail.push_back(')');
    raw_string_tail += line.substr(begin, end - begin);
    raw_string_tail.push_back('"');
    return raw_string_tail;
}

auto MakeCppAnalyzer() -> std::shared_ptr<CppAnalyzer> {
    return std::make_shared<CppAnalyzer>();
}

} // namespace stats
