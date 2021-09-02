//
// Created by 翟霄 on 2021/8/16.
// 题目召回主逻辑
//
#include <iostream>
#include <filesystem>
#include <boost/json.hpp>

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
DEFINE_string(task_dir_path, "", "任务文件目录 ");
DEFINE_validator(task_dir_path, &Util::validatePath);

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

    std::string questionRecallConfigFile;
    Util::getConfig("QuestionRecall", "config_file_path", questionRecallConfigFile);
    
    LOG(INFO) << "配置文件路径：" << Util::configFilePath;
    LOG(INFO) << "日志输出路径：" << logDirectory;
    LOG(INFO) << "日志输出等级：" << logLevel;
    LOG(INFO) << "图定义路径：" << graphDefineDirectory;
    LOG(INFO) << "读取边数：" << readEdgeCount;
    LOG(INFO) << "图结果类型：" << resultType;
    LOG(INFO) << "结果输出路径：" << resultDirectoryPath;
    LOG(INFO) << "初始化节点个数：" << initNodeCount;
    LOG(INFO) << "最大支持开始点个数：" << maxWalkBeginNodeCount;
    LOG(INFO) << "题目召回配置文件：" << questionRecallConfigFile;
    google::FlushLogFiles(google::INFO);

    // 建立图
    Graph graph = Graph(graphDefineDirectory, resultType, readEdgeCount, initNodeCount, maxWalkBeginNodeCount);
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
     * 请求部分
     */

    // 任务文件读取
    std::string json_dir_path = FLAGS_task_dir_path;
    std::filesystem::path path(json_dir_path);
    if (!std::filesystem::exists(path)) {
        LOG(ERROR) << "任务目录不存在！";
    } else {
        // 遍历图定义文件所在目录的全部图定义文件
        for (auto it = std::filesystem::recursive_directory_iterator(path); it != std::filesystem::recursive_directory_iterator(); ++it) {
            auto &entry = *it;
            if (std::filesystem::is_regular_file(entry)) {
                std::string jsonFilePath = entry.path();
                // 不读取"."开头的文件
                // 避免读取临时文件，也可用于文件的选择
                boost::filesystem::path filePath(jsonFilePath);
                if (filePath.filename().string()[0] == '.') {
                    LOG(INFO) << "跳过文件：" << filePath.filename().string();
                    continue;
                }
                LOG(INFO) << "读取文件：" << filePath.filename();

                std::ifstream jsonFile(jsonFilePath);
                std::stringstream buffer;
                buffer << jsonFile.rdbuf();
                std::string jsonString(buffer.str());
                std::cout << "请求：" << std::endl;
                std::cout << jsonString << std::endl;
                auto commandObj = boost::json::parse(jsonString);

                std::vector<std::string> beginNodeTypeList;
                std::vector<std::map<std::string, double>> beginNodeIDList;

                // 获取开始点类型
                // 同时创建相关参数的默认值
                for (auto iter = commandObj.at("beginNodeType").as_array().begin(); iter != commandObj.at("beginNodeType").as_array().end(); ++iter) {
                    beginNodeTypeList.emplace_back(iter->as_string().c_str());
                    beginNodeIDList.emplace_back(std::map<std::string, double>());
                }

                // 设置开始点ID
                auto i = 0;
                for (auto iter = commandObj.at("beginNodeID").as_array().begin(); iter != commandObj.at("beginNodeID").as_array().end(); ++iter) {
                    for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter){
                        beginNodeIDList[i][subIter->as_object().at("id").as_string().c_str()] = subIter->as_object().at("weight").as_double();
                    }
                    i++;
                }

                // 获取访问次数TopN
                int visitedCountTopN;
                visitedCountTopN = commandObj.at("visitedCountTopN").as_int64();

                // 生成题目召回的测试入参
                arch::In recallIn;
                // 设置召回目标题目数
                recallIn.expected = visitedCountTopN;

                for (int i = 0; i < beginNodeTypeList.size(); ++i) {
                    if (beginNodeTypeList[i] == "Question") {
                        // 设置本节课题目及作答结果
                        for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                            recallIn.questions_assement[iter->first] = iter->second;
                        }
                    } else {
                        // 设置本节课知识点及评测结果
                        for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                            recallIn.current_knowledge_points[iter->first] = iter->second;
                        }
                    }
                }
                
                //    // 设置前序课堂全部作答题目的ID及作答结果
                //    recallIn.preceding_questions_assement["01b96113ecb94b7c8bef62a92f2d47c7"] = 1;
                //    recallIn.preceding_questions_assement["01c0cefce12f4bf0b69788826662e329"] = 2;
                //    recallIn.preceding_questions_assement["01c5dc41c3a94459bd8cb038d91a8f73"] = 3;
                //    recallIn.preceding_questions_assement["021f1a0352c44fbca5c84c3deb5b35da"] = 4;
                //    recallIn.preceding_questions_assement["01d01f8097d04428ad3f79abf5d1abeb"] = 5;
                //    // 设置前序课堂全部知识点及测评结果
                //    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c0519"] = 0.5;
                //    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c057c"] = 0.8;
                //    recallIn.knowledge_points["e7e71d086b10469f8e71d733ac2a0c42"] = 0.9;
                //    recallIn.knowledge_points["ee664695d55446d5a2398115b674adf2"] = 0.3;
                //    recallIn.knowledge_points["6oo7ryi0e79zkqai08wbmeohxn6c0499"] = 0.2;

                std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

                // 执行召回命令
                arch::Out recallOut = Command::questionRecall(recallIn, graph);

                std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
                std::cout << "[INFO] 命令执行时间：" << programSpan.count() << "秒" << std::endl;

//                // 打印召回结果
//                for (auto iter = recallOut.payload.begin(); iter != recallOut.payload.end(); ++iter) {
//                    std::cout << *iter << std::endl;
//                }

                std::string resultFilePath = resultDirectoryPath + "/" + filePath.filename().string() + "_result.dat";
                std::ofstream resultFile;
                resultFile.open(resultFilePath);
                for (auto iter = recallOut.payload.begin(); iter != recallOut.payload.end(); ++iter) {
                    resultFile << *iter << std::endl;
                }
                resultFile.close();
            }
        }
    }

    google::ShutdownGoogleLogging();
}
