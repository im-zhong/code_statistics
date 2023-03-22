#include "configuration.h"
#include <codecvt>
#include <iostream>
#include <locale>

using namespace std;

void Configuration::load_configuration(
    const std::wstring& configuration_filename) {
    try {
        wifstream wfin(configuration_filename);
        if (wfin.is_open()) {
            load_configuration(wfin);
            wfin.close();
        } else {
            wcerr << configuration_filename << " can not open!" << endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Configuration::load_configuration(std::wifstream& wfin) {
    try {
        // 先转换一下
        std::locale locale_utf8(std::locale(), new std::codecvt_utf8<wchar_t>);
        wfin.imbue(locale_utf8);

        pt::wptree wtree;
        pt::read_xml(wfin, wtree);

        for (const auto& var : wtree.get_child(L"load-path")) {
            if (var.first == L"path") {
                _load_path_.push_back(var.second.data());
            }
        }
        for (const auto& var : wtree.get_child(L"debug-load-path")) {
            if (var.first == L"path") {
                _debug_load_path_.push_back(var.second.data());
            }
        }
        for (const auto& var : wtree.get_child(L"invalid-path")) {
            if (var.first == L"path") {
                _invalid_path_.push_back(var.second.data());
            }
        }
        for (const auto& var : wtree.get_child(L"save-path")) {
            if (var.first == L"path") {
                _save_path_.push_back(var.second.data());
            }
        }
        for (const auto& var : wtree.get_child(L"extension")) {
            if (var.first == L"path") {
                _extension_.push_back(var.second.data());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::wstring Configuration::to_string() const noexcept {
    // 把所有内容都输出出来就可以了
    wstring context;
    context += (_configuration_filename_ + L"\n");
    context += L"load path:\n";
    for (const auto& path : _load_path_) {
        context += (L"\t" + path.wstring() + L"\n");
    }
    context += L"debug load path:\n";
    for (const auto& path : _debug_load_path_) {
        context += (L"\t" + path.wstring() + L"\n");
    }
    context += L"invalid path:\n";
    for (const auto& path : _invalid_path_) {
        context += (L"\t" + path.wstring() + L"\n");
    }
    context += L"save path:\n";
    for (const auto& path : _save_path_) {
        context += (L"\t" + path.wstring() + L"\n");
    }
    context += L"log path:\n";
    for (const auto& path : _log_path_) {
        context += (L"\t" + path.wstring() + L"\n");
    }
    context += L"extension:\n";
    for (const auto& path : _extension_) {
        context += (L"\t" + path + L"\n");
    }
    return context;
}
//
// std::wostream& Configuration::print(std::wostream& os) const noexcept
//{
//
//	// os << (_configuration_filename_ + "\n");
//	os << "load path:\n";
//	for (const auto& path : _load_path_)
//	{
//		os << (L"\t" + path.wstring() + L"\n");
//	}
//	os << "debug load path:\n";
//	for (const auto& path : _debug_load_path_)
//	{
//		os << (L"\t" + path.wstring() + L"\n");
//	}
//	os << "invalid path:\n";
//	for (const auto& path : _invalid_path_)
//	{
//		os << (L"\t" + path.wstring() + L"\n");
//	}
//	os << "save path:\n";
//	for (const auto& path : _save_path_)
//	{
//		os << (L"\t" + path.wstring() + L"\n");
//	}
//	os << "log path:\n";
//	for (const auto& path : _log_path_)
//	{
//		os << (L"\t" + path.wstring() + L"\n");
//	}
//	return os;
//}

std::shared_ptr<Configuration>
make_configuration(const std::wstring& configuration_filename) {
    return std::make_shared<Configuration>(configuration_filename);
}

std::shared_ptr<Configuration> make_configuration() {
    return std::make_shared<Configuration>();
}
