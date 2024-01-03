// 2024/1/3
// zhangzhong

#include "stats/code_analyzer.hpp"
#include <iostream>
#include <sstream>

namespace stats {

std::string LineCategoryToString(LineCategory line_category) {
    std::stringstream context;
    if (line_category & LineCategory::kBlank) {
        context << "blank ";
    }
    if (line_category & LineCategory::kBlockComment) {
        context << "block annotation ";
    }
    if (line_category & LineCategory::kLineComment) {
        context << "line annotation ";
    }
    if (line_category & LineCategory::kCode) {
        context << "code ";
    }
    return context.str();
}

std::string AnalysisResult::to_string() const {
    std::stringstream context;
    for (size_t i = 0; i < line_category.size(); i++) {
        context << i + 1 << " : "
                << LineCategoryToString(
                       static_cast<LineCategory>(line_category[i]))
                << std::endl;
    }
    context << "path: " << path << std::endl;
    context << "file lines: " << line_count << std::endl;
    context << "code lines: " << code_count << std::endl;
    context << "valid rate: " << code_count / static_cast<double>(line_count)
            << std::endl;
    return context.str();
}

void AnalysisResult::clear() noexcept {
    path.clear();
    line_count = 0;
    code_count = 0;
    blank_count = 0;
    annotation_count = 0;
    line_category.clear();
}

void AnalysisResult::statistics() noexcept {
    line_count = line_category.size();
    for (const auto& category : line_category) {
        if (static_cast<uint>(category) &
            static_cast<uint>(LineCategory::kBlank))
            ++blank_count;
        if (static_cast<uint>(category) &
            (static_cast<uint>(LineCategory::kBlockComment) |
             static_cast<uint>(LineCategory::kLineComment)))
            ++annotation_count;
        if (static_cast<uint>(category) &
            static_cast<uint>(LineCategory::kCode))
            ++code_count;
    }
}

} // namespace stats
