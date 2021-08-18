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
            graph.walk(beginNodeType, beginNodeIDList[i], stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount);

            // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
            std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(targetNodeType);
            // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
            if (visitedCountTopN > result.size()) visitedCountTopN = result.size();
            std::ofstream resultFile;
            resultFile.open(resultDirectoryPath + "/" + beginNodeType + ":" + beginNodeIDList[i] + "_result.dat");
            for (auto i = 0; i < visitedCountTopN; ++i) {
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
        std::vector<std::vector<std::string>> beginNodeIDList ;
        for (auto iter = commandObj.at("beginNodeID").as_array().begin(); iter != commandObj.at("beginNodeID").as_array().end(); ++iter) {
            std::vector<std::string> idList;
            for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter){
                idList.emplace_back(subIter->as_string().c_str());
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

        // 游走
        graph.multiWalk(beginNodeTypeList, beginNodeIDList, stepDefineList, auxiliaryEdgeList, walkLengthRatioList, restartRatioList, totalStepCountList);

//        // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
//        std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(targetNodeType);
//        // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
//        if (visitedCountTopN > result.size()) visitedCountTopN = result.size();
//        std::ofstream resultFile;
//        resultFile.open(resultDirectoryPath + "/" + beginNodeType + ":" + beginNodeIDList[i] + "_result.dat");
//        for (auto i = 0; i < visitedCountTopN; ++i) {
//            resultFile << result[i].first << ": " << result[i].second << std::endl;
//        }
//        resultFile.close();

        LOG(INFO) << "[执行完毕！]";
    }
}

Out Command::questionRecall(In &request) {
    Out result;
    result.transaction_id = request.transaction_id;

    for (auto iter = request.current_knowledge_points.begin(); iter != request.current_knowledge_points.end(); ++iter) {

    }

    for (auto iter = request.questions_assement.begin(); iter != request.questions_assement.end(); ++iter) {

    }

    result.code = 0;

    return result;
}