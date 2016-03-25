cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0 -g3")
include_directories(./headers ./sources)
add_executable (sim ./sources/main.cpp)
