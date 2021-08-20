//
// Created by 翟霄 on 2021/7/30.
//
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <chrono>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/filesystem.hpp>

#include "Util.h"

DEFINE_string(server_name, "", "服务端名称");
DEFINE_string(json, "", "JSON文件路径（执行遍历或游走的参数JSON文件路径）");
DEFINE_string(action, "", "动作名称（退出：EXIT 重置图状态：RESET）");

int main(int argc, char* argv[]) {
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // 初始化日志模块
    google::InitGoogleLogging(argv[0]);

    std::string serverName = FLAGS_server_name;
    std::string sharedMemoryObjectName = serverName + "shm";
    std::string namedMutexName = serverName + "mtx";
    std::string namedConditionName = serverName + "cnd";

    // 创建共享变量、互斥量和条件变量
    boost::interprocess::managed_shared_memory managed_shm(
            boost::interprocess::open_or_create,
            sharedMemoryObjectName.c_str(),
            1024 * 1024 * 5);
    typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> string;
    string* command = managed_shm.find<string>("Command").first;
    boost::interprocess::named_mutex named_mtx(boost::interprocess::open_or_create, namedMutexName.c_str());
    boost::interprocess::named_condition named_cnd(boost::interprocess::open_or_create, namedConditionName.c_str());
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx);

    // action参数优先判断
    if (FLAGS_action != "") {
        // 设置action参数则进一步判断action类型
        if (FLAGS_action == "EXIT") {
            // 执行服务端退出命令
            std::cout << "[INFO] 停止GraphCompute服务端！" << std::endl;
            *command = "EXIT";
            named_cnd.notify_all();
            named_cnd.wait(lock);
        } else if (FLAGS_action == "RESET_ONLY_VISITED_COUNT") {
            // 执行服务端图重置命令
            std::cout << "[INFO] 重置GraphCompute服务端存储的图状态！（仅重置图中节点的访问次数）" << std::endl;
            *command = "RESET_ONLY_VISITED_COUNT";
            named_cnd.notify_all();
            named_cnd.wait(lock);
        } else if (FLAGS_action == "RESET") {
            // 执行服务端图重置命令
            std::cout << "[INFO] 重置GraphCompute服务端存储的图状态！（恢复初始状态，包括节点访问次数和已排除的点与边）" << std::endl;
            *command = "RESET";
            named_cnd.notify_all();
            named_cnd.wait(lock);
        }
    } else if (FLAGS_json != "") {
        // 设置json参数则将json文件内容存入共享变量
        std::cout << "[INFO] 执行指定图计算！" << std::endl;
        if (!boost::filesystem::exists(FLAGS_json)) {
            std::cerr << "[ERROR] JSON文件不存在！" << std::endl;
        }
        // 读取json文件
        std::ifstream jsonFile(FLAGS_json);
        std::stringstream buffer;
        buffer << jsonFile.rdbuf();
        std::string jsonString(buffer.str());
        // 将读取内容存入共享内存
        command->insert(0, jsonString.c_str());

        // 唤醒服务端进程并阻塞客户端等待响应
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        named_cnd.notify_all();
        named_cnd.wait(lock);
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        std::chrono::duration<double> programSpan = duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "[INFO] 服务端执行时间：" << programSpan.count() << "秒" << std::endl;
    } else {
        // 其他情况显示说明
        std::cout << "[INFO] GraphCompute客户端" << std::endl;
        std::cout << "[INFO] 具体参数请参考文档！" << std::endl;
    }

    return 0;
}