cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -lpthread")
add_executable (mult-thread-test main.cpp)
