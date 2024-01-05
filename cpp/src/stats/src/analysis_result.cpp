// 2024/1/3
// zhangzhong

#include "stats/analysis_result.hpp"
#include <iostream>
#include <sstream>

namespace stats {

auto LineCategoryToString(LineCategory line_category) -> std::string {
    auto context = std::stringstream{};
    if (line_category == 0) {
        context << "blank ";
    }
    if (line_category & LineCategory::kBlockComment) {
        context << "block comment ";
    }
    if (line_category & LineCategory::kLineComment) {
        context << "line comment ";
    }
    if (line_category & LineCategory::kCode) {
        context << "code ";
    }
    return context.str();
}

auto AnalysisResult::ToString() const -> std::string {
    auto context = std::stringstream{};
    for (auto i = size_t{0}; i < line_categories.size(); ++i) {
        context << i + 1 << " : "
                << LineCategoryToString(
                       static_cast<LineCategory>(line_categories[i]))
                << std::endl;
    }
    context << "path: " << path << std::endl;
    context << "file lines: " << line_count << std::endl;
    context << "code lines: " << code_count << std::endl;
    context << "valid rate: " << code_count / static_cast<double>(line_count)
            << std::endl;
    return context.str();
}

auto AnalysisResult::Clear() noexcept -> void {
    path.clear();
    line_count = 0;
    code_count = 0;
    blank_count = 0;
    comment_count = 0;
    line_categories.clear();
}

auto AnalysisResult::Statistics() noexcept -> void {
    line_count = line_categories.size();
    for (const auto& category : line_categories) {
        if (category == 0)
            ++blank_count;
        if (static_cast<uint>(category) &
            (static_cast<uint>(LineCategory::kBlockComment) |
             static_cast<uint>(LineCategory::kLineComment)))
            ++comment_count;
        if (static_cast<uint>(category) &
            static_cast<uint>(LineCategory::kCode))
            ++code_count;
    }
}

} // namespace stats
