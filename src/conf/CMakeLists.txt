add_library(conf STATIC 
    src/conf.cpp
)
target_include_directories(conf 
    PUBLIC include
)
