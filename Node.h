//
// Created by 翟霄 on 2021/7/16.
//

#ifndef GRAPHCOMPUTE_NODE_H
#define GRAPHCOMPUTE_NODE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <random>

#include "type_defination.h"

class Node {
public:
    /**
     * 默认构造方法
     */
    Node();

    /**
     * 带参构造方法
     */
    Node(const std::string &id, const std::string &type);

    /**
     * 析构方法
     */
    ~Node();

    /**
     * 获取点的ID
     * @return 返回引用
     */
    const std::string &getID() const;

    /**
     * 获取点的类型
     * @return 返回引用
     */
    const std::string &getType() const;

    const std::string getTypeID() const;

    /**
     * 返回该点邻接点列表
     * @return 点对象中邻接点列表的引用
     */
    const std::vector<std::pair<Node*, bool>> &getLinkedNodeList() const;

    /**
     * 返回该点的分类型邻接表
     * @return
     */
    const std::unordered_map<std::string, std::vector<Node *>> &getLinkedNodeMapList() const;

    /**
     * 返回该点指定若干类型的合并邻接表
     * @param typeList
     * @return
     */
    const std::vector<Node*> getLinkedNodeMapList(const std::vector<std::string> &typeList) const;

    /**
     * 返回指定类型的第一个邻接点
     * @param type
     * @return
     */
    Node* getFirstLinkedNode(const std::string &type) const;

    /**
     * 增加边
     * @param nodeID 边对应连接点的ID
     * @param node 边对应连接点的对象的常量指针
     */
    void addEdge(Node *const &node);

    /**
     * 获得当前点尚未访问的连接点列表
     * @return 当前点尚未访问的连接点常量指针列表
     * 返回值为常量指针列表则不允许通过其他方式修改列表中的元素，否则列表中原指针指向的内存将存在泄漏风险
     */
    const std::vector<Node*> getNoVisitedLinkedNodeList() const;

    /**
     * 获奖点的基本信息
     * @return
     */
    const std::string to_string() const;

    /**
     * 获取当前点全部边的连接信息
     * @return
     */
    const std::string getLinkedInfo() const;

    /**
     * 判断当前节点是否可访问
     * @return
     */
    bool canVisit() const;

    /**
     * 访问点
     * 访问次数加1
     */
    void visit(const unsigned int &visitCount=1);

    /**
     * 判断当前节点是否访问过
     * 若访问次数大于1则返回true否则返回false
     * @return
     */
    bool isVisited() const;

    /**
     * 获取当前节点的访问次数
     * @return
     */
    int getVisitedCount() const;

    /**
     * 获得当前点指定类型的邻接点
     * @param strategy  边选择策略
     * @param nextNode  邻接点指针
     * @param type      邻接点类型
     * @return
     * 返回值为指针常量，说明指针指向内存无需调用者负责，同时也不允许通过其他方式修改指针指向，否则原指针指向内容将可能存在泄漏风险
     */
    bool getNextLinkedNode(const EdgeChooseStrategy &strategy, Node *&nextNode, const std::string &type);
    /**
     * 获得当前点指定多个类型的邻接点
     * @param strategy  边选择策略
     * @param nextNode  邻接点指针
     * @param typeList  邻接点类型数组
     * @return
     */
    bool getNextLinkedNode(const EdgeChooseStrategy &strategy, Node *&nextNode, const std::vector<std::string> &typeList);
    /**
     * 随机获取指定类型的邻接点
     * @param nextNode  邻接点指针
     * @param type      邻接点类型
     * @return
     */
    bool getNextRandomLinkedNode(Node *&nextNode, const std::string &type, std::mt19937 &randomEngine);
    /**
     * 随机获取多个类型的邻接点
     * @param nextNode  邻接点指针
     * @param typeList  邻接点类型数组
     * @return
     */
    bool getNextRandomLinkedNode(Node *&nextNode, const std::vector<std::string> &typeList, std::mt19937 &randomEngine);

    /**
     * 重置图遍历/游走的状态信息
     */
    void reset(const bool &onlyVisitedCount = true);

    /**
     * 排除点
     * 将当前节点的可访问状态置为false
     */
    void exclude();

    bool excludeEdge(std::string endNodeTypeID);

    /**
     * 包含点
     * 将当前点的可访问状态置为true
     */
    void include();

    bool includeEdge(std::string endNodeTypeID);

    /**
     * 根据原始链表刷新分类型链表
     * 判断原始链表中全部节点是否可访问
     * 只将可访问节点加入分类型节点
     */
    void flushLinkedNodes();

private:
    /**
     * 私有成员
     */

    // 点ID
    std::string id;
    // 点类型
    std::string type;
    // 当前点是否可访问
    bool canVisitFlag = true;
    // 与当前点有边连接的链表
    // 由于该指针对应的内容均为Graph对象创建，由Graph对象负责维护
    // 所以在Node对象中不应该将其中的指针重指向其他点，否则将出现野指针
    std::vector<std::pair<Node*, bool>> linkedNodeList;
    // 与当前点有边连接的类型链表
    // key为连接点的type
    // value为对应type对应的连接点对象的指针列表
    // 由于该指针对应的内容均为Graph对象创建，由Graph对象负责维护
    // 所以在Node对象中不应该将其中的指针重指向其他点，否则将出现野指针
    std::unordered_map<std::string, std::vector<Node *>> linkedNodeMapList;

    /**
     * 图遍历或游走的状态信息
     */
    unsigned int vistedCount;

    /**
     * 私有方法
     */

    /**
     * 在给定的节点列表里根据指定策略选择边
     * @return
     */
    static bool getNextLinkedNode(const std::vector<Node *> &nodeList,
                                  Node *&nextNode,
                                  const EdgeChooseStrategy &strategy,
                                  std::mt19937 &randomEngine,
                                  std::uniform_int_distribution<unsigned> &randomDistribution);
    /**
     * 具体边选择策略的选边方法
     * @param nodeList
     * @param nextNode
     * @return
     */
    static bool getNextFirstLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode);
    static bool getNextFirstNoVisitedLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode);
    static bool getNextLastLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode);
    static bool getNextLastNoVisitedLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode);
    static bool getNextRandomLinkedNode(const std::vector<Node *> &nodeList,
                                        Node* &nextNode,
                                        std::mt19937 &randomEngine,
                                        std::uniform_int_distribution<unsigned> &randomDistribution);
    static bool getNextRandomNoVisitedLinkedNode(const std::vector<Node *> &nodeList,
                                                 Node* &nextNode,
                                                 std::mt19937 &randomEngine);
};


#endif //GRAPHCOMPUTE_NODE_H