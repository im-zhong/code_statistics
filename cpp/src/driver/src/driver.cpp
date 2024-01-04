// 2023/03/22

#include "driver/driver.hpp"

namespace driver {

bool Driver::FilterExtension(const std::string& extension) {
    for (const auto& valid_extension : conf_->GetFileExtensions()) {
        if (extension == valid_extension)
            return true;
    }
    return false;
}

void Driver::Run() {
    try {
        Analyze();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    PrintResults();
}

void Driver::PrintResults() {
    // 统计总的代码行 和 注释行
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

void Driver::Analyze() {
    for (auto& path : conf_->GetLoadPath()) {
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

} // namespace driver
