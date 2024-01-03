// 2024/1/3
// zhangzhong

#include "stats/cpp_analyzer.hpp"

namespace stats {

auto CppAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                size_t offset) -> size_t {
    auto raw_literal_end = FindRawStringHead(line, offset);
    return SkipUntilFindDelimiter(is, line, offset, raw_literal_end,
                                  LineCategory::kRawString);
}

auto CppAnalyzer::FindRawStringHead(const std::string_view& line, size_t offset)
    -> std::string {
    std::string raw_delimiter{};
    if (offset > 0 && line[offset - 1] == 'R') {
        // 从当前位置开始 查找 (
        // R"de( ... )de";
        auto delimiter_begin = offset + 1; // i -> ", begin = d
        auto delimiter_end = line.find('(', delimiter_begin); // end -> （

        // 如果找不到那么表示这不是一个原生字符串
        // if (delimiter_end == std::string::npos) {
        //     return LineCategory::kString;
        // }

        // 这可能是一个原生字符 因为原生字符可以换行
        // 所以我们需要不断查找直到找到结束符 首先获取结束符
        raw_delimiter.clear();
        raw_delimiter.push_back(')');

        // [begin,end) = de
        // 构建delimiter
        for (auto begin = delimiter_begin; begin < delimiter_end; ++begin) {
            auto c = line[begin];
            // if (c == ' ' || c == '\\' || (iscntrl(c) != 0))
            //     return LineCategory::kStringLiteral;
            raw_delimiter.push_back(c);
        }
        raw_delimiter.push_back('"');

        auto raw_literal_end = line.find(raw_delimiter, delimiter_end + 1);
        if (raw_literal_end !=
            std::string::npos) { // 将offset更新到rawliteral尾部 跳过rawliteral
            offset = raw_literal_end + raw_delimiter.size();
            // return LineCategory::kRawLiteral;
        } else {
            // 如果没有找到 完了 原生字符在本行没有结束 我们需要设置标志位
            // 更新外部函数来支持 is_raw_literal = true;
            // return LineCategory::kRawLiteralHead;
        }
    }
    // return LineCategory::kStringLiteral;
    return raw_delimiter;
}

auto MakeCppAnalyzer() -> std::shared_ptr<CppAnalyzer> {
    return std::make_shared<CppAnalyzer>();
}

} // namespace stats