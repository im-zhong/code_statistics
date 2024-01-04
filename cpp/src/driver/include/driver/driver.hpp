#pragma once

#include "conf/conf.hpp"
#include "stats/code_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include <list>
#include <memory>

namespace driver {
using namespace conf;
using namespace stats;

class Driver {
  public:
    Driver(std::shared_ptr<conf::Conf> conf,
           std::shared_ptr<stats::CodeAnalyzer> analyzer)
        : conf_(conf), analyzer_(analyzer),
          results_(std::make_shared<GlobalAnalysisResult>()) {}

    using GlobalAnalysisResult = std::list<std::shared_ptr<AnalysisResult>>;

    void Run();

  private:
    void Analyze();

    void AnalyzePath(const fs::path& path);

    void PrintResults();

    bool FilterExtension(const std::string& extension);

  private:
    std::shared_ptr<Conf> conf_;
    std::shared_ptr<CodeAnalyzer> analyzer_;
    std::shared_ptr<GlobalAnalysisResult> results_;
};

} // namespace driver
