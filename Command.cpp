//
// Created by 翟霄 on 2021/8/3.
//

#include "Command.h"

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <boost/json.hpp>

#include "type_defination.h"

void Command::execute(Graph &graph, const std::string &command, const std::string &resultDirectoryPath) {
    auto commandObj = boost::json::parse(command);

    if (commandObj.at("name").as_string() == "exclude_node") {
        /**
         * 排除节点
         */
        std::vector<std::string> excludeNodeTypeIDList;
        for (auto iter = commandObj.at("type_id").as_array().begin(); iter != commandObj.at("type_id").as_array().end(); ++iter) {
            excludeNodeTypeIDList.push_back(iter->as_string().c_str());
        }
        std::cout << graph.getNodeList().at("KnowledgePoint:f9e12aee12214301b757841df388be97")->getLinkedNodeMapList().at("Question").first.size() << std::endl;
        graph.excludeNodes(excludeNodeTypeIDList);
        std::cout << graph.getNodeList().at("KnowledgePoint:f9e12aee12214301b757841df388be97")->getLinkedNodeMapList().at("Question").first.size() << std::endl;
    } else if (commandObj.at("name").as_string() == "traverse") {
        /**
         * 遍历
         * 深度/广度优先遍历
         */
//        auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
//        for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
//            std::cout << *i << std::endl;
//        }
    } else if (commandObj.at("name").get_string() == "walk") {
        /**
         * 游走
         * 随机游走
         */
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
        // 获取总游走步长
        unsigned int totalStepCount = commandObj.at("totalStepCount").as_int64();
        // 获取边选择策略
        std::string strategyString = commandObj.at("strategy").as_string().c_str();
        EdgeChooseStrategy strategy;
        if (strategyString == "FIRST") {
            strategy = EdgeChooseStrategy::FIRST;
        } else if (strategyString == "LAST") {
            strategy = EdgeChooseStrategy::LAST;
        } else if (strategyString == "RANDOM") {
            strategy = EdgeChooseStrategy::RANDOM;
        } else if (strategyString == "FIRST_NO_VISIT") {
            strategy = EdgeChooseStrategy::FIRST_NO_VISIT;
        } else if (strategyString == "LAST_NO_VISIT") {
            strategy = EdgeChooseStrategy::LAST_NO_VISIT;
        } else if (strategyString == "RANDOM_NO_VISIT") {
            strategy = EdgeChooseStrategy::RANDOM_NO_VISIT;
        }
        // 获取是否重置图状态
        bool resetGraph = commandObj.at("resetGraph").as_bool();
        // 获取目标点类型
        std::string targetNodeType = commandObj.at("targetNodeType").as_string().c_str();
        // 获取访问次数TopN
        int visitedCountTopN = commandObj.at("visitedCountTopN").as_int64();

        for (auto i = 0; i < beginNodeIDList.size(); ++i) {
            // 游走
            auto walkingSequence = graph.walk(beginNodeType,
                                              beginNodeIDList[i],
                                              stepDefine,
                                              auxiliaryEdge,
                                              walkLengthRatio,
                                              totalStepCount,
                                              resetGraph);

            // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
            std::vector<std::pair<std::string, int>> result = graph.getSortedNodeTypeIDListByVisitedCount(
                    walkingSequence, targetNodeType);
            // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
            if (visitedCountTopN > result.size()) visitedCountTopN = result.size();
            std::ofstream resultFile;
            resultFile.open(resultDirectoryPath + "/" + beginNodeType + ":" + beginNodeIDList[i] + "_result.dat");
            for (auto i = 0; i < visitedCountTopN; ++i) {
                resultFile << result[i].first << ": " << result[i].second << std::endl;
            }
            resultFile.close();
        }
    }
}