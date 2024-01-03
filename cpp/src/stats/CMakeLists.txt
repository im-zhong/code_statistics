project(stats)

# static stats library
add_library(stats STATIC
    src/analysis_result.cpp
    src/cpp_analyzer.cpp
    src/code_analyzer.cpp
    src/rust_analyzer.cpp
)
target_include_directories(stats 
    PUBLIC include
)

# stats unit tests
add_executable(test_stats 
    test/test_stats.cpp
)
target_link_libraries(test_stats
    PRIVATE stats
    PRIVATE doctest::doctest
)
