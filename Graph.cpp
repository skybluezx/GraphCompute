//
// Created by 翟霄 on 2021/7/16.
//

#include "Graph.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <stack>
#include <queue>
#include <random>
#include <chrono>

#include "Util.h"
#include "Node.h"

Graph::Graph() = default;

Graph::Graph(const std::string &graphDefineFileDirectoryPath) {
    std::filesystem::path path(graphDefineFileDirectoryPath);
    // 判断图定义文件的路径是否存在
    if (!std::filesystem::exists(path)) {
        // 图定义文件的目录不存在
        LOG(ERROR) << "图定义目录不存在！";
    } else {
        // 路径存在
        auto begin = std::filesystem::recursive_directory_iterator(path);
        auto end = std::filesystem::recursive_directory_iterator();

        // 遍历图定义文件所在目录的全部图定义文件
        for (auto it = begin; it != end; ++it) {
            // 判断当前是否为图定义文件
            auto &entry = *it;
            if (std::filesystem::is_regular_file(entry)) {
                std::string fileName = entry.path();
                LOG(INFO) << "读取文件：" << fileName;

                std::ifstream graphDefineFile(fileName, std::ios::in);
                // 判断图定义文件打开是否成功
                if (!graphDefineFile.is_open()) {
                    // 打开失败则输出错误日志
                    LOG(ERROR) << "文件读取失败！";
                } else {
                    // 遍历图定义文件的全部行
                    // 每一行为一个边描述
                    std::string line;
                    while (std::getline(graphDefineFile, line)) {
                        // 切分图定义文件的行
                        std::vector<std::string> lineItemList = Util::stringSplitToVector(line, "\t");
                        // 判断行格式是否准确
                        if (lineItemList.size() != 2) {
                            // 输出错误日志
                            LOG(ERROR) << "节点/边定义格式错误！";
                        } else {
                            // 获得边对应起点和终点的定义对
                            std::vector<std::string> beginNodePair = Util::stringSplitToVector(lineItemList[0], ":");
                            std::vector<std::string> endNodePair = Util::stringSplitToVector(lineItemList[1], ":");

                            // 初始化起点
                            Node *beginNode = nullptr;
                            // 判断起点ID是否在全局点字典中已存在
                            if (!this->nodeList.contains(beginNodePair[1])) {
                                // 不存在则创建起点对应的点对象
                                beginNode = new Node(beginNodePair[1], beginNodePair[0]);
                                // 将创建的点增加至全局点字典
                                this->nodeList.insert(
                                        std::map<const std::string, Node *const>::value_type(beginNode->id, beginNode));
                            } else {
                                // 存在则获取已存在的点
                                beginNode = this->nodeList[beginNodePair[1]];
                            }

                            // 初始化终点
                            Node *endNode = nullptr;
                            // 判断终点ID是否在全局点字典中已存在
                            if (!this->nodeList.contains(endNodePair[1])) {
                                // 不存在则创建终点对应的点对象
                                endNode = new Node(endNodePair[1], endNodePair[0]);
                                // 将创建的点增加至全局点字典
                                this->nodeList.insert(
                                        std::map<const std::string, Node *const>::value_type(endNode->id, endNode));
                            } else {
                                // 存在则获取已存在的点
                                endNode = this->nodeList[endNodePair[1]];
                            }

                            // 将当前边增加至起点和终点的链表中
                            beginNode->addEdge(endNode);
                            endNode->addEdge(beginNode);
                        }
                    }
                }
                // 关闭文件
                graphDefineFile.close();
            }
        }
    }
}

Graph::~Graph() {
    // 遍历图的节点列表逐一析构每个点
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        // 因为图中的点是存储在堆内存中的所以要手动回收
        delete iter->second;
        // 从节点列表中删除该节点避免野指针
        this->nodeList.erase(iter);
    }
}

std::vector<std::string> Graph::traverse(const std::string &beginNodeID, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const {
    // 初始化遍历序列的ID列表
    // 序列为节点按遍历前后顺序形成ID列表
    std::vector<std::string> traverseSequenceList;
    if (strategy == FIRST_NO_VISIT || strategy == LAST_NO_VISIT || strategy == RANDOM_NO_VISIT) {
        // 查询开始节点在图中是否存在
        if (this->nodeList.contains(beginNodeID)) {
            Graph::traverse(traverseSequenceList, this->nodeList.at(beginNodeID), type, strategy);
        } else {
            // 设置的遍历开始节点ID不存在
            LOG(ERROR) << "开始节点不存在！";
        }
    } else {
        // 遍历方法只能使用"未访问"类型的边选择策略
        LOG(ERROR) << "遍历方法只能选择'FIRST_NO_VISIT'、'LAST_NO_VISIT'或者'RANDOM_NO_VISIT'三种边选择策略不存在！";
    }

    return traverseSequenceList;
}

std::vector<std::string> Graph::traverse(const Node &node, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const {
    // 调用传入开始节点ID的遍历方法
    return this->traverse(node.id, type, strategy);
}

void Graph::walk() {
//    // 判断是否设置开始点
//    if (this->beginNode == nullptr) {
//        // 未设置则退出游走
//        std::cout << "111" << std::endl;
//        return;
//    }
//
//    if (this->endNode == nullptr && this->walkingStep == 0) {
//        // 未设置结束点且未设置游走步数则退出游走
//        std::cout << "222" << std::endl;
//        return;
//    }
//
//    Node *&currentNode = this->beginNode;
//    int walkingStepCount = this->walkingStep;
//
//    std::cout << currentNode->id << std::endl;
//
//    while (currentNode != nullptr && walkingStepCount > 0) {
//        // 当前点的链表
//        const std::vector<Node *> &linkedNodeList = currentNode->linkedNodeList;
//
//        // 当前点存在边时继续游走
//        if (!linkedNodeList.empty()) {
//            if (this->walkingDirection == DEEP) {
//                // 深度遍历
//                if (this->edgeChooseStrategy == FIRST) {
//                    currentNode = linkedNodeList[0];
//                } else if (this->edgeChooseStrategy == LAST) {
//                    currentNode = linkedNodeList[linkedNodeList.size() - 1];
//                } else if (this->edgeChooseStrategy == MIDDLE) {
//
//                } else if (this->edgeChooseStrategy == FIRST_NO_VISIT){
//
//                } else if (this->edgeChooseStrategy == LAST_NO_VISIT){
//
//                } else if (this->edgeChooseStrategy == MIDDLE_NO_VISIT){
//
//                } else if (this->edgeChooseStrategy == RANDOM_NO_VISIT){
//
//                } else if (this->edgeChooseStrategy == CUSTOMIZED) {
//
//                }
//            } else {
//                // 宽度遍历
//            }
//            std::cout << currentNode->id << std::endl;
//            walkingStepCount--;
//        } else {
//            break;
//        }
//    }
}

void Graph::reset() {
    // 遍历图中全部节点重置节点状态
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        iter->second->reset();
    }
}

void Graph::traverse(std::vector<std::string> &traverseSequenceList, Node *const &beginNode, const WalkingDirection &direction,
                     const EdgeChooseStrategy &strategy) {
    std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

    if (direction == WalkingDirection::DEEP) {
        std::stack<Node *const> depthStack;

        depthStack.push(beginNode);
        while (!depthStack.empty()) {
            Node *const n = depthStack.top();
            depthStack.pop();

            // 访问该点
            n->visit();
            // 将该点ID加入遍历去序列
            traverseSequenceList.push_back(n->id);

            // 获得该点全部未访问链接节点
            const std::vector<Node*> &linkedNodeList = n->getNoVisitedLinkedNodeList();
            if (!linkedNodeList.empty()) {
                if (strategy == FIRST_NO_VISIT) {
                    for (auto iter = linkedNodeList.crbegin(); iter != linkedNodeList.crend(); ++iter) {
                        depthStack.push(*iter);
                    }
                } else if (strategy == LAST_NO_VISIT) {
                    for (auto iter = linkedNodeList.cbegin(); iter != linkedNodeList.cend(); ++iter) {
                        depthStack.push(*iter);
                    }
                } else {
                    std::vector<Node*> shuffleLinkedNodeList = linkedNodeList;
                    std::shuffle(shuffleLinkedNodeList.begin(), shuffleLinkedNodeList.end(), randomEngine);
                    for (auto iter = shuffleLinkedNodeList.cbegin(); iter != shuffleLinkedNodeList.cend(); ++iter) {
                        depthStack.push(*iter);
                    }
                }
            }
        }
    } else {
        // 广度优先遍历

        // 初始化度列
        std::queue<Node *const> levelQueue;
        // 将当前节点放入队列
        levelQueue.push(beginNode);
        // 广度遍历全部节点
        while (!levelQueue.empty()) {
            // 从队列中取出当前节点
            Node *const n = levelQueue.front();
            levelQueue.pop();

            // 访问该点
            n->visit();
            // 将该点ID加入遍历去序列
            traverseSequenceList.push_back(n->id);

            // 获得该点全部未访问链接节点
            const std::vector<Node *> &linkedNodeList = n->getNoVisitedLinkedNodeList();
            if (strategy == FIRST_NO_VISIT) {
                for (auto iter = linkedNodeList.cbegin(); iter != linkedNodeList.cend(); ++iter) {
                    levelQueue.push(*iter);
                }
            } else if (strategy == LAST_NO_VISIT) {
                for (auto iter = linkedNodeList.crbegin(); iter != linkedNodeList.crend(); ++iter) {
                    levelQueue.push(*iter);
                }
            } else {
                std::vector<Node*> shuffleLinkedNodeList = linkedNodeList;
                std::shuffle(shuffleLinkedNodeList.begin(), shuffleLinkedNodeList.end(), randomEngine);
                for (auto iter = shuffleLinkedNodeList.cbegin(); iter != shuffleLinkedNodeList.cend(); ++iter) {
                    levelQueue.push(*iter);
                }
            }
        }
    }
}