# Code Statistics
A simple code statistics tool

# Example Usage
`code_statistics src`

running this command, tool will recursively traverse all cpp files and do statistics under src directory.

for example, like this code

src/main.cpp
```
#include <iostream> // include for std::cout and std::endl

int main(int argc, char* argv[]) {
    // print hello world
    std::cout << "hello, world" << std::endl;
    return 0;
}
```
the second line is blank line, and the forth line is comment line, and the rest lines are all code lines. so the output of tool is something like this:
```
stats... src/main.cpp

files: 1
lines: 7
codes: 5 (71.43%)
comments: 2 (28.57%)
```

for more infomation, use `code_statistics -h` to see help.

# Build

1. install conan and configure it well, `pip install conan`. for more infomation, see [conan](https://conan.io/)
2. download and install my conan tools: [conan tools](https://github.com/im-zhong/conan.git)
3. clone this repo to your local machine `git clone https://github.com/im-zhong/code_statistics.git`
4. `cd code_statistics`
5. `conan install . --build=missing`
6. `conan build .`
