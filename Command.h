//
// Created by 翟霄 on 2021/8/3.
//

#ifndef GRAPHCOMPUTE_COMMAND_H
#define GRAPHCOMPUTE_COMMAND_H

#include <string>

#include "Graph.h"

// 题目召回接口头文件
#include "args.h"

class Command {
public:
    /**
     * 通用图操作命令
     * @param graph
     * @param command
     * @param resultDirectoryPath
     */
    static void execute(Graph &graph, const std::string &command, const std::string &resultDirectoryPath);

    /**
     * 题目召回命令
     * @param request
     * @return
     */
    static arch::Out questionRecall(const arch::In &request, Graph &graph);

    /**
     * 题目召回配置文件读取方法
     * @param configFilePath
     */
    static void questionRecallInitialize(const std::string &configFilePath);

private:
    /**
     * 题目召回的配置信息
     */
    // 每一路召回的起点类型
    static std::vector<std::string> questionRecallBeginNodeTypeList;
    // 每一路召回对应的步长定义
    static std::vector<std::vector<std::string>> questionRecallStepDefineList;
    // 每一路召回对应的辅助边
    static std::vector<std::map<std::string, std::string>> questionRecallAuxiliaryEdgeList;
    // 每一路召回对应的单次游走步长参数
    static std::vector<float> questionRecallWalkLengthRatioList;
    // 每一路召回对应的重启概率
    static std::vector<float> questionRecallRestartRatioList;
    // 每一路召回对应的总游走步数
    static std::vector<unsigned int> questionRecallTotalStepCountList;
    // 每一路召回对应的召回目标
    static std::vector<std::string> questionRecallTargetNodeTypeList;
    // 每一路召回对应的总步数切分策略
    static std::vector<bool> questionRecallIsSplitStepCountList;

    /**
     * 多线程获取图计算结果的线程体
     * @param graph             图的引用
     * @param threadNum         当前线程编号（用于到图结构的获取指定线程的计算结果）
     * @param nodeType          获取计算结果的节点类型
     * @param filePath          结果的存储路径（生成线程体时生成，所以不能使用引用）
     * @param visitedCountTopN  获取的TopN
     */
    static void visitedCountListToFile(const Graph &graph,
                                       const int &threadNum,
                                       const std::string &nodeType,
                                       const std::string filePath,
                                       const unsigned int &visitedCountTopN);
};


#endif //GRAPHCOMPUTE_COMMAND_H
