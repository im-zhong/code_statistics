// 2024/1/3
// zhangzhong

#pragma once

#include "code_analyzer.hpp"

namespace stats {

class TSAnalyzer : public CodeAnalyzer {

  public:
    TSAnalyzer();
    virtual ~TSAnalyzer() = default;

  private:
    virtual auto IsLineCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsBlockCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t override;
    auto GetRawStringTail(std::string_view const& line, size_t offset)
        -> std::string;
};

auto MakeTSAnalyzer() -> std::shared_ptr<TSAnalyzer>;

} // namespace stats
