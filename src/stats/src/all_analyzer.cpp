

#include "stats/all_analyzer.hpp"
#include "stats/analysis_result.hpp"
#include "stats/code_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include "stats/rust_analyzer.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <string>

namespace stats {

AllAnalyzer::AllAnalyzer()
    : CodeAnalyzer(), python_analyzer_{MakePythonAnalyzer()},
      cpp_analyzer_{MakeCppAnalyzer()}, rust_analyzer_{MakeRustAnalyzer()} {}

auto AllAnalyzer::GetAnalyzer(std::string const& path)
    -> std::shared_ptr<CodeAnalyzer> {
    std::filesystem::path fs_path(path);
    std::string extension = fs_path.extension().string();
    if (extension == ".py") {
        return python_analyzer_;
    } else if (extension == ".cpp" || extension == ".hpp" ||
               extension == ".cc" || extension == ".h" || extension == ".cxx" ||
               extension == ".hxx" || extension == ".c") {
        return cpp_analyzer_;
    } else if (extension == ".rs") {
        return rust_analyzer_;
    } else {
        return nullptr;
    }
}

auto AllAnalyzer::Analyze(std::string const& path)
    -> std::shared_ptr<AnalysisResult> {
    fmt::println("stats... {}", path);
    auto analyzer = GetAnalyzer(path);
    analyzer->Init();
    auto fin = std::ifstream{path};
    if (fin.is_open()) {
        analyzer->result_->path = path;
        auto result = analyzer->AnalyzeFile(fin);
        fin.close();
        return result;
    }
    return nullptr;
}

auto MakeAllAnalyzer() -> std::shared_ptr<AllAnalyzer> {
    return std::make_shared<AllAnalyzer>();
}

} // namespace stats
