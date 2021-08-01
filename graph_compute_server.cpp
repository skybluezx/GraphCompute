#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

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

    boost::interprocess::shared_memory_object::remove("shm");
    boost::interprocess::named_mutex::remove("mtx");
    boost::interprocess::named_condition::remove("cnd");

    boost::interprocess::managed_shared_memory managed_shm(
            boost::interprocess::open_or_create,
            "shm",
            1024 * 1024 * 5);
    std::string *commandString = managed_shm.find_or_construct<std::string>("Command")("");
    boost::interprocess::named_mutex named_mtx(boost::interprocess::open_or_create, "mtx");
    boost::interprocess::named_condition named_cnd(boost::interprocess::open_or_create, "cnd");
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx);

    /**
     * 主逻辑部分
     */

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);
    // 建立图
    Graph graph = Graph(graphDefineDirectory, 10);
    // 输出图的概要
    auto nodeTypeCountList = graph.getNodeTypeCountList();
    for (auto iter = nodeTypeCountList.begin(); iter != nodeTypeCountList.end(); ++iter) {
        LOG(INFO) << iter->first << ":" << iter->second;
    }
    google::FlushLogFiles(google::INFO);

    while (true) {
        try {
            if (commandString->empty()) {
                // 当前没有命令传入时阻塞等待
                LOG(INFO) << "等待任务中...";
                google::FlushLogFiles(google::INFO);
                named_cnd.notify_all();
                named_cnd.wait(lock);
            } else if (!commandString->empty() && *commandString != "EXIT") {
                // 收到命令时开始执行
                LOG(INFO) << "收到任务开始执行！";
                google::FlushLogFiles(google::INFO);

                // 执行结束将命令置空阻塞等待
                *commandString = "";
                named_cnd.notify_all();
                named_cnd.wait(lock);
            } else if (*commandString == "EXIT") {
                // 获得退出命令后退出
                LOG(INFO) << "结束服务！";
                break;
            }
        } catch (std::exception &e) {

        }
    }

    // 服务结束通知其他进程善后
    named_cnd.notify_all();
    // 删除全部共享变量、互斥量和条件变量
    boost::interprocess::shared_memory_object::remove("shm");
    boost::interprocess::named_mutex::remove("mtx");
    boost::interprocess::named_condition::remove("cnd");


//        /**
//         * 图操作
//         */
//
//        /**
//         * 遍历
//         * 深度/广度优先遍历
//         */
////    auto traverseResult = graph.traverse("a1", WalkingDirection::WIDE, EdgeChooseStrategy::RANDOM_NO_VISIT);
////    for (auto i = traverseResult.begin(); i != traverseResult.end(); ++i) {
////        std::cout << *i << std::endl;
////    }
//
//        /**
//         * 游走
//         * 随机游走
//         */
//        // 定义步的边组成
//        std::vector<std::string> stepDefine = {"KnowledgePoint", "Question"};
//        // 定义辅助边
//        std::map<std::string, std::string> auxiliaryEdge;
//        auxiliaryEdge["Question"] = "Courseware";
//        // 游走
//        auto walkingSequence = graph.walk("f9e12aee12214301b757841df388be97",
//                                          stepDefine,
//                                          auxiliaryEdge,
//                                          1.0,
//                                          100000,
//                                          EdgeChooseStrategy::RANDOM,
//                                          true);
//        // 输出指定类型节点按访问次数排序节点ID、类型以及具体访问次数
//        std::vector<std::pair<std::string, int>> result = graph.getSortedNodeIDListByVisitedCount(walkingSequence,
//                                                                                                  "Question");
//        int topSize = 100;
//        if (topSize > result.size()) topSize = result.size();
//        for (auto i = 0; i < topSize; ++i) {
//            std::cout << result[i].first << ": " << result[i].second << std::endl;
//        }
//    }

    return 0;
}