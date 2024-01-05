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

