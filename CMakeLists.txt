cmake_minimum_required(VERSION 3.19)
project(GraphCompute)

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif (CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif (CMAKE_HOST_UNIX)
    set(LINUX 1)
endif ()

set(CMAKE_CXX_STANDARD 20)

find_package (glog 0.5.0 REQUIRED)
find_package(Boost 1.62.0 REQUIRED COMPONENTS filesystem json)


if (NOT Boost_FOUND)
    message("Boost not found")
endif ()

add_executable(GraphCompute graph_compute_server.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h)
target_link_libraries (GraphCompute glog::glog)
target_link_libraries(GraphCompute ${Boost_LIBRARIES})
if (LINUX)
    target_link_libraries(GraphCompute rt)
endif ()

add_executable(GraphComputeClient graph_compute_client.cpp)
target_link_libraries (GraphComputeClient glog::glog)
target_link_libraries(GraphComputeClient ${Boost_LIBRARIES})
if (LINUX)
    target_link_libraries(GraphComputeClient rt)
endif ()

add_executable(GraphComputeTest dev_test.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h)
target_link_libraries (GraphComputeTest glog::glog)
target_link_libraries(GraphComputeTest ${Boost_LIBRARIES})

add_executable(QuestionRecall question_recall_test.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h question_recall_define.h)
target_link_libraries (QuestionRecall glog::glog)
target_link_libraries(QuestionRecall ${Boost_LIBRARIES})