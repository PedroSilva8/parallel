cmake_minimum_required(VERSION 3.0.0)
project(Parallel VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(MALLOC_CHECK_ 2)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

find_package(Threads REQUIRED)

add_subdirectory(parallel)
add_subdirectory(test)