/*
        2020/6/10
        测试项目

        2020/6/10 16：11 重构第一步：简化问题：现在开始，不考虑字符串
        2020/6/11 再次简化问题 仅考虑行注释
        2020/6/11 11：29 思考许久，做出重要决定：不考虑
   \ 了，为了微量的需求大幅修改设计或许也是应该放弃的吧
*/
/*
        2020/6/8 14:21
        有效代码统计工具重构工程
        zhong, ysu

        2020/6/8 23:17 暂停开发 思考优化一下代码统计函数的具体思路
        实在是太乱了

        2023/3/22 重构项目
*/

// #include "code_analysis.h"
// #include <iostream>

// #include "configuration.h"
// #include "global_code_analysis.h"
// using namespace std;

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "conf/conf.hpp"
#include "driver/driver.hpp"
#include <CLI/ConfigFwd.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

int main(int argc, const char* argv[]) {
    // std::wcout.imbue(std::locale(""));
    //// wcout << L"你好" << endl;
    // auto configuraton_ptr = make_configuration();
    // wcout << configuraton_ptr->to_string() << endl;

    ////configuraton_ptr->print(wcout);
    // auto analyzer = make_cpp_analyzer();
    //// 最简单的纯代码测试通过了
    //// 接下来就是给我的result写一个tostring就行了
    // auto result = analyzer->analyze(L"code_analysis.cpp");
    // result->statistics();
    // cout << result->to_string() << endl;

    // try {
    //     GlobalCodeAnalyzer analyzer;
    //     analyzer.run();
    // } catch (const std::exception& e) {
    //     cerr << e.what() << endl;
    //     ;
    // }

    CLI::App app{"A simple code statistics tool"};

    // positional option
    std::string path;
    app.add_option("path", path, "Path to analyze")->required();
    // 怎么描述这个是required

    // container options
    // -i path1 path2 path3 ...
    std::vector<std::string> ignored_paths;
    app.add_option("-i,--ignore", ignored_paths, "ignored paths");

    std::string language = "cpp";
    app.add_option(
        "-l,--language", language,
        "Language to analyze, only support cpp now, and default is cpp");

    try {
        // 在windows上不能这么用 好的 我也不在windows上开发
        app.parse(argc, argv);

        // std::cout << "path: " << path << std::endl;
        // std::cout << "language: " << language << std::endl;
        // for (const auto& ignored_path : ignored_paths) {
        //     std::cout << "ignored path: " << ignored_path << std::endl;
        // }

        auto conf = conf::make_configuration();
        conf->add_load_path(path);
        for (const auto& ignored_path : ignored_paths) {
            conf->add_invalid_path(ignored_path);
        }
        conf->add_extension();

        auto driver = driver::GlobalCodeAnalyzer(conf);
        driver.run();

    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
