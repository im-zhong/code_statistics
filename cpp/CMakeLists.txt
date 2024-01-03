cmake_minimum_required(VERSION 3.22)

project(CodeStatistics   VERSION 0.1
                DESCRIPTION "C++"
                LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(CLI11 REQUIRED)
find_package(doctest REQUIRED)

add_subdirectory(src)
