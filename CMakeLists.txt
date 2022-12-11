cmake_minimum_required(VERSION 3.23)
project(parallel)

set(CMAKE_CXX_STANDARD 23)

add_library(parallel SHARED src/task.cpp src/worker.cpp)
target_include_directories(parallel PUBLIC include)

add_executable(parallel_test test/test.cpp)
target_include_directories(parallel_test PUBLIC include)
target_link_libraries(parallel_test PUBLIC parallel)

target_compile_definitions(parallel PRIVATE _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE PUBLIC $<$<CONFIG:Debug>:OE_DEBUG> $<$<CONFIG:RelWithDebInfo>:OE_DEBUG> $<$<CONFIG:Release>:OE_RELEASE> $<$<CONFIG:MinSizeRel>:OE_RELEASE>)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(TARGETS parallel)