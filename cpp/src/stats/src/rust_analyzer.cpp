// 2023/4/5
// zhangzhong
// rust analyzer

#include "stats/rust_analyzer.hpp"
#include <memory>

namespace stats {

auto MakeRustAnalyzer() -> std::shared_ptr<RustAnalyzer> {
    return std::make_shared<RustAnalyzer>();
}

} // namespace stats
