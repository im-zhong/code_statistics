// 2024/1/5
// zhangzhong

#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace stats {

enum LineCategory : uint8_t {
    kBlank = 0x00,
    kCode = 0x01,
    kLineComment = 0x02,
    kBlockComment = 0x04,
};

auto LineCategoryToString(LineCategory line_category) -> std::string;

struct AnalysisResult {
    fs::path path;
    size_t line_count{};
    size_t code_count{};
    size_t blank_count{};
    size_t comment_count{};
    std::vector<uint8_t> line_categories;

    auto ToString() const -> std::string;
    auto Clear() noexcept -> void;
    auto Statistics() noexcept -> void;
};

} // namespace stats
