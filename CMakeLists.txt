cmake_minimum_required(VERSION 3.12)
project(vcacheopt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test test.cpp)

if(MSVC)
    target_compile_options(test PRIVATE /W4)
else()
    target_compile_options(test PRIVATE -Wall -Wextra -Wpedantic)
endif()
