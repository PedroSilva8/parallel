cmake_minimum_required(VERSION 3.0.0)
project(src VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(MALLOC_CHECK_ 2)

include(GenerateExportHeader)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

find_package(Threads REQUIRED)

add_library(Parallel SHARED)

target_link_libraries(Parallel ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(Parallel PUBLIC include)

include("GenerateExportHeader")
generate_export_header(Parallel)

add_subdirectory(src)

add_subdirectory(test)