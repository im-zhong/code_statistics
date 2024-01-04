// 2024/1/4
// zhangzhong

#include "conf/conf.hpp"
#include <string>

namespace conf {

std::shared_ptr<Conf> MakeConf() { return std::make_shared<Conf>(); }

const std::list<fs::path>& Conf::GetLoadPath() const noexcept {
    return load_paths_;
}

const std::list<std::string>& Conf::GetFileExtensions() const noexcept {
    return extensions_;
}

void Conf::AddLoadPath(const fs::path& path) noexcept {
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
