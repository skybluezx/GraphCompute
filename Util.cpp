//
// Created by 翟霄 on 2021/7/18.
//

#include "Util.h"

#include <vector>
#include <boost/algorithm/string.hpp>
//#include <ranges>

std::vector<std::string> Util::stringSplitToVector(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> result;
    boost::split(result, str, boost::is_any_of(delimiter));
    return result;
}