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
     * @return 遍历顺序对应ID列表
     */
    std::vector<std::string> traverse(const Node &node, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const;

    /**
     * 游走方法
     * 游走方法以获得关于图中点的某种模式为目标
     */
    void walk();

    /**
     * 图重置方法
     * 重置图中全部节点用于记录遍历/游走的状态信息
     */
    void reset();

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
     * 图中全部点的字典
     * key为点的ID
     * value为对应点的指针
     */
    std::map<const std::string, Node *const> nodeList;

    /**
     * 图计算部分
     */
};


#endif //GRAPHCOMPUTE_GRAPH_H