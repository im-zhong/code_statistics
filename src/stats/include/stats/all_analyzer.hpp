

#pragma once

#include "code_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include "stats/rust_analyzer.hpp"
#include <memory>

namespace stats {

class AllAnalyzer : public CodeAnalyzer {
  private:
    std::shared_ptr<PythonAnalyzer> python_analyzer_;
    std::shared_ptr<CppAnalyzer> cpp_analyzer_;
    std::shared_ptr<RustAnalyzer> rust_analyzer_;

  public:
    AllAnalyzer();
    auto GetAnalyzer(std::string const& path) -> std::shared_ptr<CodeAnalyzer>;
    virtual auto Analyze(std::string const& path)
        -> std::shared_ptr<AnalysisResult> override;
};

auto MakeAllAnalyzer() -> std::shared_ptr<AllAnalyzer>;

} // namespace stats