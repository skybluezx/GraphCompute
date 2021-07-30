//
// Created by 翟霄 on 2021/7/18.
//

#include "Util.h"

#include <iostream>

std::string Util::configFilePath = "";

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