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
    google::InitGoogleLogging(argv[0]);

    // 设置配置文件路径
    Util::configFilePath = FLAGS_config_file_path;

    // 配置日志输出路径
    std::string logDirectory;
    Util::getConfig("Path", "log_directory", logDirectory);
    FLAGS_log_dir = logDirectory;

    /**
     * 主逻辑部分
     */

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);

    // 建立图
    Graph graph = Graph(graphDefineDirectory, 10);
    // 输出图中点的全部类型
//    auto nodeTypeList = graph.getNodeTypeList();
//    for (int i = 0; i < nodeTypeList.size(); ++i) {
//        std::cout << nodeTypeList[i] << std::endl;
//    }

    /**
     * 图操作
     */

    /**
     * 遍历
     * 深度/广度优先遍历
     */
//    auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
//    for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
//        std::cout << *i << std::endl;
//    }

    /**
     * 游走
     * 随机游走
     */
    // 定义步的边组成
    std::vector<std::string> stepDefine = {"KnowledgePoint", "Question"};
    // 定义辅助边
    std::map<std::string, std::string> auxiliaryEdge;
    auxiliaryEdge["Question"] = "Courseware";
    // 游走
    auto walkingSequence = graph.walk("f9e12aee12214301b757841df388be97",
                                      stepDefine,
                                      auxiliaryEdge,
                                      1.0,
                                      100000,
                                      EdgeChooseStrategy::RANDOM,
                                      true);
    // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
    std::vector<std::pair<std::string, int>> result = graph.getSortedNodeIDListByVisitedCount(walkingSequence, "Question");
    int topSize = 100; if (topSize > result.size()) topSize = result.size();
    for (auto i = 0; i < topSize; ++i) {
        std::cout << result[i].first << ": " << result[i].second << std::endl;
    }

    return 0;
}