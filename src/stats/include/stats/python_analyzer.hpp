// 2024/1/5
// zhangzhong

#pragma once

#include "stats/code_analyzer.hpp"

namespace stats {

class PythonAnalyzer : public CodeAnalyzer {
  public:
    PythonAnalyzer();
    virtual ~PythonAnalyzer() = default;

  private:
    virtual auto IsStringHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsRawStringHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsBlockCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t override;
    virtual auto SkipBlockComment(std::istream& is, std::string& line,
                                  size_t offset) -> size_t override;
};

auto MakePythonAnalyzer() -> std::shared_ptr<PythonAnalyzer>;

} // namespace stats
