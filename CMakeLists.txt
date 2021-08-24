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

# 图计算框架
add_executable(GraphCompute main.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h)
target_link_libraries (GraphCompute glog::glog)
target_link_libraries(GraphCompute ${Boost_LIBRARIES})

# C/S模式图计算-服务端
add_executable(GraphComputeServer graph_compute_server.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h)
target_link_libraries (GraphComputeServer glog::glog)
target_link_libraries(GraphComputeServer ${Boost_LIBRARIES})
if (LINUX)
    target_link_libraries(GraphComputeServer rt)
endif ()

# C/S模式图计算-客户端
add_executable(GraphComputeClient graph_compute_client.cpp)
target_link_libraries (GraphComputeClient glog::glog)
target_link_libraries(GraphComputeClient ${Boost_LIBRARIES})
if (LINUX)
    target_link_libraries(GraphComputeClient rt)
endif ()

# 具体应用
# 题目召回服务端
add_executable(QuestionRecallServer question_recall_server.cpp Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h args.h)
target_link_libraries (QuestionRecallServer glog::glog)
target_link_libraries(QuestionRecallServer ${Boost_LIBRARIES})

add_library(graphc Util.cpp Util.h Graph.cpp Graph.h Node.cpp Node.h type_defination.h Command.cpp Command.h args.h)
target_link_libraries (graphc glog::glog)
target_link_libraries(graphc ${Boost_LIBRARIES})

