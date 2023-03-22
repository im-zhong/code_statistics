cmake_minimum_required(VERSION 3.22)

project(CodeStatistics   VERSION 0.1
                DESCRIPTION "C++"
                LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# find_package(GTest REQUIRED)
# find_package(fmt REQUIRED)
# # find_package(RocksDB REQUIRED)
# find_package(asio REQUIRED)
find_package(CLI11 REQUIRED)

add_subdirectory(src)
