add_executable(test test.cpp)

include_directories(${Parallel_SOURCE_DIR})

target_link_libraries(test PUBLIC Parallel)