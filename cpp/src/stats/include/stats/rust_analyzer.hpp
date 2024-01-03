// 2024/1/3
// zhangzhong

#include "stats/code_analyzer.hpp"
#include <memory>

namespace stats {

class RustAnalyzer : public CodeAnalyzer {};

auto MakeRustAnalyzer() -> std::shared_ptr<RustAnalyzer>;

} // namespace stats
