//
// Created by 翟霄 on 2021/8/3.
// 用于图计算框架的测试开始
// 避免C/S模式导致繁琐的开发与测试
//
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "Util.h"
#include "Graph.h"

#include "Command.h"

/**
 * 定义命令行参数
 */
// 配置文件路径
DEFINE_string(config_file_path, "", "配置文件路径 ");
DEFINE_validator(config_file_path, &Util::validatePath);

int main(int argc, char* argv[]) {
    /**
     * 启动配置部分
     */

    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // 初始化日志模块
    google::InitGoogleLogging(argv[0]);

    // 设置配置文件路径
    Util::configFilePath = FLAGS_config_file_path;

    // 配置日志输出路径
    std::string logDirectory;
    Util::getConfig("Path", "log_directory", logDirectory);
    FLAGS_log_dir = logDirectory;
    std::string logLevel;
    Util::getConfig("LOG", "log_level", logLevel);
    if (logLevel == "INFO") {
        FLAGS_minloglevel = google::INFO;
    } else {
        FLAGS_minloglevel = google::ERROR;
    }

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);
    int readEdgeCount;
    Util::getConfig("Input", "read_edge_count", readEdgeCount);
    std::string resultType;
    Util::getConfig("Input", "result_type", resultType);
    // 建立图
    Graph graph = Graph(graphDefineDirectory, resultType, readEdgeCount);
    graph.flush();
    // 输出图的概要
    auto nodeTypeCountList = graph.getNodeTypeCountList();
    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
        LOG(INFO) << iter->first << ":" << iter->second;
    }
    google::FlushLogFiles(google::INFO);

//    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test_exclude_node_or_edge_from_file.json";
//    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test_exclude_node.json";
    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test_single_input.json";
    std::ifstream jsonFile(json_file_path);
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString(buffer.str());
    std::cout << "执行命令：" << std::endl;
    std::cout << jsonString << std::endl;

    // 读取计算结果输出路径
    std::string resultDirectoryPath;
    Util::getConfig("Path", "result_directory", resultDirectoryPath);

//    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    clock_t startTime, endTime;
    startTime = clock();//计时开始

    // 执行命令
    Command::execute(graph, jsonString, resultDirectoryPath);

    endTime = clock();//计时结束
//    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
//    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
//    std::cout << "[INFO] 命令执行时间：" << programSpan.count() << "秒" << std::endl;
    std::cout << "[INFO] 命令执行时间：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << std::endl;

    google::ShutdownGoogleLogging();
}