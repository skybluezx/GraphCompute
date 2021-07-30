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
    explicit Graph(const std::string &graphDefineFileDirectoryPath, const int &readEdgeCount = -1);

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

    /**
     * 获得指定ID的节点访问次数
     * @param id
     * @return
     */
    const int getNodeVisitedCount(const std::string &id) const;

    /**
     * 获得指定ID的节点类型
     * @param id
     * @return
     */
    const std::string getNodeType(const std::string &id) const;

    /**
     * 获得图中节点的全部类型列表
     * @return
     */
    std::vector<std::string> getNodeTypeList() const;

    /**
     * 遍历方法
     * 根据指定的开始节点遍历图中全部节点
     * @param beginNodeID 开始节点ID
     * @param type 遍历方式（深度或宽度）
     * @return 遍历顺序对应ID列表
     */
    std::vector<std::string> traverse(const std::string &beginNodeID, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const;
    // 遍历方法的多态
    // 传入开始点对象
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
                                  const std::map<std::string, std::string> &auxiliaryEdge,
                                  const float &walkLengthRatio,
                                  const int &totalStepCount,
                                  const EdgeChooseStrategy &strategy,
                                  const bool &resetGraph);
    // 游走方法的多态
    // 传入开始点对象
    std::vector<std::string> walk(const Node &beginNode,
                                  const std::vector<std::string> &stepDefine,
                                  const std::map<std::string, std::string> &auxiliaryEdge,
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
     * 获取访问列表中全部节点按照访问次数从大到小顺序的列表
     * 返回节点ID和访问次数所组成Pair的列表
     * @return
     */
    std::vector<std::pair<std::string, int>> getSortedNodeIDListByVisitedCount(const std::vector<std::string> &walkingSequence) const;

    /**
     * 获取访问列表中指定类型全部节点按照访问次数从大到小顺序的列表
     * 返回节点ID和访问次数所组成Pair的列表
     * @return
     */
    std::vector<std::pair<std::string, int>> getSortedNodeIDListByVisitedCount(const std::vector<std::string> &walkingSequence, const std::string &nodeType) const;

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

    /**
     * 节点访问次数的比较方法
     * 用于获取根据节点访问次数排序的列表
     * @param a
     * @param b
     * @return
     */
    static bool cmp(std::pair<std::string, int> a, std::pair<std::string, int> b);

    /**
     * 图中全部点的字典
     * key为点的ID
     * value为对应点的指针
     */
    std::map<const std::string, Node *const> nodeList;

    /**
     * 图中点的类型字典
     * Key为类型
     * Value为该类型对应的点个数
     */
    std::map<std::string, unsigned> nodeTypeCountList;
};


#endif //GRAPHCOMPUTE_GRAPH_H