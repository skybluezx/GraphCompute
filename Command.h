//
// Created by 翟霄 on 2021/8/3.
//

#ifndef GRAPHCOMPUTE_COMMAND_H
#define GRAPHCOMPUTE_COMMAND_H

#include <string>

#include "Graph.h"

#include "question_recall_define.h"

struct In;
struct Out;

class Command {
public:
    static void execute(Graph &graph, const std::string &command, const std::string &resultDirectoryPath);

    static Out questionRecall(In &request);
private:

};


#endif //GRAPHCOMPUTE_COMMAND_H
