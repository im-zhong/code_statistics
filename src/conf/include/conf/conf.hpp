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
    auto GetLoadPaths() const noexcept -> const std::list<fs::path>&;
    auto GetExtensions() const noexcept -> const std::list<std::string>&;
    auto AddLoadPath(const fs::path& path) -> void;
    auto AddLanguage(std::string const& language) -> bool;

  private:
    std::list<fs::path> load_paths_;
    std::list<std::string> extensions_;

    const std::map<std::string, std::vector<std::string>> language_extensions_ =
        {{"cpp", {".h", ".hpp", ".c", ".cpp", ".cc", ".cxx", ".hxx"}},
         {"rust", {".rs"}},
         {"python", {".py"}},
         {"all",{".h", ".hpp", ".c", ".cpp", ".cc", ".cxx", ".hxx", ".rs", ".py"}}};
};

auto MakeConf() -> std::shared_ptr<Conf>;

} // namespace conf
