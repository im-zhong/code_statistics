// 2024/1/2
// zhangzhong

#include "stats/code_analyzer.hpp"
#include <cstddef>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>

namespace stats {

std::shared_ptr<AnalysisResult> CodeAnalyzer::Analyze(const std::string& path) {
    std::cout << "stats... " << path << std::endl;
    Init();
    std::ifstream fin(path);
    if (fin.is_open()) {
        analysis_result_->path = path;
        auto result = AnalyzeFile(fin);
        fin.close();
        return result;
    }
    return nullptr;
}

auto CodeAnalyzer::GetLineAndResetOffset(std::istream& is, std::string& line,
                                         size_t& offset) -> std::istream& {
    if (std::getline(is, line)) {
        ++line_end;
        offset = 0;
        analysis_result_->line_category.push_back(LineCategory::kBlank);
    }
    return is;
}

// 打住 现在不要思考这些细节上的问题 先把程序跑通
auto CodeAnalyzer::IsStringHead(const std::string_view& line, size_t offset)
    -> bool {
    return line.substr(offset, 1) == std::string_view{"\'"} ||
           line.substr(offset, 1) == std::string_view{"\""};
}

auto CodeAnalyzer::IsRawStringHead(const std::string_view& line, size_t offset)
    -> bool {
    return line.substr(offset, 2) == std::string_view{"R\""} ||
           line.substr(offset, 2) == std::string_view{"r\""};
}

auto CodeAnalyzer::IsLineCommentHead(const std::string_view& line,
                                     size_t offset) -> bool {
    return line.substr(offset, line_comment_head_.size()) == line_comment_head_;
}

auto CodeAnalyzer::IsBlockCommentHead(const std::string_view& line,
                                      size_t offset) -> bool {
    return line.substr(offset, block_comment_head_.size()) ==
           block_comment_head_;
}

std::shared_ptr<AnalysisResult> CodeAnalyzer::AnalyzeFile(std::istream& is) {
    std::string line{};
    size_t offset{};

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
                SetLineCategory(line_begin, LineCategory::kCode);
                ++offset;
            }
        }
        line_begin = line_end;
    }
    return analysis_result_;
}

auto CodeAnalyzer::Init() -> void {
    line_begin = 0;
    line_end = 0;

    analysis_result_ = std::make_shared<AnalysisResult>();
}

// 找不到的话返回 std::npos,从 offset开始寻找第一个非空字符
auto CodeAnalyzer::FindFirstNotBlank(const std::string& line, size_t offset)
    -> size_t {
    for (; offset < line.size(); ++offset) {
        if (!std::isspace(line[offset]))
            return offset;
    }
    return std::string::npos;
}

auto CodeAnalyzer::SkipLineComment(std::istream& is, std::string& line,
                                   size_t offset) -> size_t {

    while (line.back() == '\\') {
        GetLineAndResetOffset(is, line, offset);
    }

    // 从line_begin 到 line_end 都写成注释
    // 这段代码重复了 复用一下
    for (; line_begin != line_end; ++line_begin) {
        SetLineCategory(line_begin, LineCategory::kLineComment);
    }
    --line_begin;
    return std::string::npos;
}

auto CodeAnalyzer::SkipString(std::istream& is, std::string& line,
                              size_t offset) -> size_t {

    // delimiter = ' or "
    std::string_view delimiter{line.substr(offset, 1)};

    do {
        SetLineCategory(line_begin, LineCategory::kCode);
        size_t current_index = 0;
        size_t slash_size = 0;
        offset = line.find(delimiter, offset + delimiter.size());
        // 在处理普通字符串的时候
        // 我们有可能碰到"在本行找不到配对的“
        // 必然是因为使用了\ 导致的
        // 所以只需要针对这种特殊情况进行特殊处理就可以了
        // deal with escape sequence
        while (offset < line.size()) {
            current_index = offset - 1;
            slash_size = 0;

            while (line[current_index] == '\\') {
                // 向前查看是否为 斜线
                ++slash_size;    // 增加 \ 的数量
                --current_index; // 继续向前查看
            }
            // 最后统计出 \ 的数量之后 查看是奇数还是偶数
            if (slash_size % 2 == 0) {
                // 偶数
                // 如果有偶数个 那么他们互相匹配 这个"就和前面的匹配
                // 也就是说 字符串到这里就结束了 也就是 i 处字符串结束
                ++offset;
                return offset;
            } else {
                // 如果是奇数  就说明这是一个转义序列
                // 我们还需要从下一个位置再次查找 并且再次判断是不是正常结束
                offset = line.find(delimiter, offset + delimiter.size());
            }

            // 其实在这里也可能找不到配对的”
            // 所以我们必须要在一开始就判断是不是文本连接的字符串
        }
        // 如果退出来了 就说明在本行没有找到吧
        // 也就是说 在这里我们需要寻找下一行
        // 当前行的属性已经可以确定是代码行
        line_begin = line_end;
        // 然后读取下一行 继续在下一行寻找字符串的结尾
    } while (GetLineAndResetOffset(is, line, offset));

    // 因为文本一定是正确的 所以offset一定停在“上面
    // ++offset;
    // offset停留在"上或者npos
    return offset;
}

auto CodeAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                 size_t offset) -> size_t {

    // 首先自己找到 raw_string_head
    // line[offset:offset+2] = r' or r"
    // 然后我们找括号 找到之后就凑出来一个raw string 其实这里应该写 python的那种
    // 比较简单 C++的raw string设计的比较复杂 应该放在C++的类里面实现
    std::string raw_string_head{line.substr(offset, 2)};
    std::string raw_string_tail{raw_string_head.rbegin(),
                                raw_string_head.rend()};
    return SkipUntilFindDelimiter(is, line, offset, raw_string_tail,
                                  LineCategory::kCode);
}

auto CodeAnalyzer::SkipBlockComment(std::istream& is, std::string& line,
                                    size_t offset) -> size_t {
    return SkipUntilFindDelimiter(is, line, offset, block_comment_tail_,
                                  LineCategory::kBlockComment);
}

// 为了统一行为
// skip_until_find_delimiter会跳到delimiter的后一个位置
// 这可能是无效位置
// raw string 可以跨行吗
auto CodeAnalyzer::SkipUntilFindDelimiter(std::istream& is, std::string& line,
                                          size_t offset,
                                          const std::string_view& delimiter,
                                          LineCategory line_category)
    -> size_t {
    while ((offset = line.find(delimiter, offset)) == std::string::npos) {
        GetLineAndResetOffset(is, line, offset);
    }
    offset += delimiter.size();

    // begin -> ... head ...
    //			...
    //			... tail ...
    // end   -> ...
    //
    //			... head ...
    //			...
    // begin -> ... tail ...
    // end   -> ...
    for (; line_begin != line_end; ++line_begin) {
        SetLineCategory(line_begin, line_category);
    }
    --line_begin;
    return offset;
}

void CodeAnalyzer::SetLineCategory(size_t line, LineCategory category) {
    analysis_result_->line_category[line] |= static_cast<uint>(category);
}

} // namespace stats
