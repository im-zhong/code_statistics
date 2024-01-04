// 2024/1/4
// zhangzhong

#include "conf/conf.hpp"
#include <string>

namespace conf {

auto MakeConf() -> std::shared_ptr<Conf> { return std::make_shared<Conf>(); }

auto Conf::GetLoadPaths() const noexcept -> const std::list<fs::path>& {
    return load_paths_;
}

auto Conf::GetExtensions() const noexcept -> const std::list<std::string>& {
    return extensions_;
}

auto Conf::AddLoadPath(const fs::path& path) -> void {
    load_paths_.push_back(path);
}

auto Conf::AddLanguage(const std::string& language) -> bool {
    if (language_extensions_.find(language) == language_extensions_.end()) {
        return false;
    }

    for (const auto& extension : language_extensions_.at(language)) {
        extensions_.push_back(extension);
    }
    return true;
}

} // namespace conf
