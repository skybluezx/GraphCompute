#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "Util.h"
#include "Graph.h"
#include "Command.h"

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

    // 读取计算结果输出路径
    std::string resultDirectoryPath;
    Util::getConfig("Path", "result_directory", resultDirectoryPath);

    boost::interprocess::shared_memory_object::remove("shm");
    boost::interprocess::named_mutex::remove("mtx");
    boost::interprocess::named_condition::remove("cnd");

    boost::interprocess::managed_shared_memory managed_shm(
            boost::interprocess::open_or_create,
            "shm",
            1024 * 1024 * 5);
    typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> string;
    string *commandString = managed_shm.find_or_construct<string>("Command")("", managed_shm.get_segment_manager());
    boost::interprocess::named_mutex named_mtx(boost::interprocess::open_or_create, "mtx");
    boost::interprocess::named_condition named_cnd(boost::interprocess::open_or_create, "cnd");
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx);

    /**
     * 主逻辑部分
     */

    // 设置图定义文件
    std::string graphDefineDirectory;
    Util::getConfig("Path", "graph_define_directory", graphDefineDirectory);
    int readEdgeCount;
    Util::getConfig("Input", "read_edge_count", readEdgeCount);
    // 建立图
    Graph graph = Graph(graphDefineDirectory, readEdgeCount);
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
                if (*commandString == "RESET") {
                    graph.reset();
                    LOG(INFO) << "重置图中节点状态！";
                } else {
                    // 执行命令
                    Command::execute(graph, commandString->c_str(), resultDirectoryPath);
                }

                // 写入全部日志
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

    return 0;
}