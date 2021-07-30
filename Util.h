
// Created by 翟霄 on 2021/7/18.
//

#ifndef GRAPHCOMPUTE_UTIL_H
#define GRAPHCOMPUTE_UTIL_H

#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// 日志库
#include <glog/logging.h>
// 命令行解析库
#include <gflags/gflags.h>

class Util {
public:
    // 配置文件路径
    static std::string configFilePath;

    template<class T>
    static bool getConfig(const std::string &typeName, const std::string &itemName, T &itemValue) {
        if (Util::configFilePath.empty()) {
            LOG(ERROR) << "未设置配置文件路径！";
            return false;
        }

        if (!boost::filesystem::exists(configFilePath)) {
            LOG(ERROR) << "配置文件不存在！";
            return false;
        }

        boost::property_tree::ptree root_node, tag_system;
        boost::property_tree::ini_parser::read_ini(configFilePath, root_node);

        if (root_node.count(typeName) != 1) {
            LOG(ERROR) << "配置类型不存在！";
            return false;
        }
        tag_system = root_node.get_child(typeName);

        if (tag_system.count(itemName) != 1) {
            LOG(ERROR) << "配置项不存在！";
            return false;
        }

        itemValue = tag_system.get<T>(itemName);
        return true;
    };

    /**
     * 根据指定分隔符将string切分为多个string组成的vector
     * @param str   待切分的string
     * @param split 切分后得到子string的vector
     * @return
     */
    static std::vector<std::string> stringSplitToVector(const std::string &str, const std::string &delimiter);

    static bool validatePath(const char* flagname, const std::string& value);
};


#endif //GRAPHCOMPUTE_UTIL_H
