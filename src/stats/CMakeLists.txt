project(stats)

# static stats library
add_library(stats STATIC
    src/analysis_result.cpp
    src/cpp_analyzer.cpp
    src/code_analyzer.cpp
    src/rust_analyzer.cpp
    src/python_analyzer.cpp
    src/all_analyzer.cpp
)
target_include_directories(stats 
    PUBLIC include
)
target_link_libraries(stats
    PUBLIC fmt::fmt
)

# stats unit tests
add_executable(test_stats 
    test/test_stats.cpp
)
target_link_libraries(test_stats
    PRIVATE conf
    PRIVATE stats
    PRIVATE driver
    PRIVATE doctest::doctest
    PRIVATE fmt::fmt
)
