#pragma once
#pragma once

#include <filesystem>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/xml_parser.hpp>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
// namespace pt = boost::property_tree;

// 更名为代码分析
// 统计代码类
// 类使用文件名作为接口 一次仅统计个文件
// 在外部可以使用文件系统filesystem不断调用此模块
class CodeAnalyzerBase {
  public:
    CodeAnalyzerBase()
        : analysis_result_ptr(nullptr), control_word(0), line_begin(0),
          line_end(0) {}

    enum ControlWord : uint8_t {
        GETLINE = 0x1, // 表示本次需要调用getline
    };

    enum LineCategory : uint8_t {
        BLANK = 0x01,
        LINE_ANNOTATION = 0x02,
        BLOCK_ANNOTATION = 0x04,
        CODE = 0x08,

        BLOCK_ANNOTATION_HEAD = 0x10,
        RAW_LITERAL = 0x20,
        RAW_LITERAL_HEAD = 0x40,
        STRING_LITERAL = 0x80,
    };

    static std::string line_category_to_string(LineCategory line_category);

    // 还需要定义分析结果类
    // 对当前文件进行分析的结果全部保存到此结构中
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

    virtual std::shared_ptr<AnalysisResult> analyze(const std::string& path);
    virtual std::shared_ptr<AnalysisResult> analyze(const std::wstring& path);

  protected:
    void set_line_category(size_t line, LineCategory category) {
        analysis_result_ptr->line_category[line] |= category;
    }

    virtual void init(){};
    virtual std::shared_ptr<AnalysisResult> analyze(std::ifstream& fin) = 0;

  protected:
    std::shared_ptr<AnalysisResult> analysis_result_ptr;
    uint32_t control_word = 0u;
    uint32_t line_begin;
    uint32_t line_end; // 这些在处理块注释的时候还是有用的
};

class CppCodeAnalyzer : public CodeAnalyzerBase {
  protected:
    virtual std::shared_ptr<AnalysisResult>
    analyze(std::ifstream& fin) override;
    virtual void init() override;

  private:
    LineCategory check_line(const std::string& line, size_t& offset);
    std::istream& my_getline(std::istream& fin, std::string& line,
                             size_t& offset);

    size_t find_first_not_blank(const std::string& _Str, size_t& offset);
    bool find_block_annotation_head(std::ifstream& fin, std::string& line,
                                    size_t& offset);
    LineCategory find_raw_literal_head(const std::string& line, size_t& offset,
                                       std::string& raw_delimiter);

    void skip_char_literal(const std::string& line, size_t& offset);
    void skip_string_literal(std::ifstream& fin, std::string& line,
                             size_t& offset);
    void skip_raw_literal(std::ifstream& fin, std::string& line, size_t& offset,
                          const std::string& raw_delimiter);
    void skip_block_annotation(std::ifstream& fin, std::string& line,
                               size_t& offset);
    void skip_until_find_delimiter(std::ifstream& fin, std::string& line,
                                   size_t& offset, const std::string& delimiter,
                                   LineCategory line_category);
};

std::shared_ptr<CodeAnalyzerBase> make_cpp_analyzer();
