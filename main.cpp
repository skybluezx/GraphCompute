//
// Created by 翟霄 on 2021/8/3.
// 图计算框架的
// 避免C/S模式导致繁琐的开发与测试
//
#include <iostream>
#include <string>

#include "Util.h"
#include "Graph.h"

#include "Command.h"

/**
 * 定义命令行参数
 */
// 配置文件路径
DEFINE_string(config_file_path, "", "配置文件路径 ");
DEFINE_validator(config_file_path, &Util::validatePath);
// 图任务文件路径
DEFINE_string(task_file_path, "", "任务文件路径 ");
DEFINE_validator(task_file_path, &Util::validatePath);

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
    // 读取计算结果输出路径
    std::string resultDirectoryPath;
    Util::getConfig("Path", "result_directory", resultDirectoryPath);
    int initNodeCount;
    Util::getConfig("Input", "init_node_count", initNodeCount);
    int maxWalkBeginNodeCount;
    Util::getConfig("Walk", "max_walk_begin_node_count", maxWalkBeginNodeCount);

    LOG(INFO) << "配置文件路径：" << Util::configFilePath;
    LOG(INFO) << "日志输出路径：" << logDirectory;
    LOG(INFO) << "日志输出等级：" << logLevel;
    LOG(INFO) << "图定义路径：" << graphDefineDirectory;
    LOG(INFO) << "读取边数：" << readEdgeCount;
    LOG(INFO) << "图结果类型：" << resultType;
    LOG(INFO) << "结果输出路径：" << resultDirectoryPath;
    LOG(INFO) << "初始化节点个数：" << initNodeCount;
    LOG(INFO) << "最大支持开始点个数：" << maxWalkBeginNodeCount;
    google::FlushLogFiles(google::INFO);

    /**
     * 主逻辑部分
     */

    // 建立图
    Graph graph = Graph(graphDefineDirectory, resultType, readEdgeCount, initNodeCount, maxWalkBeginNodeCount);
    // 刷新图
    graph.flush();
    // 输出图的概要
    auto nodeTypeCountList = graph.getNodeTypeCountList();
    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
        LOG(INFO) << iter->first << ":" << iter->second;
    }

    // 任务文件读取
    std::string json_file_path = FLAGS_task_file_path;
    std::ifstream jsonFile(json_file_path);
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString(buffer.str());
    std::cout << "执行任务：" << std::endl;
    std::cout << jsonString << std::endl;

    // 设置任务开始时间
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    clock_t startTime, endTime;
    startTime = clock();//计时开始

    // 执行任务
    Command::execute(graph, jsonString, resultDirectoryPath);

    // 设置任务结束时间
    endTime = clock();
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "[INFO] 任务执行时间（自然时间）：" << programSpan.count() << "秒" << std::endl;
    std::cout << "[INFO] 任务执行时间（CPU时间）：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "秒"  << std::endl;

    google::ShutdownGoogleLogging();
}
