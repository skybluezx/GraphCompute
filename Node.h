//
// Created by 翟霄 on 2021/7/16.
//

#ifndef GRAPHCOMPUTE_NODE_H
#define GRAPHCOMPUTE_NODE_H

#include <vector>
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
     * 增加边
     * @param nodeID 边对应连接点的ID
     * @param node 边对应连接点的对象的常量指针
     */
    void addEdge(Node *const &node);

    /**
     * 获得当前点尚未访问的连接点列表
     * @return 当前点尚未方位的连接点常量列表
     * 返回值为常量列表则不允许通过其他方式修改列表中的元素，否则列表中原指针指向的内存将存在泄漏风险
     */
    const std::vector<Node*> getNoVisitedLinkedNodeList() const;

    /**
     * 获取当前点全部边的连接信息
     * @return
     */
    const std::string getLinkInfo() const;

    /**
     * 访问点
     */
    void visit();

    bool isVisit();

    /**
     * 获得当前点的下一个连接点
     * @param strategy 边选择策略
     * @return 当前点下一个连接点的指针常量（即指针不能重指向其他点）
     * 返回值为指针常量，说明指针指向内存无需调用者负责，同时也不允许通过其他方式修改指针指向，否则原指针指向内容将可能存在泄漏风险
     */
    Node *const getNextLinkedNode(const EdgeChooseStrategy &strategy);

    /**
     * 重置图遍历/游走的状态信息
     */
    void reset();
public:
    // 点ID
    std::string id;
    // 点类型
    std::string type;

private:
    // 与当前点有边连接的链表
    // key为连接点的id
    // value为连接点对象的常量指针
    // 由于该指针对应的内容均为Graph对象创建，由Graph对象负责维护
    // 所以在Node对象中不应该将其中的指针重指向其他点，否则将出现野指针
    std::vector<Node *> linkedNodeList;

    /**
     * 图遍历或游走的状态信息
     */
     bool hasVisted = false;

     std::default_random_engine randomEngine;
};


#endif //GRAPHCOMPUTE_NODE_H