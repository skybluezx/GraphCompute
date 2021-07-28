#include <iostream>

#include "Util.h"
#include "type_defination.h"
#include "Graph.h"

int main(int argc, char* argv[]) {
//    google::InitGoogleLogging(argv[0]);
//    FLAGS_log_dir = "../log";
//    LOG(INFO) << "hello world";

    // 传入图定义文件建立图
    Graph graph = Graph("/Users/zhaixiao/graph_define");

    /**
     * 样例
     */

    // 深度/广度优先遍历
//    auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
//    for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
//        std::cout << *i << std::endl;
//    }

    //随机游走
    std::vector<std::string> stepDefine = {"a", "b"};
    std::map<std::string, std::string> auxiliaryStep;
    auxiliaryStep["b"] = "c";

    auto walkingSequence = graph.walk("a1", stepDefine, auxiliaryStep, 1.0, 10, EdgeChooseStrategy::RANDOM, true);

//    std::cout << graph.nodeList.begin()->second->getNextLinkedNode(edgeChooseStrategy::FIRST)->id;
//    graph.reset();
//    graph.setBeginNode("a1");
//    graph.walkingDirection = Graph::DEEP;
//    graph.edgeChooseStrategy = Graph::LAST;
//    graph.walkingStep = 10;
//
//    GraphIterator graphIterator = graph.begin();
//
//    std::cout << graphIterator->getLinkInfo() << std::endl;

//    graph.walk();

//    std::cout << graph.nodeList.size() << std::endl;
//    for (const auto &node : graph.nodeList) {
//        std::cout << node.second->getLinkInfo() << std::endl;
//    }

    return 0;
}