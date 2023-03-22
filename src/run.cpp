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

#include "code_analysis.h"
#include <iostream>

#include "configuration.h"
#include "global_code_analysis.h"
using namespace std;

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

    try {
        GlobalCodeAnalyzer analyzer;
        analyzer.run();
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        ;
    }
}
