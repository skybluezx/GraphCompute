//
// Created by 翟霄 on 2021/7/30.
//
#include <iostream>
#include <string>
#include <sstream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/filesystem.hpp>

#include "Util.h"

DEFINE_string(json, "", "JSON文件路径（执行遍历或游走的参数JSON文件路径）");
DEFINE_string(action, "", "动作名称（退出：EXIT 重置图状态：RESET）");

int main(int argc, char* argv[]) {
    // 解析命令行参数
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // 初始化日志模块
    google::InitGoogleLogging(argv[0]);

    // 创建共享变量
    boost::interprocess::managed_shared_memory managed_shm(
            boost::interprocess::open_or_create,
            "shm",
            1024 * 1024 * 5);
    typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> string;
    string* command = managed_shm.find<string>("Command").first;
    // 创建互斥量
    boost::interprocess::named_mutex named_mtx(boost::interprocess::open_or_create, "mtx");
    // 创建条件变量
    boost::interprocess::named_condition named_cnd(boost::interprocess::open_or_create, "cnd");
    // 锁定互斥量
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
        } else if (FLAGS_action == "RESET") {
            // 执行服务端图重置命令
            std::cout << "[INFO] 重置GraphCompute服务端存储的图状态！" << std::endl;
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
        named_cnd.notify_all();
        named_cnd.wait(lock);
    } else {
        // 其他情况显示说明
        std::cout << "[INFO] GraphCompute客户端" << std::endl;
        std::cout << "[INFO] 具体参数请参考文档！" << std::endl;
    }

    return 0;
}