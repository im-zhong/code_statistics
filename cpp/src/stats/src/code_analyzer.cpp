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

auto CodeAnalyzer::MyGetline(std::istream& is, std::string& line)
    -> std::istream& {
    if (std::getline(is, line)) {
        ++line_end;
        analysis_result_->line_category.push_back(0);
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
    auto category = LineCategory::kCode;

    // TODO: control word是干嘛的？
    while (MyGetline(is, line)) {

        // aljdflajdlfjasdkf\
        asdfasdfsadlfjsadfl 'asdfsadf'

        // 差不多就是这样的逻辑了
        // offset没有重置
        offset = 0;
        // offset = FindFirstNotBlank(line, offset);
        // // 这里要有处理空行的逻辑
        // if (line.empty() || (offset == std::string::npos)) {
        //     SetLineCategory(line_begin, LineCategory::kBlank);
        //     line_begin = line_end;
        //     continue;
        // }

        // 这样可以迅速跳过空白
        // 但是问题来了 我们怎么添加空白行呢?? 要不就默认空白行吧
        // 然后其他行可以掩盖空白行
        while ((offset = FindFirstNotBlank(line, offset)) !=
               std::string::npos) {
            if (IsStringHead(line, offset)) {
                // 接下来我们要对可能出现的跨行做处理
                offset = SkipString(is, line, offset);
            } else if (IsRawStringHead(line, offset)) {
                offset = SkipRawString(is, line, offset);
            } else if (IsLineCommentHead(line, offset)) {
                // 只有在行注释的时候 这一行是可以跳过的
                // 我们可以写一个SkipLineComment 在里面处理line category
                // 然后返回一个offset 跳出循环 这样就不用写这个break了
                // 大家的逻辑就都一样了
                // category = LineCategory::kLineComment;
                // break;
                offset = SkipLineComment(is, line, offset);
            } else if (IsBlockCommentHead(line, offset)) {
                offset = SkipBlockComment(is, line, offset);
            } else {
                // TODO: 怎么优化逻辑把这个分支去掉呢？
                // normal code, just return code type
                // 绝大多数情况下都是走这个分支
                // category = LineCategory::kCode;
                // 不行 也不能跳 我们还是得过一遍所有的代码
                // 因为我们不知道之后会不会有块注释
                // break;
                // BUG: 全是代码 一直走这个逻辑 到最后没有人更新 line_begin =
                // line_end
                // SetLineCategory(line_begin, LineCategory::kCode);
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

// TODO: 可以复用
// auto CodeAnalyzer::SkipChar(const std::string& line, size_t offset) ->
// size_t
// {
//     auto my_offset = line.find('\'', offset + 1);
//     if (my_offset != std::string::npos) {
//         offset = my_offset;
//         while (line[offset - 1] == '\\' &&
//                line[offset - 2] != '\\') { // 这是一个转义' 忽视 再次查找
//                '
//             offset = line.find('\'', offset + 1);
//         }
//     }
//     ++offset;
// }

// 普通的string是不能换行的 是可以换行的 只要接 \ 就行了

auto CodeAnalyzer::SkipLineComment(std::istream& is, std::string& line,
                                   size_t offset) -> size_t {
    SetLineCategory(line_begin, LineCategory::kLineComment);
    line_begin = line_end;
    // while (MyGetline(is, line)) {
    //     SetLineCategory(line_begin, LineCategory::kLineComment);
    //     // check last char is not '\'
    //     // if (line.back() != '\\') {
    //     //     // 如果不是\ 那么就说明这一行是注释行
    //     //     // 我们需要跳出循环
    //     //     // 跳出循环之前 我们需要将line_begin更新到line_end
    //     //     // 因为我们在这里已经确定了这一行是注释行
    //     //     // 但是我们不知道下一行是不是注释行
    //     //     // 所以我们需要将line_begin更新到line_end
    //     //     // 然后在下一次循环中再次更新line_begin
    //     //     // 这样就可以保证line_begin一直指向的是代码行
    //     //     line_begin = line_end;
    //     //     break;
    //     // }
    //     // TODO: 这样吧 咱们现在先不考虑 \ 连接多行的问题
    // }
    // return offset;
    return std::string::npos;
}

auto CodeAnalyzer::SkipString(std::istream& is, std::string& line,
                              size_t offset) -> size_t {

    // delimiter = ' or "
    std::string_view delimiter{line.substr(offset, 1)};

    // string raw_delimiter;
    // switch (find_raw_literal_head(line, offset, raw_delimiter)) {
    // case LineCategory::kStringLiteral:
    //     break;
    // case LineCategory::kRawLiteral:
    //     return;
    // case LineCategory::kRawLiteralHead:
    //     // skip raw literal
    //     skip_raw_literal(is, line, offset, raw_delimiter);
    //     return;
    // };

    // 直接查看line最后一个字符是不是

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
        // 这里怎么会不对呢？这不是直接copy的代码吗？？？
        line_begin = line_end;
        // 然后读取下一行 继续在下一行寻找字符串的结尾
    } while (MyGetline(is, line));

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

    // raw string 可以跨行 和 block annotation 一样
    // 那么这两个函数就都需要一个 delimiter
    // 这样实现起来也是一样的
    // std::string raw{R"de(hel
    // lo)de"};
    do {
        // TODO: 将所有我们的代码改成这种返回offset的方式 和标准库保持一致
        offset = line.find(delimiter, offset);
        if (offset != std::string::npos) {
            offset += delimiter.size();
            break;
        }
    } while (MyGetline(is, line));

    // begin -> ... { ...
    //			...
    //			... } ...
    // end   -> ...
    //
    //			... { ...
    //			...
    // begin -> ... } ...
    // end   -> ...
    for (; line_begin != line_end; ++line_begin)
        SetLineCategory(line_begin, line_category);
    // TODO:
    // 这句代码出现在这里非常不好
    // 应该放到Analyze的大循环里
    // 两个find blank也应该合并
    // offset = FindFirstNotBlank(line, offset);
    --line_begin;
    return offset;
}

void CodeAnalyzer::SetLineCategory(size_t line, LineCategory category) {
    analysis_result_->line_category[line] |= static_cast<uint>(category);
}

} // namespace stats
