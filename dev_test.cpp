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

//#include "Command.h"

/**
 * 定义命令行参数
 */
// 配置文件路径
DEFINE_string(config_file_path, "", "配置文件路径 ");
DEFINE_validator(config_file_path, &Util::validatePath);

int main(int argc, char* argv[]) {
//    /**
//     * 启动配置部分
//     */
//
//    // 解析命令行参数
//    gflags::ParseCommandLineFlags(&argc, &argv, true);
//    // 初始化日志模块
//    google::InitGoogleLogging(argv[0]);
//
//    // 设置配置文件路径
//    Util::configFilePath = FLAGS_config_file_path;
//
//    // 配置日志输出路径
//    std::string logDirectory;
//    Util::getConfig("Path", "log_directory", logDirectory);
//    FLAGS_log_dir = logDirectory;
//
//    // 设置图定义文件
//    std::string graphDefineDirectory;
//    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);
//    int readEdgeCount;
//    Util::getConfig("Input", "read_edge_count", readEdgeCount);
//    // 建立图
//    Graph graph = Graph(graphDefineDirectory, readEdgeCount);
//    // 输出图的概要
//    auto nodeTypeCountList = graph.getNodeTypeCountList();
//    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
//        LOG(INFO) << iter->first << ":" << iter->second;
//    }
//    google::FlushLogFiles(google::INFO);
//
////    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test_single_input.json";
//    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test_exclude_node.json";
//    std::ifstream jsonFile(json_file_path);
//    std::stringstream buffer;
//    buffer << jsonFile.rdbuf();
//    std::string jsonString(buffer.str());
//    std::cout << jsonString << std::endl;
//
//    // 读取计算结果输出路径
//    std::string resultDirectoryPath;
//    Util::getConfig("Path", "result_directory", resultDirectoryPath);
//    // 执行命令
//    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
//    Command::execute(graph, jsonString, resultDirectoryPath);
//    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
//    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
//    std::cout << "[INFO] 命令执行时间：" << programSpan.count() << "秒" << std::endl;

    std::default_random_engine randomEngine;
    std::uniform_int_distribution<unsigned> randomDistribution;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
    std::cout << randomDistribution(randomEngine) << std::endl;
}