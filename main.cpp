#include <iostream>

#include "Util.h"
#include "type_defination.h"
#include "Graph.h"

/**
 * 定义命令行参数
 */
// 配置文件路径
DEFINE_string(config_file_path, "", "配置文件路径 ");
DEFINE_validator(config_file_path, &Util::validatePath);

int main(int argc, char* argv[]) {
    /**
     * 启动配置部分
     */

    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // 初始化日志模块
//    google::InitGoogleLogging(argv[0]);

    // 设置配置文件路径
    Util::configFilePath = FLAGS_config_file_path;

    // 配置日志输出路径
    std::string logDirectory;
    Util::getConfig("Path", "log_directory", logDirectory);
//    FLAGS_log_dir = logDirectory;

    /**
     * 主逻辑部分
     */

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);

    // 建立图
    Graph graph = Graph(graphDefineDirectory);

//    auto nodeTypeList = graph.getNodeTypeList();
//    for (int i = 0; i < nodeTypeList.size(); ++i) {
//        std::cout << nodeTypeList[i] << std::endl;
//    }

    /**
     * 样例
     */

    // 深度/广度优先遍历
//    auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
//    for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
//        std::cout << *i << std::endl;
//    }

    //随机游走
    std::vector<std::string> stepDefine = {"KnowledgePoint", "Question"};
    std::map<std::string, std::string> auxiliaryStep;
//    auxiliaryStep["Question"] = "Courseware";

    auto walkingSequence = graph.walk("f9e12aee12214301b757841df388be97", stepDefine, auxiliaryStep, 1.0, 100000, EdgeChooseStrategy::RANDOM, true);

    std::vector<std::pair<std::string, int>> result = graph.getSortedNodeIDTypeListByVisitedCount(walkingSequence);
    for (auto i = 0; i < 100; ++i) {
        std::cout << result[i].first << ": " << result[i].second << std::endl;
    }

    return 0;
}