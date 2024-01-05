// 2024/1/5
// zhangzhong

#include "stats/python_analyzer.hpp"
#include <cstddef>
#include <string_view>

namespace stats {

PythonAnalyzer::PythonAnalyzer() : CodeAnalyzer("#", "", "") {}

auto PythonAnalyzer::IsStringHead(std::string_view const& line, size_t offset)
    -> bool {
    if (line[offset] == '\"') {
        // make sure it is not a block comment head
        if (offset + 2 < line.size()) {
            if (line[offset + 1] == '\"' && line[offset + 2] == '\"') {
                return false;
            }
        }
        return true;
    }
    if (line[offset] == '\'') {
        // make sure it is not a block comment head
        if (offset + 2 < line.size()) {
            if (line[offset + 1] == '\'' && line[offset + 2] == '\'') {
                return false;
            }
        }
        return true;
    }
    return false;
}

auto PythonAnalyzer::IsRawStringHead(std::string_view const& line,
                                     size_t offset) -> bool {
    return line.substr(offset, 4) == "r\"\"\"" ||
           line.substr(offset, 4) == "r'''" ||
           line.substr(offset, 2) == "r\"" || line.substr(offset, 2) == "r'";
}

auto PythonAnalyzer::IsBlockCommentHead(std::string_view const& line,
                                        size_t offset) -> bool {
    return line.substr(offset, 3) == "\"\"\"" ||
           line.substr(offset, 3) == "'''";
}

auto PythonAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                   size_t offset) -> size_t {
    // make sure raw string head is r' or r'''
    auto size = size_t{1};
    if (offset + 3 < line.size()) {
        if (line[offset + 1] == line[offset + 2] &&
            line[offset + 1] == line[offset + 3]) {
            size = 3;
        }
    }
    // raw string: r"..."
    // offset -> r, but r should not couat as a part of raw string
    auto delimiter = line.substr(offset + 1, size);
    return SkipUntilFindDelimiter(is, line, offset + 1 + delimiter.size(),
                                  delimiter, LineCategory::kCode);
}

auto PythonAnalyzer::SkipBlockComment(std::istream& is, std::string& line,
                                      size_t offset) -> size_t {
    auto delimiter = line.substr(offset, 3);
    return SkipUntilFindDelimiter(is, line, offset + delimiter.size(),
                                  delimiter, LineCategory::kBlockComment);
}

auto MakePythonAnalyzer() -> std::shared_ptr<PythonAnalyzer> {
    return std::make_shared<PythonAnalyzer>();
}

} // namespace stats
