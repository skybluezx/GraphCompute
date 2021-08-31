//
// Created by 翟霄 on 2021/7/16.
//

#include "Node.h"

#include <chrono>
#include <iostream>

Node::Node() = default;

Node::Node(const std::string &id, const std::string &type)
        : id(id), type(type){
    this->vistedCount = 0;
    this->canVisitFlag = true;
}

Node::~Node() {}

const std::string &Node::getID() const {
    return this->id;
}

const std::string &Node::getType() const {
    return this->type;
}

const std::string Node::getTypeID() const {
    return this->type + ":" + this->id;
}

const std::vector<std::pair<Node *, bool>> &Node::getLinkedNodeList() const {
    return this->linkedNodeList;
}

const std::unordered_map<std::string, std::vector<Node *>> &Node::getLinkedNodeMapList() const {
    return this->linkedNodeMapList;
}

const std::vector<Node*> Node::getLinkedNodeMapList(const std::vector<std::string> &typeList) const {
    std::vector<Node*> nodeList;

    for (auto i = 0; i < typeList.size(); ++i) {
        if (this->linkedNodeMapList.contains(typeList[i])) {
            nodeList.insert(nodeList.end(), this->linkedNodeMapList.at(typeList[i]).begin(), this->linkedNodeMapList.at(typeList[i]).end());
        }
    }

    return nodeList;
}

void Node::addEdge(Node *const &node) {
    // 当前框架不支持多重边
    // 判断待添加边对应的连接点是否已存在
    if (node != nullptr) {
        this->linkedNodeList.push_back(std::pair(node, true));
    }
}

const std::vector<Node*> Node::getNoVisitedLinkedNodeList() const {
    std::vector<Node*> result;
    // 遍历当前点的邻接列表
    for (auto nodeIter = this->linkedNodeList.begin(); nodeIter != this->linkedNodeList.end(); ++nodeIter) {
        // 判断当前邻接点是否已被访问
        if (!nodeIter->first->isVisited()) {
            // 未访问则加入列表
            result.push_back(nodeIter->first);
        }
    }

    return result;
}

const std::string Node::to_string() const {
    std::string info = "[访问次数：" + std::to_string(this->getVisitedCount()) + "] " + this->type + ":" + this->id + '\n';

    if (this->canVisit()) {
        info = "[可访问]" + info;
    } else {
        info = "[不可访问]" + info;
    }

    return info;
}

const std::string Node::getLinkedInfo() const {
    std::string info = this->to_string();

    for (auto nodeIter = this->linkedNodeList.begin(); nodeIter != this->linkedNodeList.end(); ++nodeIter) {
        if (nodeIter->second) {
            info += " -> ";
        } else {
            info += " *> ";
        }
        info += nodeIter->first->to_string();
    }

    return info;
}

bool Node::canVisit() const {
    return this->canVisitFlag;
}

void Node::visit(const unsigned int &visitCount) {
    this->vistedCount += visitCount;
}

bool Node::isVisited() const {
    if (this->vistedCount > 0) {
        return true;
    } else {
        return false;
    }
}

int Node::getVisitedCount() const {
    return this->vistedCount;
}

bool Node::getNextLinkedNode(const EdgeChooseStrategy &strategy, Node *&nextNode, const std::string &type) {
//    // 链表选择为O(1)
//    // 边选择策略为O(n)，其中n为4（因为目前有四种边选择策略）
//    nextNode = nullptr;
//    if (this->linkedNodeMapList.contains(type)) {
//        return Node::getNextLinkedNode(this->linkedNodeMapList.at(type).first, nextNode, strategy, this->randomEngine,
//                                       this->linkedNodeMapList.at(type).second);
//    } else {
//        return false;
//    }
    return false;
}

bool Node::getNextLinkedNode(const EdgeChooseStrategy &strategy, Node *&nextNode, const std::vector<std::string> &typeList) {
//    // 链表合并选择为O(n)，n为设置类型个数，设计在内存中创建合并后链表，也会带来耗时
//    // 边选择策略为O(m)，其中m为4（因为目前有四种边选择策略）
//    nextNode = nullptr;
//    std::vector<Node*> nodeList = std::move(this->getLinkedNodeMapList(typeList));
//    std::uniform_int_distribution<unsigned> randomDistribution(0, nodeList.size() - 1);
//    return Node::getNextLinkedNode(nodeList, nextNode, strategy, this->randomEngine, randomDistribution);
    return false;
}

bool Node::getNextRandomLinkedNode(Node *&nextNode, const std::string &type, std::mt19937 &randomEngine) {
    nextNode = nullptr;
    if (this->linkedNodeMapList.contains(type)) {
        std::uniform_int_distribution<unsigned> randomDistribution(0, this->linkedNodeMapList.at(type).size() - 1);
        return Node::getNextRandomLinkedNode(this->linkedNodeMapList.at(type), nextNode, randomEngine, randomDistribution);
    } else {
        return false;
    }
}

bool Node::getNextRandomLinkedNode(Node *&nextNode, const std::vector<std::string> &typeList, std::mt19937 &randomEngine) {
    nextNode = nullptr;
    std::vector<Node*> nodeList = std::move(this->getLinkedNodeMapList(typeList));
    std::uniform_int_distribution<unsigned> randomDistribution(0, nodeList.size() - 1);
    return Node::getNextRandomLinkedNode(nodeList, nextNode, randomEngine, randomDistribution);
}

void Node::reset(const bool &onlyVisitedCount) {
    if (onlyVisitedCount) {
        // 已访问状态重置为未访问
        this->vistedCount = 0;
    } else {
        this->vistedCount = 0;
        this->canVisitFlag = true;
        for (auto iter = this->linkedNodeList.begin(); iter != this->linkedNodeList.end(); ++iter) {
            iter->second = true;
        }
    }
}

void Node::exclude() {
    // 只将是否可访问状态置为false
    // 边不作处理保持当前状态，即如果之前被删过边，则当前存在部分边的状态为false，否则所有边均为true
    // 对已删除点的边进行访问状态未知
    this->canVisitFlag = false;
}

bool Node::excludeEdge(std::string endNodeTypeID) {
    for (auto iter = this->linkedNodeList.begin(); iter != this->linkedNodeList.end(); ++iter) {
        if (iter->first->getTypeID() == endNodeTypeID) {
            if (iter->second) {
                iter->second = false;
                return true;
            }
        }
    }
    return false;
}

void Node::include() {
    this->canVisitFlag = true;
}

bool Node::includeEdge(std::string endNodeTypeID) {
    for (auto iter = this->linkedNodeList.begin(); iter != this->linkedNodeList.end(); ++iter) {
        if (iter->first->getTypeID() == endNodeTypeID) {
            if (!iter->second) {
                iter->second = true;
                return true;
            }
        }
    }
    return false;
}

void Node::flushLinkedNodes() {
    // 清空分类型链表
    this->linkedNodeMapList.clear();
    // 遍历链表
    for (auto i = 0; i < this->linkedNodeList.size(); ++i) {
        if (!this->linkedNodeList[i].second) {
            continue;
        }
        // 判断当前连接节点是否可访问
        if (this->linkedNodeList[i].first->canVisit()) {
            // 当前节点可访问则将其加入分类型链表

            // 判断分类型链表中当前节点对应类型的链表是否存在
            if (!this->linkedNodeMapList.contains(this->linkedNodeList[i].first->getType())) {
                // 不存在则创建空链表
                this->linkedNodeMapList[this->linkedNodeList[i].first->getType()] = std::vector<Node *>();
            }
            // 将当前节点加入分类型链表
            this->linkedNodeMapList[this->linkedNodeList[i].first->getType()].push_back(this->linkedNodeList[i].first);
        }
    }
}

bool Node::getNextLinkedNode(const std::vector<Node *> &nodeList,
                             Node *&nextNode,
                             const EdgeChooseStrategy &strategy,
                             std::mt19937 &randomEngine,
                             std::uniform_int_distribution<unsigned> &randomDistribution) {
    // 根据不同边选择策略执行不同方法
    if (strategy == FIRST) {
        return Node::getNextFirstLinkedNode(nodeList, nextNode);
    } else if (strategy == FIRST_NO_VISIT) {
        return Node::getNextFirstNoVisitedLinkedNode(nodeList, nextNode);
    } else if (strategy == LAST) {
        return Node::getNextLastLinkedNode(nodeList, nextNode);
    } else if (strategy == LAST_NO_VISIT) {
        return Node::getNextLastNoVisitedLinkedNode(nodeList, nextNode);
    } else if (strategy == RANDOM) {
        return Node::getNextRandomLinkedNode(nodeList, nextNode, randomEngine, randomDistribution);
    } else if (strategy == RANDOM_NO_VISIT) {
        return Node::getNextRandomNoVisitedLinkedNode(nodeList, nextNode, randomEngine);
    } else {
        // Todo
        // 其他边选择策略的实现
        return false;
    }
}

bool Node::getNextFirstLinkedNode(const std::vector<Node *> &nodeList, Node *&nextNode) {
    if (nodeList.size() == 0) {
        return false;
    } else {
        // 选择邻接列表中第一条边
        nextNode = nodeList[0];
        return true;
    }
}

bool Node::getNextFirstNoVisitedLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode) {
    // 选择邻接列表中第一条未访问过的边
    for (auto i = 0; i < nodeList.size(); ++i) {
        if (!nodeList[i]->isVisited()) {
            nextNode = nodeList[i];
            return true;
        }
    }
    return false;
}

bool Node::getNextLastLinkedNode(const std::vector<Node *> &nodeList, Node* &nextNode) {
    if (nodeList.empty()) {
        return false;
    } else {
        nextNode = nodeList[nodeList.size() - 1];
        return true;
    }
}

bool Node::getNextLastNoVisitedLinkedNode(const std::vector<Node *> &nodeList, Node *&nextNode) {
    // 选择邻接列表中最后一条未访问过的边
    for (auto i = nodeList.size() - 1; i >= 0; --i) {
        if (!nodeList[i]->isVisited()) {
            nextNode = nodeList[i];
            return true;
        }
    }
    return false;
}

bool Node::getNextRandomLinkedNode(const std::vector<Node *> &nodeList,
                                   Node* &nextNode,
                                   std::mt19937 &randomEngine,
                                   std::uniform_int_distribution<unsigned> &randomDistribution) {
    // 判断当前节点链表是否为空
    if (!nodeList.empty()) {
        // 否则随机选择邻接列表中的一条边
        nextNode = nodeList[randomDistribution(randomEngine)];
        return true;
    }
    return false;
}

bool Node::getNextRandomNoVisitedLinkedNode(const std::vector<Node *> &nodeList,
                                            Node *&nextNode,
                                            std::mt19937 &randomEngine) {
    // 初始化当前点全部未访问节点列表
    std::vector<Node *> noVisitedLinkedNodeList;
    // 遍历当前点的邻接表
    for (auto i = 0; i < nodeList.size(); ++i) {
        // 判断是否已访问
        if (!nodeList[i]->isVisited()) {
            // 未访问则将其加入未访问列表
            noVisitedLinkedNodeList.push_back(nodeList[i]);
        }
    }
    // 当存在未访问节点是随机访问其中一个节点
    if (!noVisitedLinkedNodeList.empty()) {
        std::uniform_int_distribution<unsigned> randomDistribution(0, noVisitedLinkedNodeList.size() - 1);
        nextNode = nodeList[randomDistribution(randomEngine)];
        return true;
    }

    return false;
}