//
// Created by 翟霄 on 2021/8/3.
//

#include "Command.h"

#include <vector>
#include <map>
#include <iostream>
#include <boost/json.hpp>

void Command::execute(Graph &graph, const std::string &command, const std::string &resultDirectoryPath) {
    auto commandObj = boost::json::parse(command);

    if (commandObj.at("name").as_string() == "exclude_node") {
        /**
         * 排除节点
         */
        LOG(INFO) << "[排除节点]";

        std::vector<std::string> excludeNodeTypeIDList;
        for (auto iter = commandObj.at("type_id").as_array().begin(); iter != commandObj.at("type_id").as_array().end(); ++iter) {
            excludeNodeTypeIDList.push_back(iter->as_string().c_str());
        }
        graph.excludeNodes(excludeNodeTypeIDList);

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").as_string() == "exclude_node_or_edge_from_file") {
        /**
         * 排除节点或边（从文件中读取）
         */
        LOG(INFO) << "[排除节点或边（从文件中读取）]";

        // 初始化
        std::vector<std::string> excludeNodeList;
        std::vector<std::pair<std::string, std::string>> excludeEdgeList;

        std::string filePath = commandObj.at("file_path").as_string().c_str();
        std::ifstream excludeFile(filePath, std::ios::in);
        // 判断图定义文件打开是否成功
        if (!excludeFile.is_open()) {
            // 打开失败则输出错误日志
            LOG(ERROR) << "文件读取失败！";
        } else {
            std::string line;
            while (std::getline(excludeFile, line)) {
                if (line.empty()) {
                    continue;
                }
                // 切分行
                std::vector<std::string> lineItemList = Util::stringSplitToVector(line, "@");
                if (lineItemList.size() != 2) {
                    // 输出错误日志
                    LOG(ERROR) << "格式错误！";
                } else {
                    std::vector<std::string> nodePair = Util::stringSplitToVector(lineItemList[1], "-");
                    if (nodePair.size() == 1) {
                        // 排除点
                        excludeNodeList.push_back(nodePair[0]);
                    } else if (nodePair.size() == 2) {
                        // 排除边
                        excludeEdgeList.push_back(std::pair(nodePair[0], nodePair[1]));
                        excludeEdgeList.push_back(std::pair(nodePair[1], nodePair[0]));
                    } else {
                        // 输出错误日志
                        LOG(ERROR) << "格式错误！";
                    }
                }
            }
        }

        if (!excludeNodeList.empty()) {
            graph.excludeNodes(excludeNodeList);
        }

        if (!excludeEdgeList.empty()) {
            graph.excludeEdges(excludeEdgeList);
        }

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").as_string() == "include_node_or_edge_from_file") {
        /**
         * 包含节点或边（从文件中读取）
         */
        LOG(INFO) << "[包含节点或边（从文件中读取）]";

        // 初始化
        std::vector<std::string> includeNodeList;
        std::vector<std::pair<std::string, std::string>> includeEdgeList;

        std::string filePath = commandObj.at("file_path").as_string().c_str();
        std::ifstream includeFile(filePath, std::ios::in);
        // 判断图定义文件打开是否成功
        if (!includeFile.is_open()) {
            // 打开失败则输出错误日志
            LOG(ERROR) << "文件读取失败！";
        } else {
            std::string line;
            while (std::getline(includeFile, line)) {
                if (line.empty()) {
                    continue;
                }
                // 切分行
                std::vector<std::string> lineItemList = Util::stringSplitToVector(line, "@");
                if (lineItemList.size() != 2) {
                    // 输出错误日志
                    LOG(ERROR) << "格式错误！";
                } else {
                    std::vector<std::string> nodePair = Util::stringSplitToVector(lineItemList[1], "-");
                    if (nodePair.size() == 1) {
                        // 包含点
                        includeNodeList.push_back(nodePair[0]);
                    } else if (nodePair.size() == 2) {
                        // 包含边
                        includeEdgeList.emplace_back(std::pair(nodePair[0], nodePair[1]));
                        includeEdgeList.emplace_back(std::pair(nodePair[1], nodePair[0]));
                    } else {
                        // 输出错误日志
                        LOG(ERROR) << "格式错误！";
                    }
                }
            }
        }

        if (!includeNodeList.empty()) {
            graph.includeNodes(includeNodeList);
        }

        if (!includeEdgeList.empty()) {
            graph.includeEdges(includeEdgeList);
        }

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").as_string() == "traverse") {
        /**
         * 遍历
         * 深度/广度优先遍历
         */
        LOG(INFO) << "[深度/广度优先遍历]";

//        auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
//        for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
//            std::cout << *i << std::endl;
//        }

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").get_string() == "walk") {
        /**
         * 随机游走
         */
        LOG(INFO) << "[随机游走]";

        // 获取开始点类型
        std::string beginNodeType = commandObj.at("beginNodeType").as_string().c_str();
        // 获取开始点ID
        std::vector<std::string> beginNodeIDList ;
        for (auto iter = commandObj.at("beginNodeID").as_array().begin(); iter != commandObj.at("beginNodeID").as_array().end(); ++iter) {
            beginNodeIDList.push_back(iter->as_string().c_str());
        }
        // 获取步的边组成
        std::vector<std::string> stepDefine;
        for (auto iter = commandObj.at("stepDefine").as_array().begin(); iter != commandObj.at("stepDefine").as_array().end(); ++iter) {
            stepDefine.push_back(iter->as_string().c_str());
        }
        // 获取辅助边
        std::map<std::string, std::string> auxiliaryEdge;
        for (auto iter = commandObj.at("auxiliaryEdge").as_object().begin(); iter != commandObj.at("auxiliaryEdge").as_object().end(); ++iter) {
            auxiliaryEdge[iter->key_c_str()] = iter->value().as_string().c_str();
            // Todo
            // 辅助边应该具有方向且应该从被辅助点指向辅助点
            // 目前为了容错，将辅助的双向都进行了存储
            auxiliaryEdge[iter->value().as_string().c_str()] = iter->key_c_str();
        }
        // 获取单次游走步长参数
        double walkLengthRatio = commandObj.at("walkLengthRatio").as_double();
        // 获取重启概率
        double restartRatio = commandObj.at("restartRatio").as_double();
        // 获取总游走步长
        unsigned int totalStepCount = commandObj.at("totalStepCount").as_int64();

        // 获取目标点类型
        std::string targetNodeType = commandObj.at("targetNodeType").as_string().c_str();
        // 获取访问次数TopN
        int visitedCountTopN = commandObj.at("visitedCountTopN").as_int64();

        for (auto i = 0; i < beginNodeIDList.size(); ++i) {
            // 游走
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            graph.walk(beginNodeType, beginNodeIDList[i], stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount);
            std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
            std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
            LOG(INFO) << "[INFO] 单次游走时长：" << programSpan.count() << "秒";
            google::FlushLogFiles(google::INFO);

            // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
            std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(targetNodeType);
            // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
            int n = visitedCountTopN;
            if (n > result.size()) n = result.size();
            std::ofstream resultFile;
            resultFile.open(resultDirectoryPath + "/" + beginNodeType + ":" + beginNodeIDList[i] + "_result.dat");
            for (auto i = 0; i < n; ++i) {
                resultFile << result[i].first << ": " << result[i].second << std::endl;
            }
            resultFile.close();
        }

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").get_string() == "multi_walk") {
        /**
         * 多重随机游走
         */
        LOG(INFO) << "[多重随机游走]";

        // 获取开始点类型
        std::vector<std::string> beginNodeTypeList;
        for (auto iter = commandObj.at("beginNodeType").as_array().begin(); iter != commandObj.at("beginNodeType").as_array().end(); ++iter) {
            beginNodeTypeList.emplace_back(iter->as_string().c_str());
        }

        // 获取开始点ID
        std::vector<std::map<std::string, double>> beginNodeIDList ;
        for (auto iter = commandObj.at("beginNodeID").as_array().begin(); iter != commandObj.at("beginNodeID").as_array().end(); ++iter) {
            std::map<std::string, double> idList;
            for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter){
                idList[subIter->as_string().c_str()] = 0;
            }
            beginNodeIDList.emplace_back(idList);
        }

        // 获取步的边组成
        std::vector<std::vector<std::string>> stepDefineList;
        for (auto iter = commandObj.at("stepDefine").as_array().begin(); iter != commandObj.at("stepDefine").as_array().end(); ++iter) {
            std::vector<std::string> defineList;
            for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter) {
                defineList.emplace_back(subIter->as_string().c_str());
            }
            stepDefineList.emplace_back(defineList);
        }

        // 获取辅助边
        // Todo
        // 一个节点是否可以拥有多条辅助边？
        // 当前只能有一个
        std::vector<std::map<std::string, std::string>> auxiliaryEdgeList;
        for (auto iter = commandObj.at("auxiliaryEdge").as_array().begin(); iter != commandObj.at("auxiliaryEdge").as_array().end(); ++iter) {
            std::map<std::string, std::string> edgeList;
            for (auto subIter = iter->as_object().begin(); subIter != iter->as_object().end(); ++subIter) {
                edgeList[subIter->key_c_str()] = subIter->value().as_string().c_str();
                // Todo
                // 辅助边应该具有方向且应该从被辅助点指向辅助点
                // 目前为了容错，将辅助的双向都进行了存储
                edgeList[subIter->value().as_string().c_str()] = subIter->key_c_str();
            }
            auxiliaryEdgeList.emplace_back(edgeList);
        }

        // 获取单次游走步长参数
        std::vector<float> walkLengthRatioList;
        for (auto iter = commandObj.at("walkLengthRatio").as_array().begin(); iter != commandObj.at("walkLengthRatio").as_array().end(); ++iter) {
            walkLengthRatioList.emplace_back(iter->as_double());
        }

        // 获取重启概率
        std::vector<float> restartRatioList;
        for (auto iter = commandObj.at("restartRatio").as_array().begin(); iter != commandObj.at("restartRatio").as_array().end(); ++iter) {
            restartRatioList.emplace_back(iter->as_double());
        }

        // 获取总游走步长
        std::vector<unsigned int> totalStepCountList;
        for (auto iter = commandObj.at("totalStepCount").as_array().begin(); iter != commandObj.at("totalStepCount").as_array().end(); ++iter) {
            totalStepCountList.emplace_back(iter->as_int64());
        }

        // 获取是否切分总步长
        std::vector<bool> isSplitStepCountList;
        for (auto iter = commandObj.at("isSplitStepCount").as_array().begin(); iter != commandObj.at("isSplitStepCount").as_array().end(); ++iter) {
            isSplitStepCountList.emplace_back(iter->as_bool());
        }

        // 获取目标点类型
        std::vector<std::string> targetNodeTypeList;
        for (auto iter = commandObj.at("targetNodeType").as_array().begin(); iter != commandObj.at("targetNodeType").as_array().end(); ++iter) {
            targetNodeTypeList.emplace_back(iter->as_string().c_str());
        }

        // 获取访问次数TopN
        std::vector<int> visitedCountTopNList;
        for (auto iter = commandObj.at("visitedCountTopN").as_array().begin(); iter != commandObj.at("visitedCountTopN").as_array().end(); ++iter) {
            visitedCountTopNList.emplace_back(iter->as_int64());
        }

        google::FlushLogFiles(google::INFO);

        // 游走
        graph.multiWalk(beginNodeTypeList, beginNodeIDList, stepDefineList, auxiliaryEdgeList, walkLengthRatioList, restartRatioList, totalStepCountList, isSplitStepCountList);

        unsigned int threadNum = 0;
        for (auto i = 0; i < beginNodeTypeList.size(); ++i) {

            if (boost::filesystem::exists(resultDirectoryPath + '/' + std::to_string(i))) {
                boost::filesystem::remove(resultDirectoryPath + '/' + std::to_string(i));
            }
            boost::filesystem::create_directory(resultDirectoryPath + '/' + std::to_string(i));

            for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
                std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(targetNodeTypeList[i], threadNum);
                // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
                unsigned int count = visitedCountTopNList[i];
                if (count > result.size()) count = result.size();
                std::ofstream resultFile;
                resultFile.open(resultDirectoryPath + "/" + std::to_string(i) + "/" + beginNodeTypeList[i] + ":" + iter->first + "_result.dat");
                for (auto k = 0; k < count; ++k) {
                    resultFile << result[k].first << ": " << result[k].second << std::endl;
                }
                resultFile.close();

                threadNum++;
            }
        }

        LOG(INFO) << "[执行完毕！]";
    }
}

arch::Out Command::questionRecall(arch::In &request, Graph &graph) {
    arch::Out result;

    /**
     * 多路召回
     */
    // 将知识点召回的知识点及其权重放入开始点列表
    std::vector<std::map<std::string, double>> beginNodeIDList;
    beginNodeIDList.emplace_back(request.current_knowledge_points);

    // 按题目召回
    std::map<std::string, double> questionBeginNodeIDList;
    for (auto iter = request.questions_assement.begin(); iter != request.questions_assement.end(); ++iter) {
        questionBeginNodeIDList[iter->first] = iter->second;
    }
    beginNodeIDList.emplace_back(questionBeginNodeIDList);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    // 多重游走
    graph.multiWalk(Command::questionRecallBeginNodeTypeList,
                    beginNodeIDList,
                    Command::questionRecallStepDefineList,
                    Command::questionRecallAuxiliaryEdgeList,
                    Command::questionRecallWalkLengthRatioList,
                    Command::questionRecallRestartRatioList,
                    Command::questionRecallTotalStepCountList,
                    Command::questionRecallIsSplitStepCountList,
                    false);
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "[INFO] 命令执行时间：" << programSpan.count() << "秒" << std::endl;
    /**
     * 多路合并策略
     */
    // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
    std::vector<std::pair<std::string, int>> recallList = graph.getSortedResultNodeTypeIDListByVisitedCount("Question");

    /**
     * 后过滤策略
     */
    // 过滤题目ID列表
    std::map<std::string, int32_t> filterQuestionList;

    // Todo 选择题过滤

    // 加入本节课的题目
    filterQuestionList.insert(request.questions_assement.begin(), request.questions_assement.end());
    // 加入前序课堂的题目
    filterQuestionList.insert(request.preceding_questions_assement.begin(), request.preceding_questions_assement.end());

    /**
     * 生成召回题目列表
     */

    // 设置召回返回题目个数
    int recallCount = request.expected;
    // 判断总召回题目个数是否小于期待召回题目个数
    // 若小于则设置期待召回题目个数为总召回题目个数
    if (recallList.size() < request.expected) recallCount = recallList.size();
    // 遍历召回题目列表
    for (auto i = 0; i < recallList.size(); ++i) {
        // 判断当前题目是否在过略列表里
        if (!filterQuestionList.contains(recallList[i].first)) {
            // 不在则加入返回列表
            result.payload.emplace_back(recallList[i].first);
        }
        // 判断返回题目数是否已满足期待召回个数
        if (result.payload.size() == recallCount) break;
    }

    result.code = 0;
    return result;
}

void Command::questionRecallInitialize(const std::string &configFilePath) {
    if (!boost::filesystem::exists(configFilePath)) {
        std::cerr << "[ERROR] JSON文件不存在！" << std::endl;
    }
    std::ifstream jsonFile(configFilePath);
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString(buffer.str());

    auto jsonObj = boost::json::parse(jsonString);

    auto stepDefineObj = jsonObj.as_object().at("stepDefine").as_object();
    Command::questionRecallStepDefineList.emplace_back(std::vector<std::string>());
    Command::questionRecallStepDefineList.emplace_back(std::vector<std::string>());
    for (auto iter = stepDefineObj["KnowledgePoint"].as_array().begin(); iter != stepDefineObj["KnowledgePoint"].as_array().end(); ++iter) {
        Command::questionRecallStepDefineList[0].emplace_back(iter->as_string().c_str());
    }
    for (auto iter = stepDefineObj["Question"].as_array().begin(); iter != stepDefineObj["Question"].as_array().end(); ++iter) {
        Command::questionRecallStepDefineList[1].emplace_back(iter->as_string().c_str());
    }

    auto auxiliaryEdgeObj = jsonObj.as_object().at("auxiliaryEdge").as_object();
    Command::questionRecallAuxiliaryEdgeList.emplace_back(std::map<std::string, std::string>());
    Command::questionRecallAuxiliaryEdgeList.emplace_back(std::map<std::string, std::string>());
    for (auto iter = auxiliaryEdgeObj["KnowledgePoint"].as_object().begin(); iter != auxiliaryEdgeObj["KnowledgePoint"].as_object().end(); ++iter) {
        Command::questionRecallAuxiliaryEdgeList[0][iter->key_c_str()] = iter->value().as_string().c_str();
    }
    for (auto iter = auxiliaryEdgeObj["Question"].as_object().begin(); iter != auxiliaryEdgeObj["Question"].as_object().end(); ++iter) {
        Command::questionRecallAuxiliaryEdgeList[1][iter->key_c_str()] = iter->value().as_string().c_str();
    }

    auto walkLengthRatioObj = jsonObj.as_object().at("walkLengthRatio").as_object();
    Command::questionRecallWalkLengthRatioList.emplace_back(walkLengthRatioObj.at("KnowledgePoint").as_double());
    Command::questionRecallWalkLengthRatioList.emplace_back(walkLengthRatioObj.at("Question").as_double());

    auto restartRatioObj = jsonObj.as_object().at("restartRatio").as_object();
    Command::questionRecallRestartRatioList.emplace_back(restartRatioObj.at("KnowledgePoint").as_double());
    Command::questionRecallRestartRatioList.emplace_back(restartRatioObj.at("Question").as_double());

    auto totalStepCountObj = jsonObj.as_object().at("totalStepCount").as_object();
    Command::questionRecallTotalStepCountList.emplace_back(totalStepCountObj.at("KnowledgePoint").as_int64());
    Command::questionRecallTotalStepCountList.emplace_back(totalStepCountObj.at("Question").as_int64());

    auto targetNodeTypeObj = jsonObj.as_object().at("targetNodeType").as_object();
    Command::questionRecallTargetNodeTypeList.emplace_back(targetNodeTypeObj.at("KnowledgePoint").as_string().c_str());
    Command::questionRecallTargetNodeTypeList.emplace_back(targetNodeTypeObj.at("Question").as_string().c_str());

    auto isSplitStepCountObj = jsonObj.as_object().at("isSplitStepCount").as_object();
    Command::questionRecallIsSplitStepCountList.emplace_back(isSplitStepCountObj.at("KnowledgePoint").as_bool());
    Command::questionRecallIsSplitStepCountList.emplace_back(isSplitStepCountObj.at("Question").as_bool());
}

/**
 * 静态成员变量初始化
 */
// 每一路召回的起点类型
std::vector<std::string> Command::questionRecallBeginNodeTypeList = {"KnowledgePoint", "Question"};
// 每一路召回对应的步长定义
std::vector<std::vector<std::string>> Command::questionRecallStepDefineList;
// 每一路召回对应的辅助边
std::vector<std::map<std::string, std::string>> Command::questionRecallAuxiliaryEdgeList;
// 每一路召回对应的单次游走步长参数
std::vector<float> Command::questionRecallWalkLengthRatioList;
// 每一路召回对应的重启概率
std::vector<float> Command::questionRecallRestartRatioList;
// 每一路召回对应的总游走步数
std::vector<unsigned int> Command::questionRecallTotalStepCountList;
// 每一路召回对应的召回目标
std::vector<std::string> Command::questionRecallTargetNodeTypeList;
// 每一路召回对应的总步数切分策略
std::vector<bool> Command::questionRecallIsSplitStepCountList;
