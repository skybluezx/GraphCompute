//
// Created by 翟霄 on 2021/7/16.
//

#include "Graph.h"

#include <iostream>
#include <stack>
#include <queue>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>
#include <filesystem>

#include "type_defination.h"
#include "Util.h"
#include "Node.h"

Graph::Graph(const std::string &graphDefineFileDirectoryPath, const std::string &resultType, const int &readEdgeCount, const int &maxWalkBeginNodeCount) : resultType(resultType), maxWalkBeginNodeCount(maxWalkBeginNodeCount) {
    /**
     * 随机数生成的相关数据结构初始化
     * 该部分随机数结构仅用于单线程模式下重启策略中随机数的生成
     * - 游走过程中用于选择下一个点的随机数生成由每个Node中的随机数生成机制负责
     * - 多线程模式下，不管是重启策略，还是下一个节点的选择，涉及到的随机数均有线程体负责生成，不复用主线程的任何结构
     */
    // 初始化随机引擎（目前用于游走过程中的重启策略）
    this->randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    // 初始化随机数分布（目前初始化为0到1之间的实数）
    this->randomDistribution = std::uniform_real_distribution<double>(0.0, 1.0);

    // 设置图定义文件的路径
    std::filesystem::path path(graphDefineFileDirectoryPath);
    // 判断图定义文件的路径是否存在
    if (!std::filesystem::exists(path)) {
        // 图定义文件的目录不存在
        LOG(ERROR) << "图定义目录不存在！";
    } else {
        // 路径存在

        // 初始化当前已读取边数
        int currentEdgeCount = 0;
        // 遍历图定义文件所在目录的全部图定义文件
        for (auto it = std::filesystem::recursive_directory_iterator(path); it != std::filesystem::recursive_directory_iterator(); ++it) {
            // 判断当前是否为图定义文件
            auto &entry = *it;
            if (std::filesystem::is_regular_file(entry)) {
                std::string filePathString = entry.path();
                // 不读取"."开头的文件
                // 避免读取临时文件，也可用于文件的选择
                boost::filesystem::path filePath(filePathString);
                if (filePath.filename().string()[0] == '.') {
                    LOG(INFO) << "跳过文件：" << filePath.filename().string();
                    continue;
                }

                LOG(INFO) << "读取文件：" << filePath.filename();

                // 打开文件
                std::ifstream graphDefineFile(filePathString, std::ios::in);
                // 判断图定义文件打开是否成功
                if (!graphDefineFile.is_open()) {
                    // 打开失败则输出错误日志
                    LOG(ERROR) << "文件读取失败！";
                } else {
                    // 遍历图定义文件的全部行
                    // 每一行为一个边描述
                    std::string line;
                    while (std::getline(graphDefineFile, line)) {
                        // 空行跳过
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

                                // 累加当前点对应类型的总节点数
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

                                // 累加当前点对应类型的总节点数
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

                            // 累加当前已读取总边数
                            currentEdgeCount++;

#ifdef INFO_LOG_OUTPUT
                            LOG(INFO) << "添加边成功！当前边数：" << currentEdgeCount;
#endif

                            // 判断是否已读取配置文件中配置的最大边数
                            if (readEdgeCount >= 0 && currentEdgeCount >= readEdgeCount) {
                                // 若已读取最大边数退出不在读取
                                break;
                            }
                        }
                    }
                    LOG(INFO) << "文件读取完成！";
                }
                // 关闭文件
                graphDefineFile.close();
            }

            // 判断是否已读取配置文件中配置的最大边数
            if (readEdgeCount >= 0 && currentEdgeCount >= readEdgeCount) {
                // 若已读取最大边数退出不在读取
                break;
            }
        }
        LOG(INFO) << "图加载完毕！";
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

const int &Graph::getMaxWalkBeginNodeCount() const {
    return this->maxWalkBeginNodeCount;
}

const std::map<const std::string, std::vector<Node *>> &Graph::getTypeNodeList() const {
    return this->typeNodeList;
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

const std::map<std::string, std::map<std::string, int>> &Graph::getNodeDegreeList() const {
    return this->nodeDegreeList;
}

const std::map<std::string, std::map<std::string, int>> &Graph::getNodeTypeMaxDegreeList() const {
    return this->nodeTypeMaxDegreeList;
}

const std::vector<std::unordered_map<std::string, unsigned int>> &Graph::getVisitedNodeTypeIDCountList() const {
    return this->visitedNodeTypeIDCountList;
}

std::vector<std::string> Graph::traverse(const std::string &beginNodeType,
                                         const std::string &beginNodeID,
                                         const WalkingDirection &type,
                                         const EdgeChooseStrategy &strategy,
                                         const bool &keepVisitedCount) const {
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

std::vector<std::string> Graph::traverse(const Node &beginNode,
                                         const WalkingDirection &type,
                                         const EdgeChooseStrategy &strategy,
                                         const bool &keepVisitedCount) const {
    // 调用传入开始节点ID的遍历方法
    return this->traverse(beginNode.getType(), beginNode.getID(), type, strategy);
}

void Graph::walk(const std::string &beginNodeType,
                 const std::string &beginNodeID,
                 const std::vector<std::string> &stepDefine,
                 const std::map<std::string, std::string> &auxiliaryEdge,
                 const float &walkLengthRatio,
                 const float &restartRatio,
                 const unsigned int &totalStepCount,
                 const bool &keepVisitedCount) {
    if (!keepVisitedCount) {
        // 清空图中全部节点的状态
        this->reset();
    }

    // 清空游走结果列表
    this->clearResultList();

    // 检查开始点是否存在
    if (!this->nodeList.contains(beginNodeType + ":" + beginNodeID)) {
        LOG(ERROR) << "开始点不存在！" << beginNodeType + ":" + beginNodeID;
        return;
    }

    // 获取开始点ID对应的点指针
    Node *beginNode = this->nodeList.at(beginNodeType + ":" + beginNodeID);

    // 检查步长定义是否正确
    if (!stepDefine.empty()) {
        if (std::find(stepDefine.begin(), stepDefine.end(), beginNode->getType()) == stepDefine.end()) {
            // 开始点类型不匹配游走步类型定义
            LOG(ERROR) << "开始点类型不匹配游走步类型定义！";
            return;
        }
    } else {
        // 步长未定义
        LOG(ERROR) << "游走步未定义！";
        return;
    }

    // 定义开始点指针
    Node *currentNode;
    // 初始化当前已完成步数为0
    int currentStepCount = 0;
    // 定义当前游走的步长
    int walkingLength;

    // 计算一次游走的长度
    if (walkLengthRatio > 0 && walkLengthRatio <= 1) {
        // 如果步长参数大等于0则计算开始点的度数与该参数的乘积作为本次步长
        // 开始点的度数只考虑步长定义中该开始点之后的第二类节点的个数
        walkingLength = beginNode->getLinkedNodeMapList().at(stepDefine[1]).first.size() * walkLengthRatio;
    } else if (walkLengthRatio == 0) {
        // 单次游走步长定义为0则不限制单次游走步长
        walkingLength = totalStepCount;
    } else if (walkLengthRatio < 0) {
        // 如果步长参数小于0则取该参数的绝对值作为本次游走的步长
        walkingLength = 0 - walkLengthRatio;
    } else {
        LOG(ERROR) << "单次游走步长未定义！";
    }

    // 当游走步数小于总步数时继续游走
    while (currentStepCount < totalStepCount) {
#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[新游走] 当前总步数/设置总步数：" << currentStepCount << "/" << totalStepCount;
#endif
        // 回到开始点
        currentNode = beginNode;

        // 初始化一次游走的步长
        int length = 0;
        for (int i = 0; i < walkingLength; ++i) {
#ifdef INFO_LOG_OUTPUT
            LOG(INFO) << "[向前一步] 本次游走总步数/步长：" << i + 1 << "/" << walkingLength;
#endif
            // 访问当前步的开始点
            currentNode->visit();
            this->insertResultList(currentNode);
            // 本次已游走步长加1
            length++;
#ifdef INFO_LOG_OUTPUT
            LOG(INFO) << "[访问当前点] " << currentNode->getType() << ":" << currentNode->getID();
#endif

            // 访问辅助边
            // Todo
            // (1) 同一类点存在连接多种点的辅助边（当前只能有一种辅助边）
            // (2) 辅助边对应的辅助点是否还可以拥有辅助边？（目前辅助边不能再拥有辅助边）
            if (auxiliaryEdge.contains(currentNode->getType())) {
                Node *auxiliaryNode;
                currentNode->getNextRandomLinkedNode(auxiliaryNode, auxiliaryEdge.at(currentNode->getType()));
                if (auxiliaryNode != nullptr) {
                    // 存在辅助点则访问
                    auxiliaryNode->visit();
                    this->insertResultList(auxiliaryNode);
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "[访问辅助点] " << auxiliaryNode->getType() << ":" << "节点ID：" << auxiliaryNode->getID();
#endif
                    // 从辅助点返回
                    // 因为是从必选点游走至该辅助点的，该辅助点至少存在一个返回必选点的边，所以这里获取的节点不可能是空指针
                    auxiliaryNode->getNextRandomLinkedNode(currentNode, currentNode->getType());
                    if (currentNode != nullptr) {
                        currentNode->visit();
                        this->insertResultList(currentNode);
#ifdef INFO_LOG_OUTPUT
                        LOG(INFO) << "[辅助点返回] " << currentNode->getType() << ":" << "节点ID：" << currentNode->getID();
#endif
                    } else {
#ifdef INFO_LOG_OUTPUT
                        LOG(ERROR) << "辅助点返回出错！未获取返回点！";
#endif
                    }
                } else {
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "当前点无辅助边！";
#endif
                }
            } else {
#ifdef INFO_LOG_OUTPUT
                LOG(INFO) << "当前种类的点不存在辅助边！";
#endif
            }

            // 判断重启概率是否大于零
            if (restartRatio > 0) {
                // 重启概率大于0时启动重启策略
                // 生成0-1之间的随机数
                // 判断随机数是否小于重启概率
                if (this->randomDistribution(this->randomEngine) < restartRatio) {
                    // 小于则将当前游走的步数置为最大步数退出本次迭代
                    // 由于本次迭代步数已置为最大步数则将继续退出本次游走返回起点
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "[重启]";
#endif
                    break;
                }
            }

            // 获取当前点的下一个点
            currentNode->getNextRandomLinkedNode(currentNode, stepDefine);
            // 判断当前点是否存在符合步长定义的下一个节点             
            if (currentNode == nullptr) {
                // 不存则则结束本次游走
                break;
            }
        }

        // 刷新当前步数
        currentStepCount += length;
    }
}

void Graph::walk(const Node &beginNode,
                 const std::vector<std::string> &stepDefine,
                 const std::map<std::string, std::string> &auxiliaryEdge,
                 const float &walkLengthRatio,
                 const float &restartRatio,
                 const unsigned int &totalStepCount,
                 const bool &keepVisitedCount) {
    this->walk(beginNode.getType(), beginNode.getID(), stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount, keepVisitedCount);
}

void Graph::walkOnThread(const std::string &beginNodeType, const std::string &beginNodeID,
                         const std::vector<std::string> &stepDefine,
                         const std::map<std::string, std::string> &auxiliaryEdge, const float &walkLengthRatio,
                         const float &restartRatio,
                         const unsigned int &totalStepCount,
                         std::unordered_map<std::string, unsigned int> &nodeVisitedCountList,
                         const bool &keepVisitedCount) {
    // 由于随机数生成涉及的数据结构不安全，所以在线程体内生成线程独立的相关数据结构
    // 当前线程的随机引擎
    std::default_random_engine randomEngine;
    randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    // 初始化0到1之间实数的随机数分布（用于重启策略）
    std::uniform_real_distribution<double> randomDoubleDistribution = std::uniform_real_distribution<double>(0.0, 1.0);

    // 清空访问次数字典
    for (auto iter = nodeVisitedCountList.begin(); iter != nodeVisitedCountList.end(); ++iter) {
        iter->second = 0;
    }

    // 检查开始点是否存在
    if (!this->nodeList.contains(beginNodeType + ":" + beginNodeID)) {
        LOG(ERROR) << "开始点不存在！" << beginNodeType + ":" + beginNodeID;
        return;
    }

    // 获取开始点ID对应的点指针
    Node *beginNode = this->nodeList.at(beginNodeType + ":" + beginNodeID);

    // 检查步长定义是否正确
    if (!stepDefine.empty()) {
        if (std::find(stepDefine.begin(), stepDefine.end(), beginNode->getType()) == stepDefine.end()) {
            // 开始点类型不匹配游走步类型定义
            LOG(ERROR) << "开始点类型不匹配游走步类型定义！";
            return;
        }
    } else {
        // 步长未定义
        LOG(ERROR) << "游走步未定义！";
        return;
    }

    // 定义开始点指针
    Node *currentNode;
    // 初始化当前已完成步数为0
    int currentStepCount = 0;
    // 定义当前游走的步长
    int walkingLength;

    // 计算一次游走的长度
    if (walkLengthRatio > 0 && walkLengthRatio <= 1) {
        // 如果步长参数大等于0则计算开始点的度数与该参数的乘积作为本次步长
        // 开始点的度数只考虑步长定义中该开始点之后的第二类节点的个数
        walkingLength = beginNode->getLinkedNodeMapList(stepDefine).size() * walkLengthRatio;
    } else if (walkLengthRatio == 0) {
        walkingLength = totalStepCount;
    } else if (walkLengthRatio < 0) {
        // 如果步长参数小于0则取该参数的绝对值作为本次游走的步长
        walkingLength = 0 - walkLengthRatio;
    }

    // 当游走步数小于总步数时继续游走
    while (currentStepCount < totalStepCount) {
#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[新游走] 当前总步数/设置总步数：" << currentStepCount << "/" << totalStepCount;
#endif
        // 访问开始点
        currentNode = beginNode;

        // 遍历步长
        int length = 0;
        for (int i = 0; i < walkingLength; ++i) {
#ifdef INFO_LOG_OUTPUT
            LOG(INFO) << "[向前一步] 本次游走总步数/步长：" << i + 1 << "/" << walkingLength;
#endif
            // 访问当前点
            nodeVisitedCountList.at(currentNode->getTypeID())++;
            // 本次已游走步长加1
            length++;
#ifdef INFO_LOG_OUTPUT
            LOG(INFO) << "[访问当前点] " << currentNode->getType() << ":" << currentNode->getID();
#endif
            // 访问辅助边
            // Todo
            // (1) 同一类点存在连接多种点的辅助边（当前只能有一种辅助边）
            // (2) 辅助边对应的辅助点是否还可以拥有辅助边？（目前辅助边不能再拥有辅助边）
            if (auxiliaryEdge.contains(currentNode->getType())) {
                Node *auxiliaryNode; 
                currentNode->getNextRandomLinkedNode(auxiliaryNode, auxiliaryEdge.at(currentNode->getType()));
                if (auxiliaryNode != nullptr) {
                    // 存在辅助点则访问
                    nodeVisitedCountList.at(auxiliaryNode->getTypeID())++;
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "[访问辅助点] " << auxiliaryNode->getType() << ":" << "节点ID：" << auxiliaryNode->getID();
#endif
                    // 从辅助点返回
                    // 因为是从必选点游走至该辅助点的，该辅助点至少存在一个返回必选点的边，所以这里获取的节点不可能是空指针
                    auxiliaryNode->getNextRandomLinkedNode(currentNode, currentNode->getType());
                    if (currentNode != nullptr) {
                        nodeVisitedCountList.at(currentNode->getTypeID())++;
#ifdef INFO_LOG_OUTPUT
                        LOG(INFO) << "[辅助点返回] " << currentNode->getType() << ":" << "节点ID：" << currentNode->getID();
#endif
                    } else {
#ifdef INFO_LOG_OUTPUT
                        LOG(ERROR) << "辅助点返回出错！未获取返回点！";
#endif
                    }
                } else {
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "当前点无辅助边！";
#endif
                }
            } else {
#ifdef INFO_LOG_OUTPUT
                LOG(INFO) << "当前种类的点不存在辅助边！";
#endif
            }

            // 判断重启概率是否大于零
            if (restartRatio > 0) {
                // 重启概率大于0时启动重启策略
                // 生成0-1之间的随机数
                // 判断随机数是否小于重启概率
                if (randomDoubleDistribution(randomEngine) < restartRatio) {
                    // 小于则将当前游走的步数置为最大步数退出本次迭代
                    // 由于本次迭代步数已置为最大步数则将继续退出本次游走返回起点
#ifdef INFO_LOG_OUTPUT
                    LOG(INFO) << "[重启]";
#endif
                    break;
                }
            }

            // 获取当前点的下一个点
            currentNode->getNextRandomLinkedNode(currentNode, stepDefine);
            // 判断当前点是否存在符合步长定义的下一个节点
            if (currentNode == nullptr) {
                // 不存则则结束本次游走
                break;
            }
        }
        // 刷新当前步数
        currentStepCount += length;
    }
}

void Graph::walkOnThread1(const std::string &beginNodeType,
                          const std::string &beginNodeID,
                          const float &restartRatio,
                          const unsigned int &totalStepCount,
                          std::unordered_map<std::string, unsigned int> &nodeVisitedCountList) {
    // 由于随机数生成涉及的数据结构不安全，所以在线程体内生成线程独立的相关数据结构
    // 当前线程的随机引擎
    std::default_random_engine randomEngine;
    randomEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    // 初始化0到1之间实数的随机数分布（用于重启策略）
    std::uniform_real_distribution<double> randomDoubleDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
    
    // 清空访问次数字典
    for (auto iter = nodeVisitedCountList.begin(); iter != nodeVisitedCountList.end(); ++iter) {
        iter->second = 0;
    }
    
    // 检查开始点是否存在
    if (!this->nodeList.contains(beginNodeType + ":" + beginNodeID)) {
        LOG(ERROR) << "开始点不存在！" << beginNodeType + ":" + beginNodeID;
        return;
    }

    // 获取开始点ID对应的指针
    Node *beginNode = this->nodeList.at(beginNodeType + ":" + beginNodeID);
    // 判断开始点是否是孤立点
    if (beginNode->getLinkedNodeList().empty()) {
        // 若是孤立点则记一次访问然后结束游走
        nodeVisitedCountList[beginNode->getTypeID()] = 1;
        return;
    }

    // 初始化当前节点为开始点
    Node *currentNode = beginNode;
    // 初始化当前已完成步数为0
    int currentStepCount = 0;
    // 当游走步数小于总步数时继续游走
    while (currentStepCount < totalStepCount) {
#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[向前一步] 当前游走步数/总步数：" << currentStepCount << "/" << totalStepCount;
#endif
        // 步数加1
        currentStepCount++;
        // 访问当前步的开始点
        nodeVisitedCountList.at(currentNode->getTypeID())++;
#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[访问当前点] " << currentNode->getTypeID();
#endif
        // 判断重启概率是否大于零
        if (restartRatio > 0) {
            // 重启概率大于0时启动重启策略
            // 生成0-1之间的随机数
            // 判断随机数是否小于重启概率
            if (randomDoubleDistribution(randomEngine) < restartRatio) {
#ifdef INFO_LOG_OUTPUT
                LOG(INFO) << "[重启]";
#endif
                currentNode = beginNode;
                continue;
            }
        }
        // 随机访问下一节点
        std::uniform_int_distribution<int> randomIntDistribution(0, currentNode->getLinkedNodeList().size() - 1);
        currentNode = currentNode->getLinkedNodeList()[randomIntDistribution(randomEngine)].first;
    }
}

void Graph::multiWalk(const std::vector<std::string> &beginNodeTypeList,
                      const std::vector<std::map<std::string, double>> &beginNodeIDList,
                      const std::vector<std::vector<std::string>> &stepDefineList,
                      const std::vector<std::map<std::string, std::string>> &auxiliaryEdgeList,
                      const std::vector<float> &walkLengthRatioList,
                      const std::vector<float> &restartRatioList,
                      const std::vector<unsigned int> &totalStepCountList,
                      const std::vector<bool> &isSplitStepCountList,
                      const bool &keepVisitedCount) {
    // 初始化线程起始编号为0
    unsigned int threadNum = 0;
    // 线程池
    std::vector<std::thread> threadList;

    // 遍历游走组
    for (auto i = 0; i < beginNodeTypeList.size(); ++i) {
#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[游走组" << i << "] ";
        LOG(INFO) << "[计算组内节点游走总步数]";
#endif
        std::map<std::string, unsigned int> stepCountList;
        if (isSplitStepCountList[i]) {
            // 开始点度数权重
            // first为起点自身权重
            // second为起点度数权重
            std::map<std::string, std::pair<double, float>> weightList;
            // 开始点度数权重之和
            float s_sum;
            // 遍历游走组内的多个起点
            for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                std::string secondNodeType = stepDefineList[i][1 % stepDefineList[i].size()];

                // 计算起点度数权重
                if (this->nodeDegreeList.contains(beginNodeTypeList[i] + ":" + iter->first)) {
                    unsigned int degree = this->nodeDegreeList.at(beginNodeTypeList[i] + ":" + iter->first).at(secondNodeType);
                    float s_j = degree * (this->nodeTypeMaxDegreeList[beginNodeTypeList[i]][secondNodeType] - std::log(degree));
                    weightList[iter->first] = std::pair<double, float>(iter->second, s_j);
                    s_sum += s_j;
                } else {
                    weightList[iter->first] = std::pair<double, float>(iter->second, 0);;
                }
            }

            for (auto iter = weightList.begin(); iter != weightList.end(); ++iter) {
                stepCountList[iter->first] = weightList[iter->first].first * (totalStepCountList[i] * (weightList[iter->first].second / s_sum));
#ifdef INFO_LOG_OUTPUT
                LOG(INFO) << "[组内节点" << iter->first << "总步数] " << stepCountList[iter->first];
#endif
            }
#ifdef INFO_LOG_OUTPUT
            LOG(INFO) << "[计算完成]";
            LOG(INFO) << "[启动组内游走]";
#endif
        } else {
            for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
                stepCountList[iter->first] = totalStepCountList[i];
#ifdef INFO_LOG_OUTPUT
                LOG(INFO) << "[组内节点" << iter->first << "总步数] " << stepCountList[iter->first];
#endif
            }
        }

        // 遍历全部开始点启动线程
        for (auto iter = beginNodeIDList[i].begin(); iter != beginNodeIDList[i].end(); ++iter) {
            // 判断当前线程数是否已达到最大线程数
            if (threadNum == this->maxWalkBeginNodeCount) {
                // 达到则退出
                break;
            }            

            threadList.emplace_back(
                    std::thread(&Graph::walkOnThread, this,
                                std::cref(beginNodeTypeList[i]),
                                std::cref(iter->first),
                                std::cref(stepDefineList[i]),
                                std::cref(auxiliaryEdgeList[i]),
                                std::cref(walkLengthRatioList[i]),
                                std::cref(restartRatioList[i]),
                                std::cref(stepCountList[iter->first]),
                                std::ref(this->visitedNodeTypeIDCountList[threadNum]),
                                std::cref(keepVisitedCount))
//                    std::thread(&Graph::walkOnThread1, this,
//                                std::cref(beginNodeTypeList[i]),
//                                std::cref(iter->first),
//                                std::cref(restartRatioList[i]),
//                                std::cref(stepCountList[iter->first]),
//                                std::ref(this->visitedNodeTypeIDCountList[threadNum]))
                                );
            threadNum++;
        }

        // 判断当前线程数是否已达到最大线程数
        if (threadNum == this->maxWalkBeginNodeCount) {
            // 达到则退出
            break;
        }

#ifdef INFO_LOG_OUTPUT
        LOG(INFO) << "[本组游走结束]";
#endif
    }

    for (auto i = 0; i < threadList.size(); ++i) {
        if (threadList[i].joinable()) {
            threadList[i].join();
        }
    }
}

void Graph::reset(const bool &onlyVisitedCount) {
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        iter->second->reset(onlyVisitedCount);
    }
}

//std::vector<std::pair<std::string, int>> Graph::getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence) const {
//    std::vector<std::pair<std::string, int>> nodeVisitedCountList;
//    std::map<std::string, int> nodeIDList;
//    for (auto i = 0; i < walkingSequence.size(); ++i) {
//        if (this->nodeList.contains(walkingSequence[i])) {
//            if (!nodeIDList.contains(walkingSequence[i])) {
//                nodeIDList[walkingSequence[i]] = 1;
//                nodeVisitedCountList.emplace_back(std::pair<std::string, int>(this->nodeList.at(walkingSequence[i])->getTypeID(), this->nodeList.at(
//                        walkingSequence[i])->getVisitedCount()));
//            }
//        }
//    }
//    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
//
//    return nodeVisitedCountList;
//}

//std::vector<std::pair<std::string, int>> Graph::getSortedNodeTypeIDListByVisitedCount(const std::string &nodeType) const {
//    std::vector<std::pair<std::string, int>> nodeVisitedCountList;
//    std::map<std::string, int> nodeIDList;
//    for (auto i = 0; i < walkingSequence.size(); ++i) {
//        if (this->nodeList.contains(walkingSequence[i])) {
//            if (!nodeIDList.contains(walkingSequence[i])) {
//                nodeIDList[walkingSequence[i]] = 1;
//                nodeVisitedCountList.emplace_back(std::pair<std::string, int>(this->nodeList.at(walkingSequence[i])->getTypeID(), this->nodeList.at(
//                        walkingSequence[i])->getVisitedCount()));
//            }
//        }
//    }
//    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
//
//    return nodeVisitedCountList;
//}

//std::vector<std::pair<std::string, int>> Graph::getSortedNodeTypeIDListByVisitedCount(const std::vector<std::string> &walkingSequence, const std::string &nodeType) const {
//    std::vector<std::pair<std::string, int>> nodeVisitedCountList;
//    std::map<std::string, int> nodeTypeIDList;
//    for (auto i = 0; i < walkingSequence.size(); ++i) {
//        if (this->nodeList.contains(walkingSequence[i])) {
//            if (this->nodeList.at(walkingSequence[i])->getType() == nodeType) {
//                if (!nodeTypeIDList.contains(walkingSequence[i])) {
//                    nodeTypeIDList[walkingSequence[i]] = 1;
//                    nodeVisitedCountList.emplace_back(
//                            std::pair<std::string, int>(this->nodeList.at(walkingSequence[i])->getTypeID(),
//                                                        this->nodeList.at(walkingSequence[i])->getVisitedCount()));
//                }
//            }
//        }
//    }
//    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
//
//    return nodeVisitedCountList;
//}

std::vector<std::pair<std::string, int>> Graph::getSortedResultNodeTypeIDListByVisitedCount(const std::string &nodeType, const unsigned int &threadNum) const {
    std::vector<std::pair<std::string, int>> nodeVisitedCountList;

#ifndef ONLY_VISITED_NODE_RESULT
    if (resultType == "walking_sequence") {
        std::map<std::string, int> nodeTypeIDList;
        for (auto i = 0; i < this->walkingSequence.at(threadNum).size(); ++i) {
            if (this->nodeList.contains(walkingSequence.at(threadNum)[i])) {
                if (this->nodeList.at(walkingSequence.at(threadNum)[i])->getType() == nodeType) {
                    if (!nodeTypeIDList.contains(walkingSequence.at(threadNum)[i])) {
                        nodeTypeIDList[walkingSequence.at(threadNum)[i]] = 1;
                        nodeVisitedCountList.emplace_back(
                                std::pair<std::string, int>(this->nodeList.at(walkingSequence.at(threadNum)[i])->getTypeID(),
                                                            this->nodeList.at(walkingSequence.at(threadNum)[i])->getVisitedCount()));
                    }
                }
            }
        }
        std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
    } else {
        auto &typeNodeList = this->typeNodeList.at(nodeType);
        nodeVisitedCountList.reserve(typeNodeList.size());
        for (auto iter = typeNodeList.begin(); iter != typeNodeList.end(); ++iter) {
            nodeVisitedCountList.emplace_back(std::pair((*iter)->getTypeID(), this->visitedNodeTypeIDCountList[threadNum].at((*iter)->getTypeID())));
        }
        std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
    }
#else
    nodeVisitedCountList.reserve(this->visitedNodeTypeIDCountList.at(threadNum).size());
    for (auto iter = this->visitedNodeTypeIDCountList.at(threadNum).begin(); iter != this->visitedNodeTypeIDCountList.at(threadNum).end(); ++iter) {
        if (Graph::getTypeFromTypeID(iter->first) == nodeType) {
            nodeVisitedCountList.emplace_back(std::pair(iter->first, iter->second));
        }
    }
    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
#endif

    return nodeVisitedCountList;
}

std::vector<std::pair<std::string, int>> Graph::getSortedResultNodeIDListByVisitedCount(const std::string &nodeType, const std::vector<unsigned int> &threadNumList) const {
    std::vector<std::pair<std::string, int>> nodeVisitedCountList;    

    const std::vector<Node*> &typeNodeList = this->getTypeNodeList().at(nodeType);
    nodeVisitedCountList.reserve(typeNodeList.size());

    // 访问次数最大的合并策略
    int maxCount;
    for (auto iter = typeNodeList.begin(); iter != typeNodeList.end(); ++iter) {
        maxCount = 0;
        for (auto i = 0; i < threadNumList.size(); ++i) {
            if (this->visitedNodeTypeIDCountList[threadNumList[i]].at((*iter)->getTypeID()) > maxCount) {
                maxCount = this->visitedNodeTypeIDCountList[threadNumList[i]].at((*iter)->getTypeID());
            }
        }
        if (maxCount == 0) {
            continue;
        }
        nodeVisitedCountList.emplace_back(std::pair((*iter)->getID(), maxCount));
    }
    std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
    
    return nodeVisitedCountList;
}

std::vector<std::vector<std::pair<std::string, int>>> Graph::getSortedResultNodeTypeIDListsByVisitedCount(const std::string &nodeType, const std::vector<unsigned int> &threadNumList) const {
    // 定义多个开始点对应的目标节点访问次数列表
    std::vector<std::vector<std::pair<std::string, int>>> nodeVisitedCountLists;
    nodeVisitedCountLists.reserve(threadNumList.size());

    for (auto i = 0; i < threadNumList.size(); ++i) {
        auto &typeNodeList = this->typeNodeList.at(nodeType);

        std::vector<std::pair<std::string, int>> nodeVisitedCountList;
        nodeVisitedCountList.reserve(typeNodeList.size());

        for (auto iter = typeNodeList.begin(); iter != typeNodeList.end(); ++iter) {
            nodeVisitedCountList.emplace_back(std::pair((*iter)->getTypeID(), this->visitedNodeTypeIDCountList[threadNumList[i]].at((*iter)->getTypeID())));
        }

        std::sort(nodeVisitedCountList.begin(), nodeVisitedCountList.end(), cmp);
        nodeVisitedCountLists.emplace_back(nodeVisitedCountList);
    }

    return nodeVisitedCountLists;
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

void Graph::excludeNodes(const std::vector<std::string> &excludeNodeTypeIDList) {
    for (auto i = 0; i < excludeNodeTypeIDList.size(); ++i) {
        if (this->nodeList.contains(excludeNodeTypeIDList[i])) {
            this->nodeList.at(excludeNodeTypeIDList[i])->exclude();
            // 重新生成当前被排除点的邻接点的分类型链表（排除当前点会导致与邻接点的边也要被排除）
            for (auto iter = this->nodeList.at(excludeNodeTypeIDList[i])->getLinkedNodeList().begin(); iter != this->nodeList.at(excludeNodeTypeIDList[i])->getLinkedNodeList().end(); ++iter) {
                iter->first->flushLinkedNodes();
            }
        }
    }
}

void Graph::excludeEdges(const std::vector<std::pair<std::string, std::string>> &beginAndEndNodeTypeIDList) {
    for (auto iter = beginAndEndNodeTypeIDList.begin(); iter != beginAndEndNodeTypeIDList.end(); ++iter) {
        if (this->nodeList.contains(iter->first)) {
            this->nodeList.at(iter->first)->excludeEdge(iter->second);
        }
        if (this->nodeList.contains(iter->second)) {
            this->nodeList.at(iter->second)->excludeEdge(iter->first);
        }
    }
}

void Graph::includeNodes(const std::vector<std::string> &includeNodeTypeIDList) {
    for (auto i = 0; i < includeNodeTypeIDList.size(); ++i) {
        if (this->nodeList.contains(includeNodeTypeIDList[i])) {
            this->nodeList.at(includeNodeTypeIDList[i])->include();
        }
    }
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        iter->second->flushLinkedNodes();
    }
}

void Graph::includeEdges(const std::vector<std::pair<std::string, std::string>> &beginAndEndNodeTypeIDList) {
    for (auto iter = beginAndEndNodeTypeIDList.begin(); iter != beginAndEndNodeTypeIDList.end(); ++iter) {
        if (this->nodeList.contains(iter->first)) {
            this->nodeList.at(iter->first)->includeEdge(iter->second);
        }
        if (this->nodeList.contains(iter->second)) {
            this->nodeList.at(iter->second)->includeEdge(iter->first);
        }
    }
}

void Graph::flush() {
    LOG(INFO) << "刷新图！";

    /**
     * 初始化图中全部节点的类型链表
     */
    for (auto iter = this->nodeTypeCountList.begin(); iter != this->nodeTypeCountList.end(); ++iter) {
        this->typeNodeList[iter->first] = std::vector<Node *>();
        this->typeNodeList[iter->first].reserve(iter->second);
    }

    // 遍历图中全部节点
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        // 将当前节点接入对应的类型链表
        this->typeNodeList[iter->second->getType()].emplace_back(iter->second);

        // 刷新当前点
        // 生成每个点的分类型链表（包括去掉分类型链表、每个链表的随机数分布器）
        iter->second->flushLinkedNodes();

        // 遍历当前点的分类型链表
        for (auto subIter = iter->second->getLinkedNodeMapList().begin(); subIter != iter->second->getLinkedNodeMapList().end(); ++subIter) {
            // 建立当前节点的分类型度数列表
            this->nodeDegreeList[iter->first] = std::map<std::string, int>();
            // 存储当前节点的分类型度数
            this->nodeDegreeList[iter->first][subIter->first] = subIter->second.first.size();

            // 判断当前节点的类型在分类型最大度数列表中是否存在
            if (!this->nodeTypeMaxDegreeList.contains(iter->second->getType())) {
                // 不存在则建立
                this->nodeTypeMaxDegreeList[iter->second->getType()] = std::map<std::string, int>();
            }
            // 判断当前节点类型的分类型最大度数列表中对应类型的最大度数是否存在
            if (!this->nodeTypeMaxDegreeList[iter->second->getType()].contains(subIter->first)) {
                // 不存在则将当前节点类型的当前链接点类型的度数存入
                this->nodeTypeMaxDegreeList[iter->second->getType()][subIter->first] = subIter->second.first.size();
            }
            // 判断当前节点的当前链接点类型的度数是否大于当前节点类型分类型最大度数列表中对应的最大度数
            if (subIter->second.first.size() > this->nodeTypeMaxDegreeList[iter->second->getType()][subIter->first]) {
                // 若大于则更新
                this->nodeTypeMaxDegreeList[iter->second->getType()][subIter->first] = subIter->second.first.size();
            }
        }
    }


    /**
     * 图计算结果的初始化
     */

#ifndef ONLY_VISITED_NODE_RESULT
    if (this->resultType == "visited_count") {
        this->visitedNodeTypeIDCountList.clear();

        // 按照支持的最大线程数创建节点访问次数列表
        // 会在maxWalkBeginNodeCount个数的基础上多申请一个HashMap，用于多个HashMap合并时的存储
        for (auto i = 0; i <= maxWalkBeginNodeCount; ++i) {
            // 生成当前线程的节点访问HashMap
            std::unordered_map<std::string, unsigned int> countMap = std::unordered_map<std::string, unsigned int>();
            // 将图中全部未排除节点的访问次数置零
            for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
                if (iter->second->canVisit()) {
                    countMap[iter->first] = 0;
                }
            }
            // 将该记录有图中全部节点初始访问次数的HashMap放入列表
            this->visitedNodeTypeIDCountList.emplace_back(countMap);
        }
    } else {
        this->walkingSequence.clear();

        for (auto i = 0; i < maxWalkBeginNodeCount; ++i) {
            this->walkingSequence.emplace_back(std::vector<std::string>());
        }
    }
#else
for (auto i = 0; i < maxWalkBeginNodeCount; ++i) {
        std::unordered_map<std::string, unsigned int> countMap = std::unordered_map<std::string, unsigned int>();
        for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
            if (iter->second->canVisit()) {
                countMap[iter->first] = 0;
            }
        }
        this->visitedNodeTypeIDCountList.emplace_back(countMap);
    }
#endif
}

void Graph::traverse(std::vector<std::string> &traverseSequenceList, Node *const &beginNode, const WalkingDirection &direction,
                     const EdgeChooseStrategy &strategy, const std::string &endNodeTypeID) {
    //std::default_random_engine randomEngine(std::chrono::system_clock::now().time_since_epoch().count());

    //if (direction == WalkingDirection::DEEP) {
    //    std::stack<Node *const> depthStack;

    //    depthStack.push(beginNode);
    //    while (!depthStack.empty()) {
    //        Node *const n = depthStack.top();
    //        depthStack.pop();

    //        // 访问该点
    //        n->visit();
    //        // 将该点ID加入遍历去序列
    //        traverseSequenceList.push_back(n->getTypeID());

    //        // 若配置了遍历重点则判断是否到达终点
    //        // 到达则退出遍历，这是遍历ID序列的最后一个元素就是终点ID
    //        if (!endNodeTypeID.empty() && n->getTypeID() == endNodeTypeID) {
    //            break;
    //        }

    //        // 获得该点全部未访问链接节点
    //        const std::vector<Node*> &linkedNodeList = n->getNoVisitedLinkedNodeList();
    //        if (!linkedNodeList.empty()) {
    //            if (strategy == FIRST_NO_VISIT) {
    //                for (auto iter = linkedNodeList.crbegin(); iter != linkedNodeList.crend(); ++iter) {
    //                    depthStack.push(*iter);
    //                }
    //            } else if (strategy == LAST_NO_VISIT) {
    //                for (auto iter = linkedNodeList.cbegin(); iter != linkedNodeList.cend(); ++iter) {
    //                    depthStack.push(*iter);
    //                }
    //            } else {
    //                std::vector<Node*> shuffleLinkedNodeList = linkedNodeList;
    //                std::shuffle(shuffleLinkedNodeList.begin(), shuffleLinkedNodeList.end(), randomEngine);
    //                for (auto iter = shuffleLinkedNodeList.cbegin(); iter != shuffleLinkedNodeList.cend(); ++iter) {
    //                    depthStack.push(*iter);
    //                }
    //            }
    //        }
    //    }
    //} else {
    //    // 广度优先遍历

    //    // 初始化度列
    //    std::queue<Node *const> levelQueue;
    //    // 将当前节点放入队列
    //    levelQueue.push(beginNode);
    //    // 广度遍历全部节点
    //    while (!levelQueue.empty()) {
    //        // 从队列中取出当前节点
    //        Node *const n = levelQueue.front();
    //        levelQueue.pop();

    //        // 访问该点
    //        n->visit();
    //        // 将该点ID加入遍历去序列
    //        traverseSequenceList.push_back(n->getTypeID());

    //        // 若配置了遍历重点则判断是否到达终点
    //        // 到达则退出遍历，这是遍历ID序列的最后一个元素就是终点ID
    //        if (!endNodeTypeID.empty() && n->getTypeID() == endNodeTypeID) {
    //            break;
    //        }

    //        // 获得该点全部未访问链接节点
    //        const std::vector<Node *> &linkedNodeList = n->getNoVisitedLinkedNodeList();
    //        if (strategy == FIRST_NO_VISIT) {
    //            for (auto iter = linkedNodeList.cbegin(); iter != linkedNodeList.cend(); ++iter) {
    //                levelQueue.push(*iter);
    //            }
    //        } else if (strategy == LAST_NO_VISIT) {
    //            for (auto iter = linkedNodeList.crbegin(); iter != linkedNodeList.crend(); ++iter) {
    //                levelQueue.push(*iter);
    //            }
    //        } else {
    //            std::vector<Node*> shuffleLinkedNodeList = linkedNodeList;
    //            std::shuffle(shuffleLinkedNodeList.begin(), shuffleLinkedNodeList.end(), randomEngine);
    //            for (auto iter = shuffleLinkedNodeList.cbegin(); iter != shuffleLinkedNodeList.cend(); ++iter) {
    //                levelQueue.push(*iter);
    //            }
    //        }
    //    }
    //}
}

bool Graph::cmp(std::pair<std::string, int> a, std::pair<std::string, int> b) {
    return a.second > b.second;
}

std::string Graph::getTypeFromTypeID(const std::string tpeID) {
    std::vector<std::string> pair = Util::stringSplitToVector(tpeID, ":");
    return pair[0];
}

void Graph::clearResultList(const unsigned int &threadNum) {
#ifndef ONLY_VISITED_NODE_RESULT
    if (this->resultType == "visited_count") {
        for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
            this->visitedNodeTypeIDCountList[threadNum][iter->first] = 0;
        }
    } else {
        this->walkingSequence.at(threadNum).clear();
    }
#else
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
        this->visitedNodeTypeIDCountList[threadNum][iter->first] = 0;
    }
#endif
}

void Graph::insertResultList(Node* &node, const unsigned int &threadNum) {
#ifndef ONLY_VISITED_NODE_RESULT
    if (this->resultType == "visited_count") {
        this->visitedNodeTypeIDCountList.at(threadNum)[node->getTypeID()] = node->getVisitedCount();
    } else {
        this->walkingSequence.at(threadNum).emplace_back(node->getTypeID());
    }
#else
    this->visitedNodeTypeIDCountList.at(threadNum)[node->getTypeID()] = node->getVisitedCount();
#endif
}

void Graph::mergeResultList(const std::vector<unsigned int> &threadNumList, const unsigned int &targetThreadNum) {
    // 定义最大访问次数
    int maxCount;
    // 遍历图中全部节点
    for (auto iter = this->nodeList.begin(); iter != this->nodeList.end(); ++iter) {
#ifdef STORAGE_SAVING_MODE
        // 判断当前节点是否可访问
        if (!iter->second->canVisit()) {
            continue;
        }
#endif

        // 初始化当前点的最大访问次数为0
        maxCount = this->visitedNodeTypeIDCountList[targetThreadNum].at(iter->first);
        // 遍历多路图操作对应的线程编号
        for (auto i = 0; i < threadNumList.size(); ++i) {
            // 比较当前点在当前路图操作中的访问次数是否最大
            if (this->visitedNodeTypeIDCountList[threadNumList[i]].at(iter->first) > maxCount) {
                maxCount = this->visitedNodeTypeIDCountList[threadNumList[i]].at(iter->first);
            }
        }
        // 记录当前点的最大访问次数
        this->visitedNodeTypeIDCountList[targetThreadNum].at(iter->first) = maxCount;
    }
}
