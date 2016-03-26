cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0 -g3")
set(BOOST_ROOT /Users/dustinhoman/Downloads/boost_1_60_0)
set(BOOST_LIBRARYDIR /usr/local/Cellar/boost/1.60.0_1/)
find_package(Boost 1.60.0 COMPONENTS system filesystem iostreams REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(sim main.cpp)
  target_link_libraries(sim
  ${Boost_IOSTREAMS_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${Boost_SYSTEM_LIBRARIES})
endif()
