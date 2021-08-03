//
// Created by 翟霄 on 2021/8/3.
//

#ifndef GRAPHCOMPUTE_COMMAND_H
#define GRAPHCOMPUTE_COMMAND_H

#include <string>

#include "Graph.h"

class Command {
public:
    static void execute(Graph &graph, const std::string &command, const std::string &resultDirectoryPath);
};


#endif //GRAPHCOMPUTE_COMMAND_H
