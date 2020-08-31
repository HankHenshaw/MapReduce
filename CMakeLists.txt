cmake_minimum_required(VERSION 3.5)

project(mapreduce)

find_package(Threads)

add_executable(hadoop main.cpp mapreduce.cpp mapreduce.h)

set_target_properties(hadoop PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-fsanitize=thread"
    LINK_OPTIONS "-fsanitize=thread"
)

target_link_libraries(hadoop
    ${CMAKE_THREAD_LIBS_INIT}
)

