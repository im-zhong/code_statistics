// 2023/4/5
// zhangzhong
// rust analyzer

#include "stats/code_analysis.hpp"

namespace stats {
void RustCodeAnalyzer::init() {}

std::shared_ptr<CodeAnalyzerBase::AnalysisResult>
RustCodeAnalyzer::analyze(std::ifstream& fin) {

    return {};
}

std::shared_ptr<CodeAnalyzerBase> make_rust_analyzer() {
    return make_cpp_analyzer();
}

} // namespace stats
