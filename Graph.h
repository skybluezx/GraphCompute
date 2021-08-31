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
    Graph() = delete;

    /**
     * 构造方法
     * 读取图定义文件所在的文件建立图
     * @param graphDefineFileDirectoryPath  图定义目录
     * @param resultType                    图计算结果类型（visited_count：节点去重后的访问次数字典 walking_sequence：按访问顺序排序的节点type-id列表）
     * @param readEdgeCount                 读取的最大边数（小等于0表示读取目录中的全部边，大于0表示读取对应个数的边）
     * @param maxWalkBeginNodeCount         并行游走支持的最大起点个数（默认为0则表示不开启并行游走）
     */
    explicit Graph(const std::string &graphDefineFileDirectoryPath, const std::string &resultType="walking_sequence", const int &readEdgeCount = -1, const int &maxWalkBeginNodeCount=1);

    /**
     * 析构方法
     * 负责将图中存储在堆内存中的点释放
     */
    ~Graph();

    /**
     * 获取图的节点列表
     * Map形式
     * Key节点Type-ID
     * Value为指向节点对象的指针常量
     * @return
     */
    const std::map<const std::string, Node *const> &getNodeList() const;

    /**
     * 获取当前图支持的最大并行开始点游走个数
     * @return
     */
    const int &getMaxWalkBeginNodeCount() const;

    /**
     * 获取图的分类型节点列表
     * Map形式
     * Key为类型
     * Value为该类型的全部节点指针组成的数组
     * @return
     */
    const std::map<const std::string, std::vector<Node *>> &getTypeNodeList() const;

    /**
     * 获得指定ID的节点访问次数
     * @param id    节点ID
     * @param type  节点类型
     * @return      -1则表示无此节点，大等于0则为该点的访问次数
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
     * [依赖flush方法生成]
     * @return
     */
    const std::map<std::string, std::map<std::string, int>> &getNodeDegreeList() const;

    /**
     * 获取当前图中全部节点类型对应的分类型最大度数列表
     * [依赖flush方法生成]
     * @return
     */
    const std::map<std::string, std::map<std::string, int>> &getNodeTypeMaxDegreeList() const;

    /**
     * 获取图中节点的访问次数列表
     * 数组中存储多个次数列表用于实现多线程并行
     * 每个次数列表为一个HashMap
     * Key为节点的Type-ID
     * Value为该节点对应的访问次数
     * @return
     */
    const std::vector<std::unordered_map<std::string, unsigned int>> &getVisitedNodeTypeIDCountList() const;

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
      * @param beginNodeType    开始点类型
      * @param beginNodeID      开始点ID
      * @param stepDefine       步长定义
      * @param auxiliaryEdge    辅助边
      * @param walkLengthRatio  单次游走步长
      * @param restartRatio     重启概率
      * @param totalStepCount   游走总步长
      * @param keepVisitedCount 是否保留节点访问次数
     * @return
     */
    void walk(const std::string &beginNodeType,
              const std::string &beginNodeID,
              const std::vector<std::string> &stepDefine,
              const std::map<std::string, std::string> &auxiliaryEdge,
              const float &walkLengthRatio,
              const float &restartRatio,
              const unsigned int &totalStepCount,
              const bool &keepVisitedCount = false);
    // 游走方法的多态
    // 传入开始点对象
    void walk(const Node &beginNode,
              const std::vector<std::string> &stepDefine,
              const std::map<std::string, std::string> &auxiliaryEdge,
              const float &walkLengthRatio,
              const float &restartRatio,
              const unsigned int &totalStepCount,
              const bool &keepVisitedCount = false);

    /**
     * 多重游走方法的线程体（通用版本）
     * @param beginNodeType         开始点类型
     * @param beginNodeID           开始点ID
     * @param stepDefine            步长定义
     * @param auxiliaryEdge         辅助边
     * @param walkLengthRatio       单次游走步长
     * @param restartRatio          重启概率
     * @param totalStepCount        游走总步长
     * @param nodeVisitedCountList  节点访问次数数组
     * @param randomEngine          随机引擎
     * @param keepVisitedCount      是否保存本次游走的访问次数
     */
    void walkOnThread(const std::string &beginNodeType,
                      const std::string &beginNodeID,
                      const std::vector<std::string> &stepDefine,
                      const std::map<std::string, std::string> &auxiliaryEdge,
                      const float &walkLengthRatio,
                      const float &restartRatio,
                      const unsigned int &totalStepCount,
                      std::unordered_map<std::string, unsigned int> &nodeVisitedCountList,
                      std::mt19937 &randomEngine,
                      const bool &keepVisitedCount = false);

    /**
     * 多重游走线程体（基于"知识点-题目-课件"步长定义的硬编码版本）
     * @param beginNodeType     开始点类型
     * @param beginNodeID       开始点ID
     * @param restartRatio      重启概率
     * @param totalStepCount    总步长
     * @param promiseObj        返回游走结果的promise对象
     */
    void walkOnThread1(const std::string &beginNodeType,
                       const std::string &beginNodeID,
                       const float &restartRatio,
                       const unsigned int &totalStepCount,
                       std::unordered_map<std::string, unsigned int> &nodeVisitedCountList);

    /**
     * 多重游走
     * @param beginNodeTypeList
     * @param beginNodeIDList
     * @param stepDefineList
     * @param auxiliaryEdgeList
     * @param walkLengthRatioList
     * @param restartRatioList
     * @param totalStepCountList
     * @param isSplitStepCountList
     * @param keepVisitedCount
     */
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

    /**
     * 获取线程编号对应的指定类型的节点访问次数列表
     * @param nodeType  节点类型
     * @param threadNum 线程编号(默认为第一个线程，单线程游走时即默认使用第一个线程)
     * @return
     */
    std::vector<std::pair<std::string, int>> getSortedResultNodeTypeIDListByVisitedCount(const std::string &nodeType, const unsigned int &threadNum=0) const;

    /**
     * 获取多个线程编号对应的指定类型的节点访问次数列表
     * @param nodeType      节点类型
     * @param threadNumList 线程编号数组
     * @return
     */
    std::vector<std::pair<std::string, int>> getSortedResultNodeIDListByVisitedCount(const std::string &nodeType, const std::vector<unsigned int> &threadNumList) const;

    /**
     * 获取多个线程编号对应的指定类型的多个节点访问次数列表
     * @param nodeType
     * @param threadNumList
     * @return
     */
    std::vector<std::vector<std::pair<std::string, int>>> getSortedResultNodeTypeIDListsByVisitedCount(const std::string &nodeType, const std::vector<unsigned int> &threadNumList) const;

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

    /**
     * 清空图操作结果列表
     * 根据图配置文件中的参数确定具体需要清空的列表
     * 该方法也受Util类声明中相关宏定义的控制
     * @param threadNum 线程编号（表示要清空的是哪个线程对应的访问结果）
     */
    void clearResultList(const unsigned int &threadNum = 0);

    /**
     * 将指点节点插入访问结果中
     * 根据图配置文件中的参数确定具体需要插入的列表类型（访问次数列表还是节点ID的访问序列）
     * @param node      需要放入访问结果的节点
     * @param threadNum 线程编号（表示要放入的是哪个线程对应的访问结果）
     */
    void insertResultList(Node* &node, const unsigned int &threadNum = 0);

    /**
     * 多路图操作结果合并
     * Todo
     * 合并策略目前仅支持访问次数最大合并，其他策略待实现
     * @param threadNumList 需要合并多路图操作线程编号列表
     */
    void mergeResultList(const std::vector<unsigned int> &threadNumList, const unsigned int &targetThreadNum);
private:
    /**
     * 成员属性
     */

    /**
     * 图中节点的TypeID字典
     * key为点的Type-ID
     * value为对应点的指针
     */
    std::map<const std::string, Node *const> nodeList;

    /**
     * 图中节点的类型字典
     * key为节点的type
     * value为该type对应的全部节点指针
     */
    std::map<const std::string, std::vector<Node *>> typeNodeList;

    /**
     * 图中点的类型字典
     * Key为类型
     * Value为该类型对应的点个数
     * Todo
     * unsigned是否合适？
     */
    std::map<std::string, unsigned int> nodeTypeCountList;

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
     * 最大支持的同时游走的起点数量
     * 该参数用于多线程游走时的并发数量控制
     * 同时该参数也参与了多线程游走的性能优化（提前将每个线程的计数字典生成）
     */
    int maxWalkBeginNodeCount;

    /**
     * 被访问过的节点访问次数字典
     * 按照最大可支持的并行起点个数初始化字典个数
     */
    std::vector<std::unordered_map<std::string, unsigned int>> visitedNodeTypeIDCountList;

    /**
     * 随机引擎列表
     * 按照最大可支持的并行起点个数初始化随机引擎的个数
     */
    std::vector<std::mt19937> randomEngineList;

    /**
     * 按访问顺序存储的节点TypeID列表
     */
    std::vector<std::vector<std::string>> walkingSequence;

    /**
     * 私有成员方法
     */

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

    /**
     * 从TypeID中获取Type
     * @param tpeID
     * @return
     */
    static std::string getTypeFromTypeID(const std::string tpeID);
};


#endif //GRAPHCOMPUTE_GRAPH_H
