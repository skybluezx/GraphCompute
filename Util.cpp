//
// Created by 翟霄 on 2021/7/18.
//

#include "Util.h"

#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

std::string Util::configFilePath = "";

//template <class T>
bool Util::getConfig(const std::string &typeName, const std::string &itemName, std::string &itemValue) {
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

    itemValue = tag_system.get<std::string>(itemName);
    return true;
}

std::vector<std::string> Util::stringSplitToVector(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> result;
    boost::split(result, str, boost::is_any_of(delimiter));
    return result;
}

bool Util::validatePath(const char* flagname, const std::string& value) {
    if (value.empty()) {
        std::cerr << "配置文件路径为空！" << std::endl;
        return false;
    }
    return true;
}