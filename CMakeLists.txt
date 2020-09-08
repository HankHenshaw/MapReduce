cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED ENV{TRAVIS_BUILD_NUMBER})
    set(ENV{TRAVIS_BUILD_NUMBER} 1)
endif()

project(mapreduce VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

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

install(TARGETS hadoop RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT "HankHenshaw at github.com")

include(CPack)