//
// Created by 翟霄 on 2021/7/16.
//

#ifndef GRAPHCOMPUTE_GRAPH_H
#define GRAPHCOMPUTE_GRAPH_H

#include <map>
#include <vector>
#include <string>

#include "Util.h"
#include "type_defination.h"
#include "Node.h"

class Graph {
public:
    /**
     * 默认构造方法
     */
    explicit Graph();

    /**
     * 构造方法
     * 读取图定义文件所在的文件建立图
     * @param graphDefineFileDirectoryPath
     */
    explicit Graph(const std::string &graphDefineFileDirectoryPath);

    /**
     * 析构方法
     * 负责将图中存储在堆内存中的点释放
     */
    ~Graph();

    /**
     * 获取图的节点列表
     * Map形式，Key节点ID，Value为指向节点对象的指针常量
     * @return
     */
    const std::map<const std::string, Node *const> &getNodeList() const;

    const int getNodeVisitedCount(const std::string &id) const;

    const std::string getNodeType(const std::string &id) const;

    std::vector<std::string> getNodeTypeList() const;

    /**
     * 遍历方法
     * 根据指定的开始节点遍历图中全部节点
     * @param beginNodeID 开始节点ID
     * @param type 遍历方式（深度或宽度）
     * @return 遍历顺序对应ID列表
     */
    std::vector<std::string> traverse(const std::string &beginNodeID, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const;
    /**
     * 遍历方法
     * 根据指定的开始节点遍历图中全部节点
     * @param node 开始节点
     * @param type 遍历方式（深度或宽度）
     * @param strategy
     * @return 遍历顺序对应ID列表
     */
    std::vector<std::string> traverse(const Node &beginNode, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const;

    /**
     * 游走方法
     * @param beginNodeID
     * @param stepDefine
     * @param auxiliaryStep
     * @param walkLengthRatio
     * @param totalStepCount
     * @param type
     * @param strategy
     * @return
     */
    std::vector<std::string> walk(const std::string &beginNodeID,
                                  const std::vector<std::string> &stepDefine,
                                  const std::map<std::string, std::string> &auxiliaryStep,
                                  const float &walkLengthRatio,
                                  const int &totalStepCount,
                                  const EdgeChooseStrategy &strategy,
                                  const bool &resetGraph);

    std::vector<std::string> walk(const Node &beginNode,
                                  const std::vector<std::string> &stepDefine,
                                  const std::map<std::string, std::string> &auxiliaryStep,
                                  const float &walkLengthRatio,
                                  const int &totalStepCount,
                                  const EdgeChooseStrategy &strategy,
                                  const bool &resetGraph);

    /**
     * 图重置方法
     * 重置图中全部节点用于记录遍历/游走的状态信息
     */
    void reset();

    /**
     *
     * @return
     */
    std::vector<std::pair<std::string, int>> getSortedNodeIDTypeListByVisitedCount(const std::vector<std::string> &walkingSequence) const;

//    template<class Archive> void serialize(Archive & ar, const unsigned int version);
private:
    /**
     * 遍历的递归方法
     * @param traverseSequenceList
     * @param currentNode
     * @param direction
     * @param strategy
     */
    static void traverse(std::vector<std::string> &traverseSequenceList,
                  Node *const &beginNode,
                  const WalkingDirection &direction,
                  const EdgeChooseStrategy &strategy);

    static bool cmp(std::pair<std::string, int> a, std::pair<std::string, int> b);

    /**
     * 图中全部点的字典
     * key为点的ID
     * value为对应点的指针
     */
    std::map<const std::string, Node *const> nodeList;

    std::map<std::string, unsigned> nodeTypeCountList;

    /**
     * 图计算部分
     */
};


#endif //GRAPHCOMPUTE_GRAPH_H