cmake_minimum_required(VERSION 3.23)
project(parallel)

set(CMAKE_CXX_STANDARD 23)

add_executable(parallel_test test/test.cpp)
target_include_directories(parallel_test PUBLIC include)