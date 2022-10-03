add_library(Parallel SHARED parallel.cpp worker.cpp)

target_sources(Parallel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/parallel.cpp)

target_link_libraries(Parallel ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(Parallel PUBLIC include)