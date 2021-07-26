
// Created by 翟霄 on 2021/7/18.
//

#ifndef GRAPHCOMPUTE_UTIL_H
#define GRAPHCOMPUTE_UTIL_H

#include <vector>
#include <string>

// 日志库
#include <glog/logging.h>

class Util {
public:
    /**
     * 根据指定分隔符将string切分为多个string组成的vector
     * @param str   待切分的string
     * @param split 切分后得到子string的vector
     * @return
     */
    static std::vector<std::string> stringSplitToVector(const std::string &str, const std::string &delimiter);
};


#endif //GRAPHCOMPUTE_UTIL_H
