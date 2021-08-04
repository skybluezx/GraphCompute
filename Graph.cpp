//
// Created by 翟霄 on 2021/7/16.
//

#include "Graph.h"

#include <iostream>
#include <filesystem>
#include <stack>
#include <queue>
#include <random>
#include <chrono>
#include "boost/filesystem.hpp"

#include "type_defination.h"
#include "Util.h"
#include "Node.h"

Graph::Graph() = default;

Graph::Graph(const std::string &graphDefineFileDirectoryPath, const int &readEdgeCount) {
    std::filesystem::path path(graphDefineFileDirectoryPath);
    // 判断图定义文件的路径是否存在
    if (!std::filesystem::exists(path)) {
        // 图定义文件的目录不存在
        LOG(ERROR) << "图定义目录不存在！";
    } else {
        // 路径存在
        // 遍历图定义文件所在目录的全部图定义文件
        for (auto it = std::filesystem::recursive_directory_iterator(path); it != std::filesystem::recursive_directory_iterator(); ++it) {
            // 判断当前是否为图定义文件
            auto &entry = *it;
            if (std::filesystem::is_regular_file(entry)) {
                std::string filePathString = entry.path();
                // 不读取"."开头的文件
                // 避免读取临时文件
                boost::filesystem::path filePath(filePathString);
                if (filePath.filename().string()[0] == '.') {
                    LOG(INFO) << "跳过文件：" << filePath.filename().string();
                    continue;
                }

                LOG(INFO) << "读取文件：" << filePath.filename();

                std::ifstream graphDefineFile(filePathString, std::ios::in);
                // 判断图定义文件打开是否成功
                if (!graphDefineFile.is_open()) {
                    // 打开失败则输出错误日志
                    LOG(ERROR) << "文件读取失败！";
                } else {
                    int currentEdgeCount = 0;
                    // 遍历图定义文件的全部行
                    // 每一行为一个边描述
                    std::string line;
                    while (std::getline(graphDefineFile, line)) {
                        if (line.empty()) {
                            continue;
                        }
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
                            if (!this->nodeList.contains(beginNodePair[0] + ":" + beginNodePair[1])) {
                                // 不存在则创建起点对应的点对象
                                beginNode = new Node(beginNodePair[1], beginNodePair[0]);
                                // 将创建的点增加至全局点字典
                                this->nodeList.insert(std::make_pair(beginNode->getTypeID(), beginNode));

                                if (!this->nodeTypeCountList.contains(beginNode->getType())) {
                                    this->nodeTypeCountList[beginNode->getType()] = 0;
                                }
                                this->nodeTypeCountList[beginNode->getType()] += 1;
                            } else {
                                // 存在则获取已存在的点
                                beginNode = this->nodeList[beginNodePair[0] + ":" + beginNodePair[1]];
                            }

                            // 初始化终点
                            Node *endNode = nullptr;
                            // 判断终点ID是否在全局点字典中已存在
                            if (!this->nodeList.contains(endNodePair[0] + ":" + endNodePair[1])) {
                                // 不存在则创建终点对应的点对象
                                endNode = new Node(endNodePair[1], endNodePair[0]);
                                // 将创建的点增加至全局点字典
                                this->nodeList.insert(std::make_pair(endNode->getTypeID(), endNode));

                                if (!this->nodeTypeCountList.contains(endNode->getType())) {
                                    this->nodeTypeCountList[endNode->getType()] = 0;
                                }
                                this->nodeTypeCountList[endNode->getType()] += 1;
                            } else {
                                // 存在则获取已存在的点
                                endNode = this->nodeList[endNodePair[0] + ":" + endNodePair[1]];
                            }

                            // 将当前边增加至起点和终点的链表中
                            beginNode->addEdge(endNode);
                            endNode->addEdge(beginNode);

                            currentEdgeCount++;
                            LOG(INFO) << "添加边成功！当前边数：" << currentEdgeCount;

                            if (readEdgeCount >= 0 && currentEdgeCount >= readEdgeCount) {
                                break;
                            }
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
    }

    // 从节点列表中删除节点避免野指针
    this->nodeList.erase(this->nodeList.begin(), this->nodeList.end());
}

const std::map<const std::string, Node *const> &Graph::getNodeList() const {
    return this->nodeList;
}

const int Graph::getNodeVisitedCount(const std::string &id, const std::string &type) const {
    if (this->nodeList.contains(type + ":" + id)) {
        return this->nodeList.at(type + ":" + id)->getVisitedCount();
    } else {
        // 若对应ID的点不存在则返回-1
        return -1;
    }
}

std::vector<std::string> Graph::getNodeTypeList() const {
    std::vector<std::string> typeList;
    for (auto iter = this->nodeTypeCountList.begin(); iter != this->nodeTypeCountList.end(); ++iter) {
        typeList.push_back(iter->first);
    }
    return typeList;
}

const std::map<std::string, unsigned> &Graph::getNodeTypeCountList() const {
    return this->nodeTypeCountList;
}

std::vector<std::string> Graph::traverse(const std::string &beginNodeType, const std::string &beginNodeID, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const {
    // 初始化遍历序列的ID列表
    // 序列为节点按遍历前后顺序形成ID列表
    std::vector<std::string> traverseSequenceList;
    if (strategy == FIRST_NO_VISIT || strategy == LAST_NO_VISIT || strategy == RANDOM_NO_VISIT) {
        // 查询开始节点在图中是否存在
        if (this->nodeList.contains(beginNodeType + ":" + beginNodeID)) {
            Graph::traverse(traverseSequenceList, this->nodeList.at(beginNodeType + ":" + beginNodeID), type, strategy);
        } else {
            // 设置的遍历开始节点ID不存在
            LOG(ERROR) << "开始节点不存在！";
        }
    } else {
        // 遍历方法只能使用"未访问"类型的边选择策略
        LOG(ERROR) << "遍历方法只能选择'FIRST_NO_VISIT'、'LAST_NO_VISIT'或者'RANDOM_NO_VISIT'三种边选择策略！";
    }

    return traverseSequenceList;
}

std::vector<std::string> Graph::traverse(const Node &beginNode, const WalkingDirection &type, const EdgeChooseStrategy &strategy) const {
    // 调用传入开始节点ID的遍历方法
    return this->traverse(beginNode.getType(), beginNode.getID(), type, strategy);
}

std::vector<std::string> Graph::walk(const std::string &beginNodeType,
                                     const std::string &beginNodeID,
                                     const std::vector<std::string> &stepDefine,
                                     const std::map<std::string, std::string> &auxiliaryEdge,
                                     const float &walkLengthRatio,
                                     const int &totalStepCount,
                                     const EdgeChooseStrategy &strategy,
                                     const bool &resetGraph) {
    // 初始化游走节点ID序列
    std::vector<std::string> walkingSequence;

    // 检查开始点是否存在
    if (!this->nodeList.contains(beginNodeType + ":" + beginNodeID)) {
        LOG(ERROR) << "开始点不存在！";
        return walkingSequence;
    }

    // 获取开始点ID对应的点指针
    Node *beginNode = this->nodeList.at(beginNodeType + ":" + beginNodeID);

    // 检查游走步定义是否正确
    if (!stepDefine.empty()) {
        if (beginNode->getType() != stepDefine[0]) {
            // 开始点类型不匹配游走步类型定义
            LOG(ERROR) << "开始点类型不匹配游走步类型定义！";
            return walkingSequence;
        }
    } else {
        // 游走步未定义
        LOG(ERROR) << "游走步未定义！";
        return walkingSequence;
    }

    // 判断是否需要重置图中全部节点的状态信息
    if (resetGraph) {
        this->reset();
    }

    // 定义开始点指针
    Node *currentNode;
    // 初始化当前已完成步数为0
    int currentStepCount = 0;
    // 定义当前游走的步长
    int walkingLength;

    // 当游走步数小于总步数时继续游走
    while (currentStepCount < totalStepCount) {
        LOG(INFO) << "新游走！当前步数：" << currentStepCount << "/" << totalStepCount;
        // 访问开始点
        currentNode = beginNode;

        // 计算当前步游走长度
        // Todo
        // 步长的计算需要更多试验和思考！！
        if (walkLengthRatio >= 0) {
            // 如果步长参数大等于0则计算开始点的度数与该参数的乘积作为本次步长
            // 开始点的度数只考虑步长定义中该开始点之后的第二类节点的个数
            walkingLength = currentNode->getLinkedNodeMapList().at(stepDefine[1]).size() * walkLengthRatio;
        } else {
            // 如果步长参数小于0则取该参数的绝对值作为本次游走的步长
            walkingLength = 0 - walkLengthRatio;
        }

        // 遍历步长
        for (int i = 0; i < walkingLength; ++i) {
            LOG(INFO) << "向前一步！当前游走的总步数：" << i + 1 << "/" << walkingLength;

            // 遍历步的组成
            // 从开始点开始遍历
            auto j = 0;
            while (j < stepDefine.size()) {
                // 判断当前点是否不存在
                if (currentNode != nullptr) {
                    // 访问当前步的开始点
                    currentNode->visit();
                    walkingSequence.push_back(currentNode->getTypeID());
                    // 输出日志
                    if (j == 0) {
                        // 每一步的开始点
                        LOG(INFO) << "访问当前步的开始点:" << stepDefine[0] << ":" << currentNode->getID();
                    } else {
                        // 每一步的非开始点
                        LOG(INFO) << "访问下一点：" << currentNode->getType() << ":" << "节点ID：" << currentNode->getID();
                    }

                    // 访问辅助边
                    // Todo
                    // (1) 同一类点存在连接多种点的辅助边（当前只能有一种辅助边）
                    // (2) 辅助边对应的辅助点是否还可以拥有辅助边？（目前辅助边不能再拥有辅助边）
                    if (auxiliaryEdge.contains(currentNode->getType())) {
                        LOG(INFO) << "访问辅助边";
                        auto auxiliaryNode = currentNode->getNextLinkedNode(strategy, auxiliaryEdge.at(
                                currentNode->getType()));
                        if (auxiliaryNode != nullptr) {
                            // 存在辅助点则访问
                            auxiliaryNode->visit();
                            walkingSequence.push_back(auxiliaryNode->getTypeID());
                            LOG(INFO) << "访问辅助点：" << auxiliaryNode->getType() << ":" << "节点ID："
                                      << auxiliaryNode->getID();
                            // 从辅助点返回
                            // 因为是从必选点游走至该辅助点的，该辅助点至少存在一个返回必选点的边，所以这里获取的节点不可能是空指针
                            currentNode = auxiliaryNode->getNextLinkedNode(strategy, currentNode->getType());
                            if (currentNode != nullptr) {
                                currentNode->visit();
                                walkingSequence.push_back(currentNode->getTypeID());
                                LOG(INFO) << "辅助点返回：" << currentNode->getType() << ":" << "节点ID："
                                          << currentNode->getID();
                            } else {
                                LOG(ERROR) << "辅助点返回出错！未获取返回点！";
                            }
                        } else {
                            LOG(INFO) << "当前点无辅助边！";
                        }
                    } else {
                        LOG(INFO) << "当前种类的点不存在辅助边！";
                    }

                    // 判断当前是否完成一步
                    if (j < stepDefine.size() - 1) {
                        // 尚未完成一步继续游走至步长定义中的下一步
                        currentNode = currentNode->getNextLinkedNode(strategy, stepDefine[j + 1]);
                    } else {
                        // 已完成一步游走至下一步的开始点
                        currentNode = currentNode->getNextLinkedNode(strategy, stepDefine[0]);
                        LOG(INFO) << "完成一步！前进至下一步的开始节点";
                    }

                    // 步长定义的索引增1
                    j++;
                } else {
                    // 若不存在则上一个节点没有步长定义中的下一个节点，或者当前步最后一个节点不连接步长定义中的开始点，导致无路可走

                    // ToDo
                    // 重启策略
                    // 当前游走到的点已经没有步长定义中指定类型的连接点
                    LOG(INFO) << "访问失败！不存在步长定义中指定类型的当前点！";
                    break;
                }
            }

            // ToDo
            // 重启策略
            // 当前游走到的点已经没有步长定义中指定类型的连接点
            if (currentNode == nullptr) {
                break;
            }
        }

        // 刷新当前步数
        currentStepCount += walkingLength;
    }

    return walkingSequence;
}

std::vector<std::string> Graph::walk(const Node &beginNode,
                                     const std::vector<std::string> &stepDefine,
                                     const std::map<std::string, std::string> &auxiliaryEdge,
                                     const float &walkLengthRatio,
                                     const int &totalStepCount,
                                     const EdgeChooseStrategy &strategy,
                                     const bool &resetGraph) {
    return this->walk(beginNode.getType(), beginNode.getID(), stepDefine, auxiliaryEdge, walkLengthRatio, totalStepCount, strategy, resetGraph);
}

void Graph::reset() {
    // 遍历图中全部节点重置节点状态
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        iter->second->reset();
    }
}

std::vector<std::pair<std::string, int>> Graph::getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence) const {
    std::vector<std::pair<std::string, int>> nodeVisitedCountList;
    std::map<std::string, int> nodeIDList;
    for (auto i = 0; i < walkingSequence.size(); ++i) {
        if (this->nodeList.contains(walkingSequence[i])) {
            if (!nodeIDList.contains(walkingSequence[i])) {
                nodeIDList[walkingSequence[i]] = 1;
                nodeVisitedCountList.emplace_back(std::pair<std::string, int>(this->nodeList.at(walkingSequence[i])->getTypeID(), this->nodeList.at(
                        walkingSequence[i])->getVisitedCount()));
            }
        }
    }
    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);

    return nodeVisitedCountList;
}

std::vector<std::pair<std::string, int>> Graph::getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence, const std::string &nodeType) const {
    std::vector<std::pair<std::string, int>> nodeVisitedCountList;
    std::map<std::string, int> nodeTypeIDList;
    for (auto i = 0; i < walkingSequence.size(); ++i) {
        if (this->nodeList.contains(walkingSequence[i])) {
            if (this->nodeList.at(walkingSequence[i])->getType() == nodeType) {
                if (!nodeTypeIDList.contains(walkingSequence[i])) {
                    nodeTypeIDList[walkingSequence[i]] = 1;
                    nodeVisitedCountList.emplace_back(
                            std::pair<std::string, int>(this->nodeList.at(walkingSequence[i])->getTypeID(),
                                                        this->nodeList.at(walkingSequence[i])->getVisitedCount()));
                }
            }
        }
    }
    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);

    return nodeVisitedCountList;
}

bool Graph::isLinked(const std::string &aNodeType, const std::string &aNodeID, const std::string &bNodeType, const std::string &bNodeID, std::vector<std::string> &traverseSequenceList) {
    bool result;

    // 清空遍历路径
    std::vector<std::string> list;
    traverseSequenceList.swap(list);

    // 判断两点在图中是否存在
    if (!this->nodeList.contains(aNodeType + ":" + aNodeID) || !this->nodeList.contains(bNodeType + ":" + bNodeID)) {
        result = false;
    }
    // 获取起点指针
    Node *beginNode = this->nodeList.at(aNodeType + ":" + aNodeID);

    this->reset();

    // 深度优先遍历
    // 选择FIRST_NO_VISIT策略
    // 配置结束点ID
    Graph::traverse(traverseSequenceList, beginNode, WalkingDirection::DEEP, EdgeChooseStrategy::FIRST_NO_VISIT, bNodeType + ":" + bNodeID);

    // 判断遍历ID路径是否为空
    if (!traverseSequenceList.empty()) {
        // 判断结束点是否为重点ID
        std::cout << traverseSequenceList[traverseSequenceList.size() - 1] << std::endl;
        std::cout << bNodeType + ":" + bNodeID << std::endl;
        if (traverseSequenceList[traverseSequenceList.size() - 1] == bNodeType + ":" + bNodeID) {
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    return result;
}

void Graph::traverse(std::vector<std::string> &traverseSequenceList, Node *const &beginNode, const WalkingDirection &direction,
                     const EdgeChooseStrategy &strategy, const std::string &endNodeTypeID) {
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
            traverseSequenceList.push_back(n->getTypeID());

            // 若配置了遍历重点则判断是否到达终点
            // 到达则退出遍历，这是遍历ID序列的最后一个元素就是终点ID
            if (!endNodeTypeID.empty() && n->getTypeID() == endNodeTypeID) {
                break;
            }

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
            traverseSequenceList.push_back(n->getTypeID());

            // 若配置了遍历重点则判断是否到达终点
            // 到达则退出遍历，这是遍历ID序列的最后一个元素就是终点ID
            if (!endNodeTypeID.empty() && n->getTypeID() == endNodeTypeID) {
                break;
            }

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

bool Graph::cmp(std::pair<std::string, int> a, std::pair<std::string, int> b) {
    return a.second > b.second;
}