#pragma once
/*
        专门用来加载配置文件
*/

// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/xml_parser.hpp>
#include <filesystem>

#include <fstream>
#include <list>
#include <string>

namespace fs = std::filesystem;
// namespace pt = boost::property_tree;

class Configuration {

public:
  Configuration(const std::wstring &configuration_filename)
      : _configuration_filename_(configuration_filename) {
    load_configuration(_configuration_filename_);
  }

  Configuration() : _configuration_filename_(L"configuration.xml") {
    load_configuration(_configuration_filename_);
  }

private:
  // 读取配置文件仅仅是基础
  // 更重要的是我们可以读取配置文件的信息
  // 因为配置信息是只读的 所以我们只需要提供一系列get函数即可

  void load_configuration(const std::wstring &configuration_filename);
  void load_configuration(std::wifstream &wfin);

  // 加载xml文件获取所有load-path 和 invalid-path 和 save-path

public:
  const std::wstring &get_configuration_filename() const noexcept {
    return _configuration_filename_;
  }
  const std::list<fs::path> &get_load_path() const noexcept {
    return _load_path_;
  }
  const std::list<fs::path> &get_debug_load_path() const noexcept {
    return _debug_load_path_;
  }
  const std::list<fs::path> &get_invalid_path() const noexcept {
    return _invalid_path_;
  }
  const std::list<fs::path> &get_save_path() const noexcept {
    return _save_path_;
  }
  const std::list<fs::path> &get_log_path() const noexcept {
    return _log_path_;
  }
  const std::list<std::wstring> &get_extension() const noexcept {
    return _extension_;
  }

  std::wstring to_string() const noexcept;
  // std::wostream& print(std::wostream& os) const noexcept;

  // 宽字符转换
  // static std::wstring utf8_to_wstring(const std::string& str)
  //{
  //	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
  //	return convert.from_bytes(str);
  //}

  // static std::string wstring_to_utf8(const std::wstring& str)
  //{
  //	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
  //	return convert.to_bytes(str);
  // }

private:
  // 其实这里用forward_list比vector好一点
  // 因为我们不需要索引信息
  // 可以有一个默认的配置路径
  std::wstring _configuration_filename_;
  std::list<fs::path> _load_path_;
  std::list<fs::path> _debug_load_path_;
  std::list<fs::path> _invalid_path_;
  std::list<fs::path> _save_path_;
  std::list<fs::path> _log_path_;
  std::list<std::wstring> _extension_;
};

std::shared_ptr<Configuration>
make_configuration(const std::wstring &configuration_filename);
std::shared_ptr<Configuration> make_configuration();
