cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -O0 -g3")
add_executable (sort-search main.cpp)
