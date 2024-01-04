// 2024/1/4
// zhangzhong

#pragma once

#include "conf/conf.hpp"
#include "stats/code_analyzer.hpp"
#include <list>
#include <memory>

namespace driver {

class Driver {
  public:
    using GlobalAnalysisResult =
        std::list<std::shared_ptr<stats::AnalysisResult>>;

    Driver(std::shared_ptr<conf::Conf> conf,
           std::shared_ptr<stats::CodeAnalyzer> analyzer)
        : conf_(conf), analyzer_(analyzer),
          results_(std::make_shared<GlobalAnalysisResult>()) {}

    void Run();

  private:
    void RunImpl();
    void AnalyzePath(const fs::path& path);
    void PrintResults();
    bool FilterExtension(const std::string& extension);

  private:
    std::shared_ptr<conf::Conf> conf_;
    std::shared_ptr<stats::CodeAnalyzer> analyzer_;
    std::shared_ptr<GlobalAnalysisResult> results_;
};

} // namespace driver
