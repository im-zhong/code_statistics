// 2023/4/5
// zhangzhong

#include "stats/rust_analyzer.hpp"
#include <memory>

namespace stats {

RustAnalyzer::RustAnalyzer() : CodeAnalyzer("//", "/*", "*/") {}

auto MakeRustAnalyzer() -> std::shared_ptr<RustAnalyzer> {
    return std::make_shared<RustAnalyzer>();
}

} // namespace stats
