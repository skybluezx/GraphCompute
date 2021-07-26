cmake_minimum_required(VERSION 3.19)
project(GraphCompute)

set(CMAKE_CXX_STANDARD 20)
find_package (glog 0.5.0 REQUIRED)

add_executable(GraphCompute main.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h)
target_link_libraries (GraphCompute glog::glog)