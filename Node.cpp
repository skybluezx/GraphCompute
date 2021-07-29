//
// Created by 翟霄 on 2021/7/16.
//

#include "Node.h"

#include <chrono>

#include <iostream>

Node::Node() = default;

Node::Node(const std::string &id, const std::string &type)
        : id(id), type(type){
    this->randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

Node::~Node() {}

const std::string &Node::getID() const {
    return this->id;
}

const std::string &Node::getType() const {
    return this->type;
}

const std::string Node::getIDType() const {
    return this->type + ":" + this->id;
}

const std::vector<Node *> &Node::getLinkedNodeList() const {
    return this->linkedNodeList;
}

const std::map<std::string, std::vector<Node *>> &Node::getLinkedNodeMapList() const {
    return this->linkedNodeMapList;
}

void Node::addEdge(Node *const &node) {
    // 当前框架不支持多重边
    // 判断待添加边对应的连接点是否已存在
    if (node != nullptr) {
        this->linkedNodeList.push_back(node);

        if (!this->linkedNodeMapList.contains(node->type)) {
            this->linkedNodeMapList[node->type] = std::vector<Node*>();
        }
        std::vector<Node*> &nodeList = this->linkedNodeMapList[node->type];
        if (std::find(nodeList.begin(), nodeList.end(), node) == nodeList.end()) {
            // 不存在则添加
            nodeList.push_back(node);
        }
    }
}

const std::vector<Node*> Node::getNoVisitedLinkedNodeList() const {
    std::vector<Node*> result;
    // 遍历当前点的邻接列表
    for (auto nodeIter = this->linkedNodeList.begin(); nodeIter != this->linkedNodeList.end(); ++nodeIter) {
        // 判断当前邻接点是否已被访问
        if (!(*nodeIter)->isVisited()) {
            // 未访问则加入列表
            result.push_back(*nodeIter);
        }
    }

    return result;
}

const std::string Node::getLinkedInfo() const {
    std::string info = this->type + ":" + this->id + '\n';
    for (auto nodeIter = this->linkedNodeList.begin(); nodeIter != this->linkedNodeList.end(); ++nodeIter) {
        info += " -> ";
        info += (*nodeIter)->type + ":" + (*nodeIter)->id + "\n";
    }

    return info;
}

void Node::visit() {
    this->vistedCount++;
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

Node *const Node::getNextLinkedNode(const EdgeChooseStrategy &strategy, const std::string &type) {
    // 初始化下一个邻接节点的指针为空指针
    Node* nextNode = nullptr;

    // 判断当前节点的邻接列表是否为空
    if (!this->linkedNodeList.empty()) {
        if (type.empty()) {
            nextNode = this->getNextLinkedNode(this->linkedNodeList, strategy);
        } else {
            if (this->linkedNodeMapList.contains(type)) {
                nextNode = this->getNextLinkedNode(this->linkedNodeMapList.at(type), strategy);
            }
        }
    }

    return nextNode;
}

Node *const Node::getNextLinkedNode(const EdgeChooseStrategy &strategy, const std::vector<std::string> &typeList) {
    Node* nextNode = nullptr;

    // 判断当前节点的邻接列表是否为空
    if (!this->linkedNodeList.empty()) {
        if (typeList.empty()) {
            nextNode = this->getNextLinkedNode(this->linkedNodeList, strategy);
        } else {
            for (auto iter = typeList.cbegin(); iter != typeList.cend(); ++iter) {
                std::cout << *iter << std::endl;
            }
        }
    }

    return nextNode;
}

void Node::reset() {
    // 已访问状态重置为未访问
    this->vistedCount = 0;
}

Node *const Node::getNextLinkedNode(const std::vector<Node *> &nodeList, const EdgeChooseStrategy &strategy) {
    Node* nextNode = nullptr;

    if (strategy == FIRST) {
        // 选择邻接列表中第一条边
        nextNode = nodeList[0];
    } else if (strategy == FIRST_NO_VISIT) {
        // 选择邻接列表中第一条未访问的边
        for (auto i = 0; i < nodeList.size(); ++i) {
            if (!nodeList[i]->isVisited()) {
                nextNode = nodeList[i];
            }
        }
    } else if (strategy == LAST) {
        // 选择邻接列表中的最后一条边
        nextNode = nodeList[nodeList.size() - 1];
    } else if (strategy == LAST_NO_VISIT) {
        // 选择邻接列表中最后一条未访问过的边
        for (auto i = nodeList.size() - 1; i >= 0; --i) {
            if (!nodeList[i]->isVisited()) {
                nextNode = nodeList[i];
            }
        }
    } else if (strategy == RANDOM) {
        // 随机选择邻接列表中的一条边
        std::uniform_int_distribution<unsigned> randomDistribution(0, nodeList.size() - 1);
        nextNode = nodeList[randomDistribution(this->randomEngine)];
    } else if (strategy == RANDOM_NO_VISIT) {
        // 随机选择邻接列表中未访问的一条边
        auto noVisitedLinkedNodeList = this->getNoVisitedLinkedNodeList();
        std::uniform_int_distribution<unsigned> randomDistribution(0, noVisitedLinkedNodeList.size() - 1);
        nextNode = noVisitedLinkedNodeList[randomDistribution(this->randomEngine)];
    } else {

    }

    return nextNode;
}