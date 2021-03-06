//
// Created by 翟霄 on 2021/8/3.
//

#include "Command.h"

#include <vector>
#include <map>
#include <iostream>
#include <boost/json.hpp>
#include <math.h>

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
            beginNodeIDList.emplace_back(iter->as_string().c_str());
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
                resultFile << result[i].first << ":" << result[i].second << std::endl;
            }
            resultFile.close();
        }

        LOG(INFO) << "[执行完毕！]";
    } else if (commandObj.at("name").get_string() == "multi_walk") {
        /**
         * 多重随机游走
         */
        LOG(INFO) << "[多重随机游走]";

        // 创建相关参数列表
        std::vector<std::string> beginNodeTypeList;
        std::vector<std::map<std::string, double>> beginNodeIDList;
        std::vector<std::vector<std::string>> stepDefineList;
        std::vector<std::map<std::string, std::string>> auxiliaryEdgeList;
        std::vector<float> walkLengthRatioList;
        std::vector<float> restartRatioList;
        std::vector<unsigned int> totalStepCountList;
        std::vector<bool> isSplitStepCountList;

        // 获取开始点类型
        // 同时创建相关参数的默认值
        for (auto iter = commandObj.at("beginNodeType").as_array().begin(); iter != commandObj.at("beginNodeType").as_array().end(); ++iter) {
            beginNodeTypeList.emplace_back(iter->as_string().c_str());

            beginNodeIDList.emplace_back(std::map<std::string, double>());
            stepDefineList.emplace_back(std::vector<std::string>());
            auxiliaryEdgeList.emplace_back(std::map<std::string, std::string>());
            walkLengthRatioList.emplace_back(0);
            restartRatioList.emplace_back(0);
            totalStepCountList.emplace_back(0);
            isSplitStepCountList.emplace_back(false);
        }

        // 设置开始点ID
        auto i = 0;
        for (auto iter = commandObj.at("beginNodeID").as_array().begin(); iter != commandObj.at("beginNodeID").as_array().end(); ++iter) {
            for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter){
                beginNodeIDList[i][subIter->as_object().at("id").as_string().c_str()] = subIter->as_object().at("weight").as_double();
            }
            i++;
        }

        // 是否合并全部开始点的游走结果
        // Todo
        // 目前采用最大值合并，其他合并策略待开发
        bool isMergeMultiBeginNodeResult = commandObj.at("is_merge").as_bool();

        // 设置步的边组成
        i = 0;
        for (auto iter = commandObj.at("stepDefine").as_array().begin(); iter != commandObj.at("stepDefine").as_array().end(); ++iter) {
            for (auto subIter = iter->as_array().begin(); subIter != iter->as_array().end(); ++subIter) {
                stepDefineList[i].emplace_back(subIter->as_string().c_str());
            }
            i++;
        }

        // 设置辅助边
        // Todo
        // 一个节点是否可以拥有多条辅助边？
        // 当前只能有一个
        i = 0;
        for (auto iter = commandObj.at("auxiliaryEdge").as_array().begin(); iter != commandObj.at("auxiliaryEdge").as_array().end(); ++iter) {
            for (auto subIter = iter->as_object().begin(); subIter != iter->as_object().end(); ++subIter) {
                auxiliaryEdgeList[i][subIter->key_c_str()] = subIter->value().as_string().c_str();
                // Todo
                // 辅助边应该具有方向且应该从被辅助点指向辅助点
                // 目前为了容错，将辅助的双向都进行了存储
                auxiliaryEdgeList[i][subIter->value().as_string().c_str()] = subIter->key_c_str();
            }
            i++;
        }

        // 设置单次游走步长参数
        i = 0;
        for (auto iter = commandObj.at("walkLengthRatio").as_array().begin(); iter != commandObj.at("walkLengthRatio").as_array().end(); ++iter) {
            walkLengthRatioList[i] = iter->as_double();
            i++;
        }

        // 设置重启概率
        i = 0;
        for (auto iter = commandObj.at("restartRatio").as_array().begin(); iter != commandObj.at("restartRatio").as_array().end(); ++iter) {
            restartRatioList[i] = iter->as_double();
            i++;
        }

        // 设置总游走步长
        i = 0;
        for (auto iter = commandObj.at("totalStepCount").as_array().begin(); iter != commandObj.at("totalStepCount").as_array().end(); ++iter) {
            totalStepCountList[i] = iter->as_int64();
            i++;
        }

        // 设置是否切分总步长
        i = 0;
        for (auto iter = commandObj.at("isSplitStepCount").as_array().begin(); iter != commandObj.at("isSplitStepCount").as_array().end(); ++iter) {
            isSplitStepCountList[i] = iter->as_bool();
            i++;
        }

        // 获取目标点类型
        std::string targetNodeType;
        targetNodeType = commandObj.at("targetNodeType").as_string().c_str();

        // 获取访问次数TopN
        int visitedCountTopN;
        visitedCountTopN = commandObj.at("visitedCountTopN").as_int64();

        google::FlushLogFiles(google::INFO);

        if (isMergeMultiBeginNodeResult) {
            bool keepVisitedCount = false;
            unsigned int threadNum = 0;
            std::vector<unsigned int> threadNumList;
            std::vector<std::map<std::string, double>> currentBeginNodeIDListGroup(beginNodeTypeList.size());

            // 遍历开始点类型
            for (auto i = 0; i < beginNodeTypeList.size(); ++i) {
                // 根据图支持的最大并行开始点个数遍历游走轮数
                for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                    currentBeginNodeIDListGroup[i][iter->first] = iter->second;
                    threadNumList.emplace_back(threadNum);
                    threadNum++;

                    if (threadNum == graph.getMaxWalkBeginNodeCount()) {
                        // 多线程游走
                        graph.multiWalk(beginNodeTypeList,
                                        currentBeginNodeIDListGroup,
                                        stepDefineList,
                                        auxiliaryEdgeList,
                                        walkLengthRatioList,
                                        restartRatioList,
                                        totalStepCountList,
                                        isSplitStepCountList,
                                        keepVisitedCount);
                        graph.mergeResultList(threadNumList, graph.getMaxWalkBeginNodeCount());

                        keepVisitedCount = true;
                        threadNum = 0;
                        threadNumList.clear();
                        currentBeginNodeIDListGroup = std::vector<std::map<std::string, double>>(beginNodeTypeList.size());
                    }
                }
            }

            if (threadNum != 0) {
                // 游走
                std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

                graph.multiWalk(beginNodeTypeList,
                                currentBeginNodeIDListGroup,
                                stepDefineList,
                                auxiliaryEdgeList,
                                walkLengthRatioList,
                                restartRatioList,
                                totalStepCountList,
                                isSplitStepCountList,
                                keepVisitedCount);
                graph.mergeResultList(threadNumList, graph.getMaxWalkBeginNodeCount());

                std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
                LOG(INFO) << "[INFO] 单次游走时长：" << programSpan.count() << "秒";
                google::FlushLogFiles(google::INFO);
            }

            // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
            std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(targetNodeType, graph.getMaxWalkBeginNodeCount());

            // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
            unsigned int count = visitedCountTopN;
            if (count > result.size()) count = result.size();
            std::ofstream resultFile;
            resultFile.open(resultDirectoryPath + "/merged_result.dat");
            for (auto i = 0; i < count; ++i) {
                resultFile << result[i].first << ": " << result[i].second << std::endl;
            }
            resultFile.close();
        } else {
            bool keepVisitedCount = false;
            // 初始化当前线程索引为0
            unsigned int threadNum = 0;
            // 初始化线程索引数组
            std::vector<unsigned int> threadNumList;
            // 初始化每次游走开始点列表
            std::vector<std::map<std::string, double>> currentBeginNodeIDListGroup(beginNodeTypeList.size());
            // 初始化每次游走的线程索引与开始点ID的对应关系
            std::map<unsigned int, std::string> threadBeginNodeIDList;

            // 遍历开始点类型
            for (auto i = 0; i < beginNodeTypeList.size(); ++i) {
                // 清理结果目录
                if (boost::filesystem::exists(resultDirectoryPath + '/' + std::to_string(i))) {
                    boost::filesystem::directory_iterator directoryIterator(resultDirectoryPath + '/' + std::to_string(i));
                    boost::filesystem::directory_iterator directoryIteratorEnd;
                    for (; directoryIterator != directoryIteratorEnd; ++directoryIterator) {
                        if (!boost::filesystem::is_regular_file(directoryIterator->status())) continue;
                        std::string filename = directoryIterator->path().filename().string();
                        boost::filesystem::remove(directoryIterator->path());
                    }
                    boost::filesystem::remove(resultDirectoryPath + '/' + std::to_string(i));
                }
                boost::filesystem::create_directory(resultDirectoryPath + '/' + std::to_string(i));

                // 根据图支持的最大并行开始点个数遍历游走轮数
                for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                    currentBeginNodeIDListGroup[i][iter->first] = iter->second;
                    threadBeginNodeIDList[threadNum] = iter->first;
                    threadNumList.emplace_back(threadNum);
                    threadNum++;

                    // 判断是否凑够最大并行数能够开启一次多线程游走
                    if (threadNum == graph.getMaxWalkBeginNodeCount()) {
                        // 多线程游走
                        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                        graph.multiWalk(beginNodeTypeList,
                                        currentBeginNodeIDListGroup,
                                        stepDefineList,
                                        auxiliaryEdgeList,
                                        walkLengthRatioList,
                                        restartRatioList,
                                        totalStepCountList,
                                        isSplitStepCountList,
                                        keepVisitedCount);
                        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                        std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
                        LOG(INFO) << "[INFO] 单次游走时长：" << programSpan.count() << "秒";
                        google::FlushLogFiles(google::INFO);

                        // 多线程生成每个开始点的游走结果
                        std::vector<std::thread> threadList;
                        for (auto threadIndex = 0; threadIndex < threadNumList.size(); ++threadIndex) {
                            // 生成游走结果的保存路径
                            std::string filePath = resultDirectoryPath + "/" + std::to_string(i) + "/" + beginNodeTypeList[i] + ":" + threadBeginNodeIDList[threadIndex] + "_result.dat";
                            threadList.emplace_back(Command::visitedCountListToFile,
                                                    std::cref(graph),
                                                    std::cref(threadNumList[threadIndex]),
                                                    std::cref(targetNodeType),
                                                    // 不能是引用，因为filePath是在for循环内生成的，当前线程生成结束后filePath变量会失效
                                                    std::move(filePath),
                                                    std::cref(visitedCountTopN));
                        }
                        for (auto j = 0; j < threadList.size(); ++j) {
                            if (threadList[j].joinable()) {
                                threadList[j].join();
                            }
                        }
                        threadList.clear();

                        keepVisitedCount = true;
                        // 清空本次多线程运行状态
                        // 重新开始遍历下一批线程
                        threadNum = 0;
                        threadNumList.clear();
                        currentBeginNodeIDListGroup = std::vector<std::map<std::string, double>>(beginNodeTypeList.size());
                    }
                }

                // 判断是否还有未凑够最大并行游走
                if (threadNum != 0) {
                    // 多线程游走
                    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                    graph.multiWalk(beginNodeTypeList,
                                    currentBeginNodeIDListGroup,
                                    stepDefineList,
                                    auxiliaryEdgeList,
                                    walkLengthRatioList,
                                    restartRatioList,
                                    totalStepCountList,
                                    isSplitStepCountList,
                                    keepVisitedCount);
                    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
                    LOG(INFO) << "[INFO] 单次游走时长：" << programSpan.count() << "秒";
                    google::FlushLogFiles(google::INFO);

                    // 多线程生成每个开始点的游走结果
                    std::vector<std::thread> threadList;
                    for (auto threadIndex = 0; threadIndex < threadNumList.size(); ++threadIndex) {
                        // 生成游走结果的保存路径
                        std::string filePath = resultDirectoryPath + "/" + std::to_string(i) + "/" + beginNodeTypeList[i] + ":" + threadBeginNodeIDList[threadIndex] + "_result.dat";
                        threadList.emplace_back(Command::visitedCountListToFile,
                                                std::cref(graph),
                                                std::cref(threadNumList[threadIndex]),
                                                std::cref(targetNodeType),
                                                // 不能是引用，因为filePath是在for循环内生成的，当前线程生成结束后filePath变量会失效
                                                std::move(filePath),
                                                std::cref(visitedCountTopN));
                    }
                    for (auto j = 0; j < threadList.size(); ++j) {
                        if (threadList[j].joinable()) {
                            threadList[j].join();
                        }
                    }
                    threadList.clear();

                    // 清空本次多线程运行状态
                    // 重新开始遍历下一批线程
                    threadNum = 0;
                    threadNumList.clear();
                    currentBeginNodeIDListGroup = std::vector<std::map<std::string, double>>(beginNodeTypeList.size());
                }
            }
        }

        LOG(INFO) << "[执行完毕！]"; }
}

arch::Out Command::questionRecall(const arch::In &request, Graph &graph) {
    arch::Out result;
    /**
     * 多路召回
     */
    // 计算总开始点个数(用于将总步数分配至多轮游走)
    unsigned int beginNodeCount = request.current_knowledge_points.size() + request.questions_assement.size();
    // 初始化是否保留访问次数为false（用于清除上一次请求的访问次数）
    bool keepVisitedCount = false;

    // 每个知识点召回的题目个数列表清空
    Command::questionRecallKnowledgePointQuestionCountList.clear();
    // 初始化知识点召回和题目召回Map
    Command::beginNodeIDList[0].clear();
    Command::beginNodeIDList[1].clear();

    unsigned int threadNum = 0;
    std::vector<unsigned int> threadNumList;
    auto kpIter = request.current_knowledge_points.begin();
    auto quIter = request.questions_assement.begin();

    // 遍历全部待召回知识点和题目
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    // 遍历全部待召回知识点和题目
    while (kpIter != request.current_knowledge_points.end() || quIter != request.questions_assement.end()) {
        // 当开始点个数尚未达到最大线程数且存在尚未召回知识点时
        if (threadNum < graph.getMaxWalkBeginNodeCount() && kpIter != request.current_knowledge_points.end()) {
            Command::beginNodeIDList[0][kpIter->first] = kpIter->second;
            kpIter++;
            threadNumList.emplace_back(threadNum);
            threadNum++;
        }
        // 当开始点个数尚未达到最大线程数且存在尚未召回题目时
        if (threadNum < graph.getMaxWalkBeginNodeCount() && quIter != request.questions_assement.end()) {
            Command::beginNodeIDList[1][quIter->first] = quIter->second;
            quIter++;
            threadNumList.emplace_back(threadNum);
            threadNum++;
        }
        // 当凑够一轮游走所需的线程数或者是最后一轮游走时
        if (threadNum == graph.getMaxWalkBeginNodeCount() || (kpIter == request.current_knowledge_points.end() && quIter == request.questions_assement.end())) {
            if (Command::questionRecallIsSplitStepCount) {
                Command::questionRecallTotalStepCountList[0] = float(threadNum) / beginNodeCount * Command::questionRecallTotalStepCount;
                Command::questionRecallTotalStepCountList[1] = float(threadNum) / beginNodeCount * Command::questionRecallTotalStepCount;
            } else {
                Command::questionRecallTotalStepCountList[0] = Command::questionRecallTotalStepCount;
                Command::questionRecallTotalStepCountList[1] = Command::questionRecallTotalStepCount;
            }

            // 多重游走
            // Todo
            graph.multiWalk1(Command::questionRecallBeginNodeTypeList,
                            Command::beginNodeIDList,
                            Command::questionRecallRestartRatioList,
                            Command::questionRecallTotalStepCountList,
                            keepVisitedCount);
            graph.mergeResultList(threadNumList, graph.getMaxWalkBeginNodeCount());

            keepVisitedCount = true;
            threadNum = 0;
            threadNumList.clear();
            Command::beginNodeIDList = std::vector<std::map<std::string, double>>(2);
        }
    }
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
    LOG(INFO) << "[INFO] 游走时长：" << programSpan.count() << "秒" << std::endl;

    /**
     * 获取游走结果
     */
    // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
    std::vector<std::pair<std::string, int>> recallList = graph.getSortedResultNodeTypeIDListByVisitedCount("Question", graph.getMaxWalkBeginNodeCount());

    /**
     * 后过滤策略初始化
     */
    // 过滤题目ID列表
    std::map<std::string, int32_t> filterQuestionList;
    // 加入本节课的题目
    filterQuestionList.insert(request.questions_assement.begin(), request.questions_assement.end());
    // 加入前序课堂的题目
    filterQuestionList.insert(request.preceding_questions_assement.begin(), request.preceding_questions_assement.end());

    // 初始化本节课中题目对应的每个知识点的最大难度
    // Todo
    // 过滤本节课还是前序所有课？
    std::unordered_map<std::string, unsigned int> kpMaxHard;
//    // 初始化本节课中题目的全局最大难度
//    // Todo
//    // 过滤本节课还是前序所有课
//    int32_t courseMaxHard = 0;
    for(auto iter = request.questions_assement.begin(); iter != request.questions_assement.end(); ++iter){
//        // 刷新全局难度
//        if (Command::questionRecallQuestionHardFilterMap["Question:" + iter->first] > courseMaxHard){
//            courseMaxHard = iter->second;
//        }
        // 判断当前题目是否在题库中
        if (graph.getNodeList().contains("Question:" + iter->first)) {
            // 判断当前题目是否拥有知识点
            if (graph.getNodeList().at("Question:" + iter->first)->getLinkedNodeMapList().contains("KnowledgePoint")) {
                // 获取当前题目的知识点列表
                auto &knowledgePointList = graph.getNodeList().at("Question:" + iter->first)->getLinkedNodeMapList().at("KnowledgePoint");
                // 将当前题目知识点对应最大难度加入列表
                for (auto kpIter = knowledgePointList.begin(); kpIter != knowledgePointList.end(); ++kpIter) {
                    // 判断当前题目的难度是否大于题目所属知识点的当前最大难度
                    if (Command::questionRecallQuestionHardFilterMap["Question:" + iter->first] > kpMaxHard[(*kpIter)->getID()]) {
                        kpMaxHard[(*kpIter)->getID()] = Command::questionRecallQuestionHardFilterMap["Question:" + iter->first];
                    }
                }
            }
        }
    }
    
    /**
     * 生成召回题目列表
     */
    // 设置召回返回题目个数
    int recallCount = request.expected;
    // 判断总召回题目个数是否小于期待召回题目个数
    // 若小于则设置期待召回题目个数为总召回题目个数
    if (recallList.size() < request.expected) recallCount = recallList.size();
    // 遍历召回题目列表
    for (auto iter = recallList.begin(); iter != recallList.end(); ++iter) {
        // 判断当前题目是否在过略列表里
        if (!filterQuestionList.contains(graph.getNodeList().at(iter->first)->getID())) {
            /**
             * 题目属性过滤
             */

            // 选择题过滤
            if (Command::questionRecallMultipleChoiceQuestionFilterMap[iter->first] != 1) {
                continue;
            }

            // 是否自动判卷过滤
            if (Command::questionRecallIsDecidableFilterMap[iter->first] == 0) {
                continue;
            }

//            // 全局难度过滤
//            if (courseMaxHard > 0 && Command::questionRecallQuestionHardFilterMap[iter->first] > courseMaxHard) {
//                continue;
//            }

            /**
             * 题目-知识点维度过滤
             */

            // 判断当前题目是否有对应知识点
            if (graph.getNodeList().at(iter->first)->getFirstLinkedNode("KnowledgePoint") == nullptr) {
                continue;
            }
            // [取消]判断当前题目是否在本堂课的知识点范围内
            // [取消]判断当前题目是否在本节课上题目对应的知识点范围内
            // [启用]判断当前题目是否在本节课上题目对应的知识点范围内且难度不大于当前知识点对应的最大难度
            auto &questionKp = graph.getNodeList().at(iter->first)->getFirstLinkedNode("KnowledgePoint")->getID();
            if (!kpMaxHard.contains(questionKp)) {
                // 召回的题目不包含课上题目对应知识点
                continue;
            } else {
                // 召回的题目包含课上题目对应知识点
                if (kpMaxHard[questionKp] > 0 && Command::questionRecallQuestionHardFilterMap[iter->first] > kpMaxHard[questionKp]) {
                    continue;
                }
            }
            // 判断当前题目对应的知识点是否已召回足够多题目
            questionRecallKnowledgePointQuestionCountList[graph.getNodeList().at(iter->first)->getFirstLinkedNode("KnowledgePoint")->getID()]++;
            if (questionRecallKnowledgePointQuestionCountList[graph.getNodeList().at(iter->first)->getFirstLinkedNode("KnowledgePoint")->getID()] < request.expected / request.current_knowledge_points.size()) {
                // 未超过占比则加入返回列表
                result.payload.emplace_back(graph.getNodeList().at(iter->first)->getID());
            }
        }
        // 判断返回题目数是否已满足期待召回个数
        if (result.payload.size() == recallCount) break;
    }

    result.code = 0;
    return result;
}

bool Command::questionRecallInitialize(const std::string &configFilePath) {
    // 判断文件是否存在
    if (!boost::filesystem::exists(configFilePath)) {
        std::cerr << "[ERROR] JSON文件不存在！" << std::endl;
        return false;
    }
    // 解析配置文件
    std::ifstream jsonFile(configFilePath);
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonString(buffer.str());
    auto jsonObj = boost::json::parse(jsonString);

    // 读取知识点召回和题目召回的步长定义
    auto stepDefineObj = jsonObj.as_object().at("stepDefine").as_object();
    Command::questionRecallStepDefineList.emplace_back(std::vector<std::string>());
    Command::questionRecallStepDefineList.emplace_back(std::vector<std::string>());
    for (auto iter = stepDefineObj["KnowledgePoint"].as_array().begin(); iter != stepDefineObj["KnowledgePoint"].as_array().end(); ++iter) {
        Command::questionRecallStepDefineList[0].emplace_back(iter->as_string().c_str());
    }
    for (auto iter = stepDefineObj["Question"].as_array().begin(); iter != stepDefineObj["Question"].as_array().end(); ++iter) {
        Command::questionRecallStepDefineList[1].emplace_back(iter->as_string().c_str());
    }
    // 读取知识点召回和题目召回的辅助边
    auto auxiliaryEdgeObj = jsonObj.as_object().at("auxiliaryEdge").as_object();
    Command::questionRecallAuxiliaryEdgeList.emplace_back(std::map<std::string, std::string>());
    Command::questionRecallAuxiliaryEdgeList.emplace_back(std::map<std::string, std::string>());
    for (auto iter = auxiliaryEdgeObj["KnowledgePoint"].as_object().begin(); iter != auxiliaryEdgeObj["KnowledgePoint"].as_object().end(); ++iter) {
        Command::questionRecallAuxiliaryEdgeList[0][iter->key_c_str()] = iter->value().as_string().c_str();
    }
    for (auto iter = auxiliaryEdgeObj["Question"].as_object().begin(); iter != auxiliaryEdgeObj["Question"].as_object().end(); ++iter) {
        Command::questionRecallAuxiliaryEdgeList[1][iter->key_c_str()] = iter->value().as_string().c_str();
    }
    // 读取知识点召回和题目的步长参数
    auto walkLengthRatioObj = jsonObj.as_object().at("walkLengthRatio").as_object();
    Command::questionRecallWalkLengthRatioList.emplace_back(walkLengthRatioObj.at("KnowledgePoint").as_double());
    Command::questionRecallWalkLengthRatioList.emplace_back(walkLengthRatioObj.at("Question").as_double());
    // 读取知识点召回和题目召回的重启概率
    auto restartRatioObj = jsonObj.as_object().at("restartRatio").as_object();
    Command::questionRecallRestartRatioList.emplace_back(restartRatioObj.at("KnowledgePoint").as_double());
    Command::questionRecallRestartRatioList.emplace_back(restartRatioObj.at("Question").as_double());
    // 读取总游走步数
    Command::questionRecallTotalStepCount = jsonObj.as_object().at("totalStepCount").as_int64();
    // 读取是否切分总游走步数
    Command::questionRecallIsSplitStepCount = jsonObj.as_object().at("isSplitStepCount").as_bool();

    // 初始化开始点列表
    // 初始化知识点开始点列表
    Command::beginNodeIDList.emplace_back(std::map<std::string, double>());
    // 初始化题目开始点列表
    Command::beginNodeIDList.emplace_back(std::map<std::string, double>());

    // 初始化总游走步数列表
    // 初始胡知识点开始点列表
    Command::questionRecallTotalStepCountList.emplace_back(0);
    // 初始化题目开始点列表
    Command::questionRecallTotalStepCountList.emplace_back(0);

    // 初始化是否切分部分列表
    // 初始胡知识点开始点列表
    Command::questionRecallIsSplitStepCountList.emplace_back(false);
    // 初始化题目开始点列表
    Command::questionRecallIsSplitStepCountList.emplace_back(false);

    Command::questionRecallFilterPath = jsonObj.as_object().at("filter_file_path").as_string().c_str();
    // 读取过滤初始化文件 （questionid;hard;isMultipleChoice;isDecidable）
    LOG(INFO) << "[读取过滤文件]";
    std::ifstream excludeFile(Command::questionRecallFilterPath, std::ios::binary);
    if (!excludeFile.is_open()) {
        // 打开失败则输出错误日志
        LOG(ERROR) << "文件读取失败！";
    } else {
        // 建立和文件大小相同的内存空间
        std::vector<char> buf(excludeFile.seekg(0, std::ios::end).tellg());
        // 将文件中的数据全部读入内从
        excludeFile.seekg(0, std::ios::beg).read(&buf[0], static_cast<std::streamsize>(buf.size()));
        // 关闭文件释放空间
        excludeFile.close();

        // 初始化过滤文件每一行的四列
        std::string questionID, hard, isMultipleChoice, isDecidable;
        // 初始化对应标志为false
        bool questionIDFlag = false, hardFlag = false, isMultipleChoiceFlag = false, isDecidableFlag = false;
        // 初始化左游标为开始位置
        auto beginIter = buf.begin();
        for (auto iter = buf.begin(); iter != buf.end(); ++iter) {
            // 当前字段为制表符时
            if (*iter == '\t') {
                if (!questionIDFlag) {
                    questionID.insert(questionID.begin(), beginIter, iter);
                    questionIDFlag = true;

                    beginIter = iter + 1;
                } else if (!hardFlag) {
                    if (beginIter != iter) {
                        hard.insert(hard.begin(), beginIter, iter);
                        hardFlag = true;
                    }
                    beginIter = iter + 1;
                } else if (!isMultipleChoiceFlag) {
                    if (beginIter != iter) {
                        isMultipleChoice.insert(isMultipleChoice.begin(), beginIter, iter);
                        isMultipleChoiceFlag = true;
                    }
                    beginIter = iter + 1;
                }
            }

            if (*iter == '\n') {
                if (beginIter != iter) {
                    isDecidable.insert(isDecidable.begin(), beginIter, iter);
                    isDecidableFlag = true;
                }
                beginIter = iter + 1;

                if (questionIDFlag && hardFlag && isMultipleChoiceFlag && isDecidableFlag) {
                    // 难度map
                    Command::questionRecallQuestionHardFilterMap.insert(std::pair<std::string, int> ("Question:" + questionID, std::stoi(hard)));
                    // 单选map
                    Command::questionRecallMultipleChoiceQuestionFilterMap.insert(std::pair<std::string, int> ("Question:" + questionID, std::stoi(isMultipleChoice)));
                    // 自动判卷map
                    Command::questionRecallIsDecidableFilterMap.insert(std::pair<std::string, int> ("Question:" + questionID, std::stoi(isDecidable)));
                }

                questionIDFlag = false;
                hardFlag = false;
                isMultipleChoiceFlag = false;
                isDecidableFlag = false;

                questionID.clear();
                hard.clear();
                isMultipleChoice.clear();
                isDecidable.clear();
            }
        }
    }
    LOG(INFO) << "[读取过滤文件完成]";

    return true;
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
// 总游走步数
unsigned int Command::questionRecallTotalStepCount;
// 总步数切分策略
bool Command::questionRecallIsSplitStepCount;

std::vector<std::map<std::string, double>> Command::beginNodeIDList;
std::vector<unsigned int> Command::questionRecallTotalStepCountList;
std::vector<bool> Command::questionRecallIsSplitStepCountList;

std::map<std::string, unsigned int> Command::questionRecallKnowledgePointQuestionCountList;

// 过滤文件路径
std::string Command::questionRecallFilterPath;
// 难度过滤策略
std::unordered_map<std::string, int> Command::questionRecallQuestionHardFilterMap;
// 单选题过滤策略
std::unordered_map<std::string, int> Command::questionRecallMultipleChoiceQuestionFilterMap;
// 自动判卷过滤策略
std::unordered_map<std::string, int> Command::questionRecallIsDecidableFilterMap;

void Command::visitedCountListToFile(const Graph &graph,
                                     const int &threadNum,
                                     const std::string &nodeType,
                                     // 不能是引用，否则将导致线程运行过程中该变量失效
                                     const std::string filePath,
                                     const unsigned int &visitedCountTopN) {
    // 获取指定线程编号、指定节点类型的图计算结果
    std::vector<std::pair<std::string, int>> result = graph.getSortedResultNodeTypeIDListByVisitedCount(nodeType, threadNum);
    // 输出游走序列中指定点按访问次数由大到小排序的TopN节点信息
    unsigned int count = visitedCountTopN;
    if (count > result.size()) count = result.size();
    std::ofstream resultFile;
    resultFile.open(filePath);
    for (auto i = 0; i < count; ++i) {
        resultFile << result[i].first << ":" << result[i].second << std::endl;
    }
    resultFile.close();
}
