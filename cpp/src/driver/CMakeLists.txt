project(driver)

# static driver library
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

# driver unit tests
add_executable(test_driver 
    test/test_driver.cpp
)
target_link_libraries(test_driver
    PRIVATE driver
    PRIVATE doctest::doctest
)
