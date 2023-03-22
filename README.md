# code_statistics
Code Line Statistic Tool

# 设计cli
code_statistics 
    -l,--language cpp,java,python,.. default is cpp
    # -e,--extension 'regex' 不行这个不容易实现 而且我不知道你是什么语言呀 就给定语言有某种具体的后缀就行了
    每种语言都有对应的后缀 cpp: .h .c .hpp .cpp
    -i,--ignore path1,path2, 'regex' 只要符合regex的文件路径 都忽略
    <path> 必须给定一个开始搜索的路径 程序向递归的搜索此文件夹内部的所有符合条件的文件 并统计结果

# 项目结构非常的简单
1. code analyzer/ analyzer 接口 就是一个文件名 filename 然后他会返回
统计结果 StatisticsResult

2. global code analyzer 就是一个driver 它把configuration和analyzer结合起来

3. 我们仍然需要组织一个configuration 这次是通过命令行的方式来组织
4. 然后我们把configuration传递给driver 这样driver就可以开始工作了
5. driver调用analyzer(path) -> result, 然后driver将result累加即可