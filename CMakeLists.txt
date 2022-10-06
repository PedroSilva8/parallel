cmake_minimum_required(VERSION 3.0.0)
project(src VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)

include(GenerateExportHeader)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
endif (CMAKE_BUILD_TYPE EQUAL "DEBUG")

find_package(Threads REQUIRED)

add_library(Parallel SHARED)

target_link_libraries(Parallel ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(Parallel PUBLIC include)

generate_export_header(Parallel)

target_compile_definitions(Parallel PRIVATE _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE PUBLIC $<$<CONFIG:Debug>:OE_DEBUG> $<$<CONFIG:RelWithDebInfo>:OE_DEBUG> $<$<CONFIG:Release>:OE_RELEASE> $<$<CONFIG:MinSizeRel>:OE_RELEASE>)
INSTALL (DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(TARGETS Parallel CONFIGURATIONS Debug RUNTIME DESTINATION Debug/bin)
install(TARGETS Parallel CONFIGURATIONS Release RUNTIME DESTINATION Release/bin)

add_subdirectory(src)

add_subdirectory(test)