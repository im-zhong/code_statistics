// 2024/1/3
// zhangzhong

#pragma once

#include "stats/code_analyzer.hpp"
#include <memory>

namespace stats {

class RustAnalyzer : public CodeAnalyzer {
  public:
    RustAnalyzer();
    virtual ~RustAnalyzer() = default;
};

auto MakeRustAnalyzer() -> std::shared_ptr<RustAnalyzer>;

} // namespace stats
