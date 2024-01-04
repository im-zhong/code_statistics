// 2024/1/4
// zhangzhong

#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace conf {

class Conf {
  public:
    const std::list<fs::path>& GetLoadPath() const noexcept;

    const std::list<std::string>& GetFileExtensions() const noexcept;

    void AddLoadPath(const fs::path& path) noexcept;

    auto AddLanguage(const std::string& language) -> bool;

  private:
    std::list<fs::path> load_paths_;
    std::list<std::string> extensions_;

    const std::map<std::string, std::vector<std::string>> language_extensions_ =
        {{"cpp", {".h", ".hpp", ".c", ".cpp", ".cc", ".cxx", ".hxx"}},
         {"rust", {".rs"}}};
};

std::shared_ptr<Conf> MakeConf();

} // namespace conf
