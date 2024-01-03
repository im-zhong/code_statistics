// 2024/1/2
// zhangzhong

#pragma once

#include <cstddef>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace stats {

namespace fs = std::filesystem;

enum LineCategory : uint16_t {
    kBlank = 0x01,
    kCode = 0x02,
    kLineComment = 0x04,

    kBlockComment = 0x08,
    kBlockCommentHead = 0x10,
    kBlockCommentTail = 0x20,

    // string为什么要区分呢 不都是代码吗？
    // kRawLiteral = 0x20,
    // kRawLiteralHead = 0x40,
    // kStringLiteral = 0x80,
    kRawString = 0x40,
    kString = 0x80,
};

std::string LineCategoryToString(LineCategory line_category);

struct AnalysisResult {
    fs::path path;
    size_t line_count{};
    size_t code_count{};
    size_t blank_count{};
    size_t annotation_count{};
    std::vector<uint8_t> line_category;

    std::string to_string() const;
    void clear() noexcept;
    void statistics() noexcept;
};

class CodeAnalyzer {
  public:
    CodeAnalyzer(const std::string& line_comment_head = "//",
                 const std::string& block_comment_head = "/*",
                 const std::string& block_comment_tail = "*/")
        : analysis_result_(std::make_shared<AnalysisResult>()), control_word(0),
          line_begin(0), line_end(0), line_comment_head_{line_comment_head},
          block_comment_head_{block_comment_head},
          block_comment_tail_{block_comment_tail} {}

    enum ControlWord : uint8_t {
        GETLINE = 0x1, // 表示本次需要调用getline
    };

    std::shared_ptr<AnalysisResult> Analyze(const std::string& path);

    virtual std::shared_ptr<AnalysisResult> AnalyzeFile(std::istream& is);

    // 还需要定义分析结果类
    // 对当前文件进行分析的结果全部保存到此结构中
    // TODO: 其实Char和String可以合并处理 反而能兼容更多语言
    // 也就是我们不在区分char和string
    // virtual auto IsCharHead(const std::string& line, size_t offset) -> bool;
    virtual auto IsStringHead(const std::string_view& line, size_t offset)
        -> bool;
    // TODO: 把string和raw string的处理分开
    virtual auto IsRawStringHead(const std::string_view& line, size_t offset)
        -> bool;
    virtual auto IsLineCommentHead(const std::string_view& line, size_t offset)
        -> bool;
    virtual auto IsBlockCommentHead(const std::string_view& line, size_t offset)
        -> bool;

  protected:
    void SetLineCategory(size_t line, LineCategory category);

    virtual auto Init() -> void;

  protected:
    auto MyGetline(std::istream& is, std::string& line) -> std::istream&;

    auto FindFirstNotBlank(const std::string& line, size_t offset) -> size_t;

    auto SkipLineComment(std::istream& is, std::string& line, size_t offset)
        -> size_t;

    auto SkipString(std::istream& is, std::string& line, size_t offset)
        -> size_t;

    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t;

    auto SkipBlockComment(std::istream& is, std::string& line, size_t offset)
        -> size_t;

    auto SkipUntilFindDelimiter(std::istream& is, std::string& line,
                                size_t offset,
                                const std::string_view& delimiter,
                                LineCategory line_category) -> size_t;

  protected:
    std::shared_ptr<AnalysisResult> analysis_result_;
    uint32_t control_word = 0u;
    uint32_t line_begin;
    uint32_t line_end; // 这些在处理块注释的时候还是有用的

    std::string line_comment_head_;
    std::string block_comment_head_;
    std::string block_comment_tail_;
    // 那这样的话咱就不需要这两个东西了
    // 一般的语言的head都是 ' or "
    // std::string char_head_;
    // std::string string_head_;
    // std::string raw_string_head_;
    // std::string_view raw_string_delimiter_{"R\""};
};

} // namespace stats
