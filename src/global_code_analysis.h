#pragma once

#include "code_analysis.h"
#include "configuration.h"
#include <list>

class GlobalCodeAnalyzer {
public:
  GlobalCodeAnalyzer()
      : _configuration_ptr_(make_configuration()),
        _analyzer_(make_cpp_analyzer()), _global_result_ptr_(nullptr) {
    _global_result_ptr_ = std::make_shared<GlobalAnalysisResult>();
  }

  using GlobalAnalysisResult =
      std::list<std::shared_ptr<CodeAnalyzerBase::AnalysisResult>>;

  // std::wstring _configuration_filename_;
  std::shared_ptr<Configuration> _configuration_ptr_;
  std::shared_ptr<CodeAnalyzerBase> _analyzer_;
  std::shared_ptr<GlobalAnalysisResult> _global_result_ptr_;

  // 可以通过过滤器的可以继续执行
  bool path_filter(const fs::path &path) {
    for (const auto &invalid_path : _configuration_ptr_->get_invalid_path()) {
      if (path == invalid_path)
        return false;
    }
    return true;
  }

  // 这个也可以变成扩展
  bool extension_filter(const std::wstring &extension) {
    for (const auto &valid_extension : _configuration_ptr_->get_extension()) {
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
    for (const auto &result : *_global_result_ptr_) {
      // result->statistics();
      // std::cout << result->to_string() << std::endl;
      ++all_file_count;
      all_line_count += result->line_count;
      all_code_count += result->code_count;
    }
    cout << "all file count: " << all_file_count << endl;
    cout << "all line count: " << all_line_count << endl;
    cout << "all code count: " << all_code_count << endl;
    cout << "valid code rate: "
         << static_cast<double>(all_code_count) / all_line_count << endl;
  }

  void global_analyze() {
    try {
      for (auto &path : _configuration_ptr_->get_load_path()) {
        // path需要过滤
        if (!path_filter(path))
          continue;

        if (fs::is_directory(path)) {
          auto recursive_directory = fs::recursive_directory_iterator(path);
          for (auto &entry : recursive_directory) {
            if (!path_filter(entry.path().parent_path())) {
              recursive_directory.pop();
              continue;
            }

            if (fs::exists(entry) && fs::is_regular_file(entry)) {
              if (!path_filter(entry.path()) ||
                  !path_filter(entry.path().filename().wstring()) ||
                  !extension_filter(entry.path().extension().wstring()))
                continue;

              auto analysis_result =
                  _analyzer_->analyze(entry.path().wstring());
              _global_result_ptr_->push_back(analysis_result);
              analysis_result->statistics();
              std::cout << analysis_result->to_string() << std::endl;
            } else {
              // 异常
            }
          }
        } else if (fs::is_regular_file(path)) {
          if (!path_filter(path) || !path_filter(path.filename().wstring()) ||
              !extension_filter(path.extension().wstring()))
            continue;

          auto analysis_result = _analyzer_->analyze(path.wstring());
          _global_result_ptr_->push_back(analysis_result);
          analysis_result->statistics();
          std::cout << analysis_result->to_string() << std::endl;
        } else {
          // 异常
        }
      }
    } catch (const std::exception &e) {
    }
  }
};