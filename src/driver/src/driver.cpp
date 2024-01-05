// 2024/1/4
// zhangzhong

#include "driver/driver.hpp"
#include <fmt/core.h>

namespace driver {

Driver::Driver(std::shared_ptr<conf::Conf> conf,
               std::shared_ptr<stats::CodeAnalyzer> analyzer)
    : conf_(conf), analyzer_(analyzer),
      results_(std::make_shared<GlobalAnalysisResult>()) {}

auto Driver::Run() -> void {
    RunImpl();
    PrintResults();
}

auto Driver::RunImpl() -> void {
    for (auto& path : conf_->GetLoadPaths()) {
        if (fs::is_directory(path)) {
            for (auto& entry : fs::recursive_directory_iterator(path)) {
                if (fs::exists(entry) && fs::is_regular_file(entry)) {
                    AnalyzePath(entry.path());
                }
            }
        } else if (fs::is_regular_file(path)) {
            AnalyzePath(path);
        }
    }
}

auto Driver::AnalyzePath(const fs::path& path) -> void {
    if (!FilterExtension(path.extension().string()))
        return;
    auto result = analyzer_->Analyze(path);
    results_->push_back(result);
    result->Statistics();
}

auto Driver::FilterExtension(std::string const& extension) -> bool {
    for (const auto& valid_extension : conf_->GetExtensions()) {
        if (extension == valid_extension)
            return true;
    }
    return false;
}

auto Driver::PrintResults() -> void {
    auto all_file_count = size_t{0};
    auto all_line_count = size_t{0};
    auto all_code_count = size_t{0};
    auto all_comment_count = size_t{0};
    for (const auto& result : *results_) {
        ++all_file_count;
        all_line_count += result->line_count;
        all_code_count += result->code_count;
        all_comment_count += result->comment_count;
    }
    fmt::println(
        "\nfiles: {}\nlines: {}\ncodes: {} ({:.2f}%)\ncomments: {} ({:.2f}%)",
        all_file_count, all_line_count, all_code_count,
        static_cast<double>(all_code_count) / all_line_count * 100,
        all_comment_count,
        static_cast<double>(all_comment_count) / all_line_count * 100);
}

} // namespace driver
