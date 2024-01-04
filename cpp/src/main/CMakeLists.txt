add_executable(code_statistics main.cpp)
target_link_libraries(code_statistics 
    PRIVATE CLI11::CLI11
    PRIVATE conf
    PRIVATE stats
    PRIVATE driver
)
