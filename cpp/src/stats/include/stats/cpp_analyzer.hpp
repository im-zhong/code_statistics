// 2024/1/3
// zhangzhong

#pragma once

#include "code_analyzer.hpp"

namespace stats {

class CppAnalyzer : public CodeAnalyzer {

  public:
    CppAnalyzer() = default;

    virtual ~CppAnalyzer() = default;

  protected:
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t override;

    auto FindRawStringHead(const std::string_view& line, size_t offset)
        -> std::string;
};

auto MakeCppAnalyzer() -> std::shared_ptr<CppAnalyzer>;

} // namespace stats
