// 2024/1/4
// zhangzhong

#pragma once

#include "conf/conf.hpp"
#include "stats/code_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include "stats/rust_analyzer.hpp"
#include <list>
#include <memory>

namespace driver {

class Driver {
  public:
    using GlobalAnalysisResult =
        std::list<std::shared_ptr<stats::AnalysisResult>>;

    Driver(std::shared_ptr<conf::Conf> conf,
           std::shared_ptr<stats::CodeAnalyzer> analyzer);
    auto Run() -> void;

  private:
    auto RunImpl() -> void;
    auto AnalyzePath(const fs::path& path) -> void;
    auto PrintResults() -> void;
    auto FilterExtension(std::string const& extension) -> bool;

  private:
    std::shared_ptr<conf::Conf> conf_;
    std::shared_ptr<stats::CodeAnalyzer> analyzer_;
    std::shared_ptr<GlobalAnalysisResult> results_;
    std::shared_ptr<stats::PythonAnalyzer> python_analyzer_;
    std::shared_ptr<stats::CppAnalyzer> cpp_analyzer_;
    std::shared_ptr<stats::RustAnalyzer> rust_analyzer_;
};

} // namespace driver
