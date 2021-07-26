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

void Node::addEdge(Node *const &node) {
    // 当前框架不支持多重边
    // 判断待添加边对应的连接点是否已存在
    if (std::find(this->linkedNodeList.begin(), this->linkedNodeList.end(), node) == this->linkedNodeList.end()) {
        // 不存在则添加
        this->linkedNodeList.push_back(node);
    }
}

const std::vector<Node*> Node::getNoVisitedLinkedNodeList() const {
    std::vector<Node*> result;
    // 遍历当前点的邻接列表
    for (auto iter = this->linkedNodeList.begin(); iter != this->linkedNodeList.end(); ++iter) {
        // 判断当前邻接点是否已被访问
        if (!(*iter)->hasVisted) {
            // 未访问则加入列表
            result.push_back(*iter);
        }
    }
    return result;
}

const std::string Node::getLinkInfo() const {
    std::string info = this->type + ":" + this->id + '\n';
    for (const auto &node : this->linkedNodeList) {
        info += " -> ";
        info += node->type + ":" + node->id + "\n";
    }
    return info;
}

void Node::visit() {
    this->hasVisted = true;
}

bool Node::isVisit() {
    return this->hasVisted;
}

Node *const Node::getNextLinkedNode(const EdgeChooseStrategy &strategy) {
    // 初始化下一个邻接节点的指针为空指针
    Node* nextNode = nullptr;
    // 判断当前节点的邻接列表是否为空
    if (!this->linkedNodeList.empty()) {
        // 不为空则根据不同便边选择策略选择不同的边连接的邻接节点
        if (strategy == FIRST) {
            // 选择邻接列表中第一条边
            nextNode = this->linkedNodeList[0];
        } else if (strategy == FIRST_NO_VISIT) {
            // 选择邻接列表中第一条未访问的边
            for (auto i = 0; i < this->linkedNodeList.size(); ++i) {
                if (!this->linkedNodeList[i]->hasVisted) {
                    nextNode = this->linkedNodeList[i];
                }
            }
        } else if (strategy == LAST) {
            // 选择邻接列表中的最后一条边
            nextNode = this->linkedNodeList[this->linkedNodeList.size() - 1];
        } else if (strategy == LAST_NO_VISIT) {
            // 选择邻接列表中最后一条未访问过的边
            for (auto i = this->linkedNodeList.size() - 1; i >= 0; --i) {
                if (!this->linkedNodeList[i]->hasVisted) {
                    nextNode = this->linkedNodeList[i];
                }
            }
        } else if (strategy == RANDOM) {
            // 随机选择邻接列表中的一条边
            std::uniform_int_distribution<unsigned> randomDistribution(0, this->linkedNodeList.size() - 1);
            nextNode = this->linkedNodeList[randomDistribution(this->randomEngine)];
        } else if (strategy == RANDOM_NO_VISIT) {
            // 随机选择邻接列表中未访问的一条边
            auto noVisitedLinkedNodeList = this->getNoVisitedLinkedNodeList();
            std::uniform_int_distribution<unsigned> randomDistribution(0, noVisitedLinkedNodeList.size() - 1);
            nextNode = noVisitedLinkedNodeList[randomDistribution(this->randomEngine)];
        } else {

        }
    }
    return nextNode;
}

void Node::reset() {
    // 已访问状态重置为未访问
    this->hasVisted = false;
}