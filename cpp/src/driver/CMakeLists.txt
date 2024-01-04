add_library(driver STATIC 
    src/driver.cpp
)
target_include_directories(driver 
    PUBLIC include
)
target_link_libraries(driver
    PUBLIC conf
    PUBLIC stats 
)
