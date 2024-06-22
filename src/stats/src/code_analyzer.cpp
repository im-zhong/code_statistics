// 2024/1/2
// zhangzhong

#include "stats/code_analyzer.hpp"
#include "stats/all_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include "stats/rust_analyzer.hpp"
#include "stats/ts_analyzer.hpp"
#include <cstddef>
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>

namespace stats {

CodeAnalyzer::CodeAnalyzer(std::string const& line_comment_head,
                           std::string const& block_comment_head,
                           std::string const& block_comment_tail)
    : result_{std::make_shared<AnalysisResult>()}, line_begin_{0}, line_end_{0},
      line_comment_head_{line_comment_head},
      block_comment_head_{block_comment_head},
      block_comment_tail_{block_comment_tail} {}

auto CodeAnalyzer::Analyze(std::string const& path)
    -> std::shared_ptr<AnalysisResult> {
    fmt::println("stats... {}", path);
    Init();
    auto fin = std::ifstream{path};
    if (fin.is_open()) {
        result_->path = path;
        auto result = AnalyzeFile(fin);
        fin.close();
        return result;
    }
    return nullptr;
}

auto CodeAnalyzer::Init() -> void {
    line_begin_ = 0;
    line_end_ = 0;
    result_ = std::make_shared<AnalysisResult>();
}

// 分析一个文件的内容
auto CodeAnalyzer::AnalyzeFile(std::istream& is)
    -> std::shared_ptr<AnalysisResult> {
    auto line = std::string{};
    auto offset = size_t{0};

    while (GetLineAndResetOffset(is, line, offset)) {
        while ((offset = FindFirstNotBlank(line, offset)) !=
               std::string::npos) {
            if (IsStringHead(line, offset)) {
                offset = SkipString(is, line, offset);
            } else if (IsRawStringHead(line, offset)) {
                offset = SkipRawString(is, line, offset);
            } else if (IsLineCommentHead(line, offset)) {
                offset = SkipLineComment(is, line, offset);
            } else if (IsBlockCommentHead(line, offset)) {
                offset = SkipBlockComment(is, line, offset);
            } else {
                SetLineCategory(line_begin_, LineCategory::kCode);
                ++offset;
            }
        }
        line_begin_ = line_end_;
    }
    return result_;
}

// 获取一行内容，并将offset置为0
auto CodeAnalyzer::GetLineAndResetOffset(std::istream& is, std::string& line,
                                         size_t& offset) -> std::istream& {
    if (std::getline(is, line)) {
        ++line_end_;
        offset = 0;
        result_->line_categories.push_back(LineCategory::kBlank);
    }
    return is;
}

auto CodeAnalyzer::FindFirstNotBlank(std::string const& line, size_t offset)
    -> size_t {
    for (; offset < line.size(); ++offset) {
        if (!std::isspace(line[offset]))
            return offset;
    }
    return std::string::npos;
}

auto CodeAnalyzer::SetLineCategory(size_t line, LineCategory category) -> void {
    result_->line_categories[line] |= static_cast<uint>(category);
}

auto CodeAnalyzer::SetMultiLineCategory(size_t begin, size_t end,
                                        LineCategory category) -> void {
    for (auto i = begin; i != end; ++i) {
        SetLineCategory(i, category);
    }
}

auto CodeAnalyzer::IsStringHead(std::string_view const& line, size_t offset)
    -> bool {
    // construct the sub string and compare it is very slow
    return line[offset] == '\'' || line[offset] == '\"';
}

auto CodeAnalyzer::IsRawStringHead(std::string_view const& line, size_t offset)
    -> bool {
    if (line[offset] == 'r' || line[offset] == 'R') {
        if (line[offset + 1] == '\'' || line[offset + 1] == '\"') {
            // the character before r must be a blank or at the front the line
            // i.e. operator"" is not raw string head
            if (offset == 0 || std::isspace(line[offset - 1])) {
                return true;
            }
        }
    }
    return false;
}

auto CodeAnalyzer::IsLineCommentHead(std::string_view const& line,
                                     size_t offset) -> bool {
    return line.substr(offset, line_comment_head_.size()) == line_comment_head_;
}

auto CodeAnalyzer::IsBlockCommentHead(std::string_view const& line,
                                      size_t offset) -> bool {
    return line.substr(offset, block_comment_head_.size()) ==
           block_comment_head_;
}

auto CodeAnalyzer::SkipString(std::istream& is, std::string& line,
                              size_t offset) -> size_t {
    // delimiter is ' or "
    auto delimiter = std::string_view{line.substr(offset, 1)};
    auto is_find_delimiter = false;
    auto current_index = size_t{0};
    auto slash_size = size_t{0};

    while (true) {
        current_index = 0;
        slash_size = 0;
        offset = line.find(delimiter, offset + delimiter.size());

        while (offset < line.size()) {
            current_index = offset - 1;
            slash_size = 0;
            // look back to find and count slash
            // i.e. "....\\\", offset point to the last " now
            while (line[current_index] == '\\') {
                ++slash_size;
                --current_index;
            }

            if (slash_size % 2 == 0) {
                // if slash_size is even, then it is not escape sequence
                // and we find the end of string, make offset point to the
                // next position of delimiter
                ++offset;
                is_find_delimiter = true;
                break;
            } else {
                // otherwise it is escape sequence
                // and we need to find the next " at the next position
                offset = line.find(delimiter, offset + delimiter.size());
            }
        }
        if (is_find_delimiter) {
            break;
        }
        // if we reach here, it means we cannot find the delimiter in this line
        if (!GetLineAndResetOffset(is, line, offset)) {
            break;
        }
    }

    SetMultiLineCategory(line_begin_, line_end_, LineCategory::kCode);
    line_begin_ = line_end_ - 1;
    return offset;
}

auto CodeAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                 size_t offset) -> size_t {
    // raw string: r"..."
    // offset -> r, but r should not couat as a part of raw string
    auto raw_string_head = std::string{line.substr(offset + 1, 1)};
    auto raw_string_tail =
        std::string{raw_string_head.rbegin(), raw_string_head.rend()};
    return SkipUntilFindDelimiter(is, line, offset + 1 + raw_string_head.size(),
                                  raw_string_tail, LineCategory::kCode);
}

auto CodeAnalyzer::SkipLineComment(std::istream& is, std::string& line,
                                   size_t offset) -> size_t {
    // deal with '\' at the end of line
    while (line.back() == '\\') {
        GetLineAndResetOffset(is, line, offset);
    }

    SetMultiLineCategory(line_begin_, line_end_, LineCategory::kLineComment);
    line_begin_ = line_end_ - 1;
    return std::string::npos;
}

auto CodeAnalyzer::SkipBlockComment(std::istream& is, std::string& line,
                                    size_t offset) -> size_t {
    return SkipUntilFindDelimiter(is, line, offset + block_comment_head_.size(),
                                  block_comment_tail_,
                                  LineCategory::kBlockComment);
}

auto CodeAnalyzer::SkipUntilFindDelimiter(std::istream& is, std::string& line,
                                          size_t offset,
                                          std::string_view const& delimiter,
                                          LineCategory line_category)
    -> size_t {
    // find delimiter, and set offset to the next position of delimiter
    while ((offset = line.find(delimiter, offset)) == std::string::npos) {
        GetLineAndResetOffset(is, line, offset);
    }
    offset += delimiter.size();

    // initial:
    // begin -> ... head ...
    //			...
    //			... tail ...
    // end   -> ...
    SetMultiLineCategory(line_begin_, line_end_, line_category);
    line_begin_ = line_end_ - 1;
    // after:
    //			... head ...
    //			...
    // begin -> ... tail ...
    // end   -> ...
    return offset;
}

auto MakeCodeAnalyzer(std::string const& language)
    -> std::shared_ptr<CodeAnalyzer> {
    if (language == "cpp") {
        return MakeCppAnalyzer();
    } else if (language == "rust") {
        return MakeRustAnalyzer();
    } else if (language == "python") {
        return MakePythonAnalyzer();
    } else if (language == "ts") {
        return MakeTSAnalyzer();
    } else if (language == "all") {
        return MakeAllAnalyzer();
    }
    return nullptr;
}

} // namespace stats
