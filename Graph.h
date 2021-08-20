//
// Created by 翟霄 on 2021/7/16.
//

#ifndef GRAPHCOMPUTE_GRAPH_H
#define GRAPHCOMPUTE_GRAPH_H

#include <map>
#include <vector>
#include <string>
#include <future>

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
    explicit Graph(const std::string &graphDefineFileDirectoryPath, const std::string &resultType="walking_sequence", const int &readEdgeCount = -1);

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
    const int getNodeVisitedCount(const std::string &id, const std::string &type) const;

    /**
     * 获得图中节点的全部类型列表
     * @return
     */
    std::vector<std::string> getNodeTypeList() const;

    /**
     * 获取节点类型个数列表
     * @return
     */
    const std::map<std::string, unsigned> &getNodeTypeCountList() const;

    /**
     * 获取当前图中每个节点的分类型度数列表
     * @return
     */
    const std::map<std::string, std::map<std::string, int>> &getNodeDegreeList() const;

    /**
     * 获取当前图中全部节点类型对应的分类型最大度数列表
     * @return
     */
    const std::map<std::string, std::map<std::string, int>> &getNodeTypeMaxDegreeList() const;

    /**
     * 遍历方法
     * 根据指定的开始节点遍历图中全部节点
     * @param beginNodeID 开始节点ID
     * @param type 遍历方式（深度或宽度）
     * @return 遍历顺序对应ID列表
     */
    std::vector<std::string> traverse(const std::string &beginNodeType,
                                      const std::string &beginNodeID,
                                      const WalkingDirection &type,
                                      const EdgeChooseStrategy &strategy,
                                      const bool &keepVisitedCount = false) const;
    // 遍历方法的多态
    // 传入开始点对象
    std::vector<std::string> traverse(const Node &beginNode,
                                      const WalkingDirection &type,
                                      const EdgeChooseStrategy &strategy,
                                      const bool &keepVisitedCount = false) const;

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
    void walk(const std::string &beginNodeType,
              const std::string &beginNodeID,
              const std::vector<std::string> &stepDefine,
              const std::map<std::string, std::string> &auxiliaryEdge,
              const float &walkLengthRatio,
              const float &restartRatio,
              const unsigned int &totalStepCount);
    // 游走方法的多态
    // 传入开始点对象
    void walk(const Node &beginNode,
                      const std::vector<std::string> &stepDefine,
                      const std::map<std::string, std::string> &auxiliaryEdge,
                      const float &walkLengthRatio,
                      const float &restartRatio,
                      const unsigned int &totalStepCount);

    /**
     * 游走方法（线程版）
     * @param beginNodeType
     * @param beginNodeID
     * @param stepDefine
     * @param auxiliaryEdge
     * @param walkLengthRatio
     * @param restartRatio
     * @param totalStepCount
     * @param threadNum
     * @param keepVisitedCount
     */
    void walkOnThread(const std::string &beginNodeType,
              const std::string &beginNodeID,
              const std::vector<std::string> &stepDefine,
              const std::map<std::string, std::string> &auxiliaryEdge,
              const float &walkLengthRatio,
              const float &restartRatio,
              const unsigned int &totalStepCount,
              std::promise<std::unordered_map<std::string, unsigned int>>&& promiseObj,
              const unsigned int &threadNum = 0,
              const bool &keepVisitedCount = false) const;

    // 知识点-题目-课件定制游走方法
    void walkOnThread1(const std::string &beginNodeType,
                       const std::string &beginNodeID,
                       const float &restartRatio,
                       const unsigned int &totalStepCount,
                       std::promise<std::unordered_map<std::string, unsigned int>>&& promiseObj);

    void multiWalk(const std::vector<std::string> &beginNodeTypeList,
                   const std::vector<std::map<std::string, double>> &beginNodeIDList,
                   const std::vector<std::vector<std::string>> &stepDefineList,
                   const std::vector<std::map<std::string, std::string>> &auxiliaryEdgeList,
                   const std::vector<float> &walkLengthRatioList,
                   const std::vector<float> &restartRatioList,
                   const std::vector<unsigned int> &totalStepCountList,
                   const std::vector<bool> &isSplitStepCountList,
                   const bool &keepVisitedCount = false);

    /**
     * 图重置方法
     * 重置图中全部节点用于记录遍历/游走的状态信息
     */
    void reset(const bool &onlyVisitedCount = true);

    /**
     * 获取访问列表中全部节点按照访问次数从大到小顺序的列表
     * 返回节点ID和访问次数所组成Pair的列表
     * @return
     */
//    std::vector<std::pair<std::string, int>> getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence) const;

    /**
     * 获取访问列表中指定类型全部节点按照访问次数从大到小顺序的列表
     * 返回节点ID和访问次数所组成Pair的列表
     * @return
     */
//    std::vector<std::pair<std::string, int>> getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence, const std::string &nodeType) const;

//    std::vector<std::pair<std::string, int>> getSortedNodeTypeIDListByVisitedCount(const std::string &nodeType) const;

    std::vector<std::pair<std::string, int>> getSortedResultNodeTypeIDListByVisitedCount(const std::string &nodeType, const unsigned int &threadNum = 0) const;

    /**
     * 判断两点在图中是否相连
     * @param aNodeID 起点ID
     * @param bNodeID 终点ID
     * @param traverseSequenceList 遍历的节点ID路径
     * @return
     */
    bool isLinked(const std::string &aNodeType, const std::string &aNodeID, const std::string &bNodeType, const std::string &bNodeID, std::vector<std::string> &traverseSequenceList);

    /**
     * 将图节点列表中的部分点排除
     * @param excludeNodeTypeIDList
     */
    void excludeNodes(const std::vector<std::string> &excludeNodeTypeIDList);

    /**
     *
     * @param beginAndEndNodeTypeIDList
     */
    void excludeEdges(const std::vector<std::pair<std::string, std::string>> &beginAndEndNodeTypeIDList);

    /**
     * 将图节点列表中部分点包含
     * @param includeNodeTypeIDList
     */
    void includeNodes(const std::vector<std::string> &includeNodeTypeIDList);

    /**
     *
     * @param beginAndEndNodeTypeIDList
     */
    void includeEdges(const std::vector<std::pair<std::string, std::string>> &beginAndEndNodeTypeIDList);

    /**
     * 刷新图中节点的分类型链表的状态
     * 若不刷新则图中节点的分类型链表将无法提供随机访问能力
     */
    void flush();
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
                  const EdgeChooseStrategy &strategy,
                  const std::string &endNodeTypeID = "");

    /**
     * 节点访问次数的比较方法
     * 用于获取根据节点访问次数排序的列表
     * @param a
     * @param b
     * @return
     */
    static bool cmp(std::pair<std::string, int> a, std::pair<std::string, int> b);

    static std::string getTypeFromTypeID(const std::string tpeID);

    /**
     * 清空图操作结果列表
     * 根据图配置文件中的参数确定具体需要清空的列表
     * 该方法也受Util类声明中相关宏定义的控制
     */
    void clearResultList(const unsigned int &threadNum = 0);

    void insertResultList(Node* &node, const unsigned int &threadNum = 0);

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

    /**
     * 图中点的读书字典
     * Key为节点TypeID
     * Value为该点的度数字典
     */
    std::map<std::string, std::map<std::string, int>> nodeDegreeList;

    /**
     * 图中不同类型点的最大度数
     */
    std::map<std::string, std::map<std::string, int>> nodeTypeMaxDegreeList;

    /**
     * 图操作结果类型
     * visited_count    被访问过的节点访问次数字典
     * walking_sequence 按访问顺序存储的节点TypeID列表
     */
    std::string resultType;

    /**
     * 被访问过的节点访问次数字典
     */
    std::map<unsigned int, std::unordered_map<std::string, unsigned int>> visitedNodeTypeIDCountList;

    /**
     * 按访问顺序存储的节点TypeID列表
     */
    std::map<unsigned int, std::vector<std::string>> walkingSequence;

    /**
     * 随机数生成引擎
     */
    std::default_random_engine randomEngine;

    /**
     * 随机数分布器
     */
    std::uniform_real_distribution<double> randomDistribution;
};


#endif //GRAPHCOMPUTE_GRAPH_H