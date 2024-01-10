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

//遍历conf_中的所有路径，分析每个路径所指向的文件
auto Driver::RunImpl() -> void {
    for (auto& path : conf_->GetLoadPaths()) {
        if (fs::is_directory(path)) {
            // fs::recursive_directory_iterator
            // 是一个迭代器，它可以递归地遍历一个目录及其所有子目录中的所有文件和子目录
            for (auto& entry : fs::recursive_directory_iterator(path)) {
                // fs::is_regular_file(entry) 检查 entry
                // 是否是一个常规文件（不是目录或其他特殊类型的文件）
                if (fs::exists(entry) && fs::is_regular_file(entry)) {
                    AnalyzePath(entry.path());
                }
            }
        } else if (fs::is_regular_file(path)) {
            AnalyzePath(path);
        }
    }
}

//分析path所指向的文件
auto Driver::AnalyzePath(const fs::path& path) -> void {
    // path.extension() 它返回 path 的扩展名     对于文件名
    // "example.txt"，扩展名是 ".txt"
    if (!FilterExtension(path.extension().string()))
        return;
    auto result = analyzer_->Analyze(path);
    results_->push_back(result);
    result->Statistics();
}

//判断文件的后缀名是否在extensions_中.
auto Driver::FilterExtension(std::string const& extension) -> bool {
    for (const auto& valid_extension : conf_->GetExtensions()) {
        if (extension == valid_extension)
            return true;
    }
    return false;
}

//打印结果
auto Driver::PrintResults() -> void {
    auto cpp_file_count = size_t{0};
    auto cpp_line_count = size_t{0};
    auto cpp_code_count = size_t{0};
    auto cpp_comment_count = size_t{0};
    auto python_file_count = size_t{0};
    auto python_line_count = size_t{0};
    auto python_code_count = size_t{0};
    auto python_comment_count = size_t{0};
    auto rust_file_count = size_t{0};
    auto rust_line_count = size_t{0};
    auto rust_code_count = size_t{0};
    auto rust_comment_count = size_t{0};
    for (const auto& result : *results_) {
        std::string extension = result->path.extension().string();
        if (extension == ".py") {
            ++python_file_count;
            python_line_count += result->line_count;
            python_code_count += result->code_count;
            python_comment_count += result->comment_count;
        } else if (extension == ".cpp" || extension == ".hpp" ||
                   extension == ".cc" || extension == ".h" ||
                   extension == ".cxx" || extension == ".hxx" ||
                   extension == ".c") {
            ++cpp_file_count;
            cpp_line_count += result->line_count;
            cpp_code_count += result->code_count;
            cpp_comment_count += result->comment_count;
        } else if (extension == ".rs") {
            ++rust_file_count;
            rust_line_count += result->line_count;
            rust_code_count += result->code_count;
            rust_comment_count += result->comment_count;
        }
    }
    if (cpp_line_count == 0) {
        fmt::println("no cpp files");
    } else {
        fmt::println(
            "\ncpp files: {}\ncpp lines: {}\ncpp codes: {} ({:.2f}%)\ncpp "
            "comments: {} ({:.2f}%)\n",
            cpp_file_count, cpp_line_count, cpp_code_count,
            static_cast<double>(cpp_code_count) / cpp_line_count * 100,
            cpp_comment_count,
            static_cast<double>(cpp_comment_count) / cpp_line_count * 100);
    }
    if (python_line_count == 0) {
        fmt::println("no python files");
    } else {
        fmt::println("python files: {}\npython lines: {}\npython codes: {} "
                     "({:.2f}%)\npython comments: {} "
                     "({:.2f}%)\n",
                     python_file_count, python_line_count, python_code_count,
                     static_cast<double>(python_code_count) /
                         python_line_count * 100,
                     python_comment_count,
                     static_cast<double>(python_comment_count) /
                         python_line_count * 100);
    }
    if (rust_line_count == 0) {
        fmt::println("no rust files");
    } else {
        fmt::println(
            "rust files: {}\nrust lines: {}\nrust codes: {} "
            "({:.2f}%)\nrust comments: {} "
            "({:.2f}%)",
            rust_file_count, rust_line_count, rust_code_count,
            static_cast<double>(rust_code_count) / rust_line_count * 100,
            rust_comment_count,
            static_cast<double>(rust_comment_count) / rust_line_count * 100);
    }
}

} // namespace driver
