// 2024/6/22
// zhangzhong

#include "stats/ts_analyzer.hpp"
#include "stats/code_analyzer.hpp"

namespace stats {

TSAnalyzer::TSAnalyzer() : CodeAnalyzer("//", "/*", "*/") {}

auto TSAnalyzer::IsLineCommentHead(std::string_view const& line, size_t offset)
    -> bool {
    return line[offset] == '/' && line[offset + 1] == '/';
}

auto TSAnalyzer::IsBlockCommentHead(std::string_view const& line, size_t offset)
    -> bool {
    return line[offset] == '/' && line[offset + 1] == '*';
}

auto TSAnalyzer::SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t {
    auto raw_string_tail = GetRawStringTail(line, offset);
    return SkipUntilFindDelimiter(is, line, offset, raw_string_tail,
                                  LineCategory::kCode);
}

auto TSAnalyzer::GetRawStringTail(std::string_view const& line, size_t offset)
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

auto MakeTSAnalyzer() -> std::shared_ptr<TSAnalyzer> {
    return std::make_shared<TSAnalyzer>();
}

} // namespace stats