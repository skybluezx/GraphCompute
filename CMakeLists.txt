cmake_minimum_required(VERSION 3.19)
project(GraphCompute)

set(CMAKE_CXX_STANDARD 20)
find_package (glog 0.5.0 REQUIRED)
find_package(Boost 1.62.0 REQUIRED COMPONENTS filesystem)

if(NOT Boost_FOUND)
    message("Boost not found")
endif()

add_executable(GraphCompute main.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h)
target_link_libraries (GraphCompute glog::glog)
target_link_libraries(GraphCompute ${Boost_LIBRARIES})