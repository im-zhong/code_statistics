#include "stats/code_analysis.hpp"
#include <sstream>
using namespace std;

namespace stats {

std::shared_ptr<CodeAnalyzerBase::AnalysisResult>
CodeAnalyzerBase::analyze(const std::string& path) {
    cout << "stats... " << path << endl;
    init();
    std::ifstream fin(path);
    if (fin.is_open()) {
        analysis_result_ptr->path = path;
        return analyze(fin);
    }
    return nullptr;
}

// std::shared_ptr<CodeAnalyzerBase::AnalysisResult>
// CodeAnalyzerBase::analyze(const std::wstring& path) {
//     std::wcout.imbue(std::locale(""));
//     wcout << path << endl;
//     init();
//     std::ifstream fin(path);
//     if (fin.is_open()) {
//         analysis_result_ptr->path = path;
//         return analyze(fin);
//     }
//     return nullptr;
// }

std::shared_ptr<CppCodeAnalyzer::AnalysisResult>
CppCodeAnalyzer::analyze(std::ifstream& fin) {
    std::string line{};
    size_t offset{};
    LineCategory line_category{};

    while (!(control_word & ControlWord::GETLINE) ||
           my_getline(fin, line, offset)) {
        // offset = find_first_not_blank(line, offset);

        control_word |= ControlWord::GETLINE;
        line_category = check_line(line, offset);
        set_line_category(line_begin, line_category);

        switch (line_category) {
        case LineCategory::CODE:
            if (!find_block_annotation_head(fin, line, offset))
                break;

        case LineCategory::BLOCK_ANNOTATION_HEAD:
            skip_block_annotation(fin, line, offset);
            if (offset != string::npos)
                control_word &= ~ControlWord::GETLINE;
            break;
        }
        // 好像这里不需要了
        // 因为一定是 end-begin=1的样子
        // 我们只需要检测是不是不需要读取 那么
        if (control_word & ControlWord::GETLINE)
            line_begin = line_end;
        // else
        //	line_begin = line_end - 1;
    }

    fin.close();
    return analysis_result_ptr;
}

void CppCodeAnalyzer::init() {
    // 着一些可以组织成一个init函数
    control_word = 0;
    control_word |= ControlWord::GETLINE;

    line_begin = 0;
    line_end = 0;

    analysis_result_ptr = std::make_shared<AnalysisResult>();
    // analysis_result_ptr->clear();
}

// 那么这样看来 offset传给这个函数是没用的啊
std::istream& CppCodeAnalyzer::my_getline(std::istream& fin, std::string& line,
                                          size_t& offset) {
    if (std::getline(fin, line)) {
        offset = 0;
        ++line_end;
        analysis_result_ptr->line_category.push_back(0);
    }
    return fin;
}

CppCodeAnalyzer::LineCategory
CppCodeAnalyzer::check_line(const std::string& line, size_t& offset) {
    if (line.empty() || (find_first_not_blank(line, offset) == string::npos))
        return LineCategory::BLANK;

    while (offset < line.size()) {
        switch (line[offset]) // 如果本行莫名其妙的结束了 那么判断为代码行就行了
        {
        case '/':
            if (line[offset + 1] == '/')
                return LineCategory::LINE_ANNOTATION;
            else if (line[offset + 1] ==
                     '*') { // 如果我们在本行开头找到了 /*
                            // 并不能说明这行是注释行 除非在本行找不到 */
                offset = line.find("*/", offset + 2);
                if (offset == string::npos)
                    // 如果没有找到 那么说明本行就是块注释的开头 都不用再找了
                    return LineCategory::BLOCK_ANNOTATION_HEAD;

                // 如果找到了 那么就说明本行的块注释结束了
                // 更新下标到块注释结束的位置 继续判断 相当于无视这个块注释
                if (find_first_not_blank(line, offset += 2) >= line.size())
                    return LineCategory::BLOCK_ANNOTATION;
            } else {
                return LineCategory::CODE;
            }
            break;

        default:
            return LineCategory::CODE;
        }
    }
    return LineCategory::CODE;
}

// 找不到的话返回 std::npos,从 offset开始寻找第一个非空字符
size_t CppCodeAnalyzer::find_first_not_blank(const std::string& line,
                                             size_t& offset) {
    for (; offset < line.size(); ++offset) {
        if (!isspace(line[offset]))
            return offset;
    }
    return offset = string::npos;
}

bool CppCodeAnalyzer::find_block_annotation_head(std::ifstream& fin,
                                                 std::string& line,
                                                 size_t& offset) {
    // 所有的offset均由skip函数管理
    for (; offset < line.size();) {
        switch (line[offset]) {
        case '\'':
            // skip_char_literal
            skip_char_literal(line, offset);
            break;

        case '"':
            // skip_string_literal
            skip_string_literal(fin, line, offset);
            break;

        case '/':
            // 这一部分要不抽象出一个函数来
            // 因为和
            // annotation
            if (line[offset + 1] == '/') {
                set_line_category(line_begin, LineCategory::LINE_ANNOTATION);
                return false;
            } else if (line[offset + 1] == '*') {
                offset = line.find("*/", offset + 2);
                if (offset == string::npos)
                    return true;
                else {
                    set_line_category(line_begin,
                                      LineCategory::BLOCK_ANNOTATION);
                    offset += 2;
                }
            } else {
                ++offset;
            }
            break;

        default:
            ++offset;
        }
    }
    return false;
}

void CppCodeAnalyzer::skip_char_literal(const std::string& line,
                                        size_t& offset) {
    auto my_offset = line.find('\'', offset + 1);
    if (my_offset != string::npos) {
        offset = my_offset;
        while (line[offset - 1] == '\\' &&
               line[offset - 2] != '\\') { // 这是一个转义' 忽视 再次查找 '
            offset = line.find('\'', offset + 1);
        }
    }
    ++offset;
}

void CppCodeAnalyzer::skip_string_literal(std::ifstream& fin, std::string& line,
                                          size_t& offset) {
    string raw_delimiter;
    switch (find_raw_literal_head(line, offset, raw_delimiter)) {
    case LineCategory::STRING_LITERAL:
        break;
    case LineCategory::RAW_LITERAL:
        return;
    case LineCategory::RAW_LITERAL_HEAD:
        // skip raw literal
        skip_raw_literal(fin, line, offset, raw_delimiter);
        return;
    };

    // 直接查看line最后一个字符是不是

    do {
        set_line_category(line_begin, LineCategory::CODE);
        size_t current_index = 0;
        size_t slash_size = 0;
        offset = line.find('"', offset + 1);
        // 在处理普通字符串的时候
        // 我们有可能碰到"在本行找不到配对的“
        // 必然是因为使用了\ 导致的
        // 所以只需要针对这种特殊情况进行特殊处理就可以了
        while (offset < line.size()) {
            current_index = offset - 1;
            slash_size = 0;

            while (line[current_index] == '\\') // 向前查看是否为 斜线
            {
                ++slash_size;    // 增加 \ 的数量
                --current_index; // 继续向前查看
            }
            // 最后统计出 \ 的数量之后 查看是奇数还是偶数
            if (slash_size % 2 == 0) // 偶数
            {
                // 如果有偶数个 那么他们互相匹配 这个"就和前面的匹配
                // 也就是说 字符串到这里就结束了 也就是 i 处字符串结束
                ++offset;
                return;
            } else // 如果是奇数  就说明这是一个转义序列
                   // 我们还需要从下一个位置再次查找 并且再次判断是不是正常结束
            {
                offset = line.find('"', offset + 1);
            }

            // 其实在这里也可能找不到配对的”
            // 所以我们必须要在一开始就判断是不是文本连接的字符串
        }
        // 如果退出来了 就说明在本行没有找到吧
        // 也就是说 在这里我们需要寻找下一行
        // 当前行的属性已经可以确定是代码行
        line_begin = line_end;
        // 然后读取下一行 继续在下一行寻找字符串的结尾
    } while (my_getline(fin, line, offset));

    // 因为文本一定是正确的 所以offset一定停在“上面
    // ++offset;
    // offset停留在"上或者npos
}

// 这个函数仅仅返回一个bool是不行的
// 他需要返回一个LineCategory
CppCodeAnalyzer::LineCategory
CppCodeAnalyzer::find_raw_literal_head(const std::string& line, size_t& offset,
                                       std::string& raw_delimiter) {
    if (offset > 0 && line[offset - 1] == 'R') {
        // 从当前位置开始 查找 (
        // R"de( ... )de";
        auto delimiter_begin = offset + 1; // i -> ", begin = d
        auto delimiter_end = line.find('(', delimiter_begin); // end -> （

        // 如果找不到那么表示这不是一个原生字符串
        if (delimiter_end == string::npos) {
            return LineCategory::STRING_LITERAL;
        }

        // 这可能是一个原生字符 因为原生字符可以换行
        // 所以我们需要不断查找直到找到结束符 首先获取结束符
        raw_delimiter.clear();
        raw_delimiter.push_back(')');

        // [begin,end) = de
        // 构建delimiter
        for (auto begin = delimiter_begin; begin < delimiter_end; ++begin) {
            auto c = line[begin];
            if (c == ' ' || c == '\\' || (iscntrl(c) != 0))
                return LineCategory::STRING_LITERAL;
            raw_delimiter.push_back(c);
        }
        raw_delimiter.push_back('"');

        auto raw_literal_end = line.find(raw_delimiter, delimiter_end + 1);
        if (raw_literal_end !=
            string::npos) { // 将offset更新到rawliteral尾部 跳过rawliteral
            offset = raw_literal_end + raw_delimiter.size();
            return LineCategory::RAW_LITERAL;
        } else {
            // 如果没有找到 完了 原生字符在本行没有结束 我们需要设置标志位
            // 更新外部函数来支持 is_raw_literal = true;
            return LineCategory::RAW_LITERAL_HEAD;
        }
    }
    return LineCategory::STRING_LITERAL;
}

void CppCodeAnalyzer::skip_raw_literal(std::ifstream& fin, std::string& line,
                                       size_t& offset,
                                       const std::string& raw_delimiter) {
    skip_until_find_delimiter(fin, line, offset, raw_delimiter,
                              LineCategory::CODE);
}

void CppCodeAnalyzer::skip_block_annotation(std::ifstream& fin,
                                            std::string& line, size_t& offset) {
    skip_until_find_delimiter(fin, line, offset, "*/",
                              LineCategory::BLOCK_ANNOTATION);
}

// 为了统一行为
// skip_until_find_delimiter会跳到delimiter的后一个位置
// 这可能是无效位置
void CppCodeAnalyzer::skip_until_find_delimiter(std::ifstream& fin,
                                                std::string& line,
                                                size_t& offset,
                                                const std::string& delimiter,
                                                LineCategory line_category) {
    do {
        offset = line.find(delimiter, offset);
        if (offset != string::npos) {
            offset += delimiter.size();
            break;
        }
    } while (my_getline(fin, line, offset));

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
        set_line_category(line_begin, line_category);
    offset = find_first_not_blank(line, offset);
    --line_begin;
}

// -------------------------------------------------------------------

std::string
CodeAnalyzerBase::line_category_to_string(LineCategory line_category) {
    stringstream context;
    if (line_category & LineCategory::BLANK) {
        context << "blank ";
    }
    if (line_category & LineCategory::BLOCK_ANNOTATION) {
        context << "block annotation ";
    }
    if (line_category & LineCategory::LINE_ANNOTATION) {
        context << "line annotation ";
    }
    if (line_category & LineCategory::CODE) {
        context << "code ";
    }
    return context.str();
}

std::shared_ptr<CodeAnalyzerBase> make_cpp_analyzer() {
    return std::make_shared<CppCodeAnalyzer>();
}

} // namespace stats
