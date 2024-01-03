#pragma once

#include "conf/conf.hpp"
#include "stats/code_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include <list>
#include <memory>

namespace driver {
using namespace conf;
using namespace stats;
class GlobalCodeAnalyzer {

  public:
    GlobalCodeAnalyzer()
        : configuration_ptr_{make_configuration()},
          analyzer_{MakeCppAnalyzer()}, global_result_ptr_{nullptr} {
        global_result_ptr_ = std::make_shared<GlobalAnalysisResult>();
    }

    GlobalCodeAnalyzer(std::shared_ptr<conf::Configuration> conf)
        : configuration_ptr_(conf), analyzer_(MakeCppAnalyzer()),
          global_result_ptr_(nullptr) {
        global_result_ptr_ = std::make_shared<GlobalAnalysisResult>();
    }

    using GlobalAnalysisResult = std::list<std::shared_ptr<AnalysisResult>>;

    // std::wstring _configuration_filename_;
    std::shared_ptr<Configuration> configuration_ptr_;
    std::shared_ptr<CodeAnalyzer> analyzer_;
    std::shared_ptr<GlobalAnalysisResult> global_result_ptr_;

    // 可以通过过滤器的可以继续执行
    bool path_filter(const fs::path& path) {
        for (const auto& invalid_path :
             configuration_ptr_->get_invalid_path()) {
            if (path == invalid_path)
                return false;
        }
        return true;
    }

    // 这个也可以变成扩展
    bool extension_filter(const std::string& extension) {
        for (const auto& valid_extension :
             configuration_ptr_->get_extension()) {
            if (extension == valid_extension)
                return true;
        }
        return false;
    }

    // 需要配置文件的支持
    // 应该在配置文件中选择文件扩展名 这样就可以分析不同的文件了
    // 根据路径创建遍历

    // 在main函数中调用
    void run() {
        using namespace std;
        // 读取文件应该用这个类
        // std::wifstream
        global_analyze();
        // 使用global_result 输出日志
        // for (const auto& result : *_global_result_ptr_)
        //{
        //	result->statistics();
        //	std::cout << result->to_string() << std::endl;
        //}

        // 统计总的代码行 和 注释行
        size_t all_file_count = 0;
        size_t all_line_count = 0;
        size_t all_code_count = 0;
        for (const auto& result : *global_result_ptr_) {
            // result->statistics();
            // std::cout << result->to_string() << std::endl;
            ++all_file_count;
            all_line_count += result->line_count;
            all_code_count += result->code_count;
        }
        cout << "\nfiles: " << all_file_count << endl;
        cout << "lines: " << all_line_count << endl;
        cout << "codes: " << all_code_count << endl;
        // cout << "rate: " << static_cast<double>(all_code_count) /
        // all_line_count
        //      << endl;
    }

    void global_analyze() {
        try {
            for (auto& path : configuration_ptr_->get_load_path()) {
                // path需要过滤
                if (!path_filter(path))
                    continue;

                if (fs::is_directory(path)) {
                    auto recursive_directory =
                        fs::recursive_directory_iterator(path);
                    for (auto& entry : recursive_directory) {
                        if (!path_filter(entry.path().parent_path())) {
                            recursive_directory.pop();
                            continue;
                        }

                        if (fs::exists(entry) && fs::is_regular_file(entry)) {
                            // std::cout << entry.path().string() << std::endl;
                            if (!path_filter(entry.path()) ||
                                !path_filter(
                                    entry.path().filename().string()) ||
                                !extension_filter(
                                    entry.path().extension().string())) {
                                // if (!path_filter(entry.path())) {
                                //     std::cout << "path_filter" << std::endl;
                                // }
                                // if (!path_filter(
                                //         entry.path().filename().string())) {
                                //     std::cout << "filename_filter" <<
                                //     std::endl;
                                // }
                                // if (!extension_filter(
                                //         entry.path().extension().string())) {
                                //     std::cout << "extension_filter"
                                //               << std::endl;
                                // }

                                // std::cout << "continue" << std::endl;
                                continue;
                            }

                            auto analysis_result =
                                analyzer_->Analyze(entry.path().string());
                            global_result_ptr_->push_back(analysis_result);
                            analysis_result->statistics();
                            // std::cout << analysis_result->to_string()
                            //           << std::endl;
                        } else {
                            // 异常
                        }
                    }
                } else if (fs::is_regular_file(path)) {
                    if (!path_filter(path) ||
                        !path_filter(path.filename().string()) ||
                        !extension_filter(path.extension().string()))
                        continue;

                    auto analysis_result = analyzer_->Analyze(path.string());
                    global_result_ptr_->push_back(analysis_result);
                    analysis_result->statistics();
                    // std::cout << analysis_result->to_string() << std::endl;
                } else {
                    // 异常
                }
            }
        } catch (const std::exception& e) {
        }
    }
};
} // namespace driver
