//
// Created by 翟霄 on 2021/8/16.
// 题目召回主逻辑
//
#include <iostream>

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

    std::string questionRecallConfigFile;
    Util::getConfig("QuestionRecall", "config_file_path", questionRecallConfigFile);

    LOG(INFO) << "配置文件路径：" << Util::configFilePath;
    LOG(INFO) << "日志输出路径：" << logDirectory;
    LOG(INFO) << "日志输出等级：" << logLevel;
    LOG(INFO) << "图定义路径：" << graphDefineDirectory;
    LOG(INFO) << "读取边数：" << readEdgeCount;
    LOG(INFO) << "图结果类型：" << resultType;
    LOG(INFO) << "题目召回配置文件：" << questionRecallConfigFile;
    google::FlushLogFiles(google::INFO);

    // 建立图
    Graph graph = Graph(graphDefineDirectory, resultType, readEdgeCount);
    // 刷新图
    graph.flush();
    // 输出图的概要
    auto nodeTypeCountList = graph.getNodeTypeCountList();
    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
        LOG(INFO) << iter->first << ":" << iter->second;
    }
    google::FlushLogFiles(google::INFO);

    // 读取题目召回配置文件
    Command::questionRecallInitialize(questionRecallConfigFile);

    /**
     * 样例部分
     */

    // 生成题目召回的测试入参
    arch::In recallIn;
    // 设置召回目标题目数
    recallIn.expected = 100;
    // 设置本节课题目及作答结果
    recallIn.questions_assement["00042065a40e44cbbb828dbde48b3380"] = 1;
    recallIn.questions_assement["00056a833cdf47439b94b9d33a40c5dc"] = 2;
    recallIn.questions_assement["000c9dba24b5442dbf10492dc591dba1"] = 3;
    recallIn.questions_assement["000d692ddabf4457aacfa15a9fa6e3d2"] = 4;
    recallIn.questions_assement["0010530e1030415fa268fe21c987890b"] = 5;
    // 设置本节课知识点及评测结果
    recallIn.current_knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c03db"] = 0.5;
    recallIn.current_knowledge_points["a2cb5157fe734eb6a95dbbb6a29f3638"] = 0.8;
    recallIn.current_knowledge_points["32e7d0d2f797452bafa4c930c2d3121a"] = 0.9;
    recallIn.current_knowledge_points["abe940b2a0f54e05ad2a364b4952f866"] = 0.3;
    recallIn.current_knowledge_points["c1d5971750dc4634ac437f6688a4c156"] = 0.2;
    // 设置前序课堂全部作答题目的ID及作答结果
    recallIn.preceding_questions_assement["01b96113ecb94b7c8bef62a92f2d47c7"] = 1;
    recallIn.preceding_questions_assement["01c0cefce12f4bf0b69788826662e329"] = 2;
    recallIn.preceding_questions_assement["01c5dc41c3a94459bd8cb038d91a8f73"] = 3;
    recallIn.preceding_questions_assement["021f1a0352c44fbca5c84c3deb5b35da"] = 4;
    recallIn.preceding_questions_assement["01d01f8097d04428ad3f79abf5d1abeb"] = 5;
    // 设置前序课堂全部知识点及测评结果
    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c0519"] = 0.5;
    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c057c"] = 0.8;
    recallIn.knowledge_points["e7e71d086b10469f8e71d733ac2a0c42"] = 0.9;
    recallIn.knowledge_points["ee664695d55446d5a2398115b674adf2"] = 0.3;
    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c0499"] = 0.2;

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    // 执行召回命令
    arch::Out recallOut = Command::questionRecall(recallIn, graph);

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "[INFO] 命令执行时间：" << programSpan.count() << "秒" << std::endl;

    graph.clearResultList();

    // 打印召回结果
    for (auto iter = recallOut.payload.begin(); iter != recallOut.payload.end(); ++iter) {
        std::cout << *iter << std::endl;
    }

    google::ShutdownGoogleLogging();
}