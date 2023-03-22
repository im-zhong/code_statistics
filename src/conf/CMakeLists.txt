project(conf)

# static conf library
add_library(conf STATIC 
    src/conf.cpp
)
target_include_directories(conf 
    PUBLIC include
)

# conf unit tests
add_executable(test_conf 
    test/test_conf.cpp
)
target_link_libraries(test_conf
    PRIVATE conf
    PRIVATE gtest::gtest
)
