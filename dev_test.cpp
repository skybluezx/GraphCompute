//
// Created by 翟霄 on 2021/8/3.
//
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

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

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);
    int readEdgeCount;
    Util::getConfig("Input", "read_edge_count", readEdgeCount);
    // 建立图
    Graph graph = Graph(graphDefineDirectory, readEdgeCount);
    // 输出图的概要
    auto nodeTypeCountList = graph.getNodeTypeCountList();
    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
        LOG(INFO) << iter->first << ":" << iter->second;
    }
    google::FlushLogFiles(google::INFO);

//    std::string json_file_path = "/Users/zhaixiao/workplace/c_cpp/GraphCompute/task/test.json";
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
//    Command::execute(graph, jsonString, resultDirectoryPath);

    std::vector<std::string> idList;
    auto result1 = graph.isLinked("KnowledgePoint", "f9e12aee12214301b757841df388be97", "Question", "ffec47fd810b41e093f114a17b2e0b27", idList);
    auto result2 = graph.isLinked("KnowledgePoint", "f9e12aee12214301b757841df388be97", "Question", "jedb69p0s7kyf6u707kqx4lwieuqm96k", idList);
    auto result3 = graph.isLinked("KnowledgePoint", "f9e12aee12214301b757841df388be97", "Courseware", "74f887a3897a415d969fd4c5cbd84eba", idList);
    auto result4 = graph.isLinked("KnowledgePoint", "f9e12aee12214301b757841df388be97", "KnowledgePoint", "6oo7ryi0e79zkqai08wbmeohxn6c03ea", idList);
    std::cout << result1 << result2 << result3 << result4 << std::endl;
}