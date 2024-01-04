// 2024/1/4
// zhangzhong

#include "driver/driver.hpp"

namespace driver {

void Driver::Run() {
    RunImpl();
    PrintResults();
}

void Driver::RunImpl() {
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

void Driver::AnalyzePath(const fs::path& path) {
    if (!FilterExtension(path.extension().string()))
        return;
    auto result = analyzer_->Analyze(path);
    results_->push_back(result);
    result->statistics();
}

bool Driver::FilterExtension(const std::string& extension) {
    for (const auto& valid_extension : conf_->GetExtensions()) {
        if (extension == valid_extension)
            return true;
    }
    return false;
}

void Driver::PrintResults() {
    size_t all_file_count = 0;
    size_t all_line_count = 0;
    size_t all_code_count = 0;
    for (const auto& result : *results_) {
        ++all_file_count;
        all_line_count += result->line_count;
        all_code_count += result->code_count;
    }
    std::cout << "\nfiles: " << all_file_count << std::endl;
    std::cout << "lines: " << all_line_count << std::endl;
    std::cout << "codes: " << all_code_count << std::endl;
}

} // namespace driver
