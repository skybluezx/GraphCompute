//
// Created by 翟霄 on 2021/8/16.
//

#ifndef GRAPHCOMPUTE_QUESTION_RECALL_DEFINE_H
#define GRAPHCOMPUTE_QUESTION_RECALL_DEFINE_H

#include <map>
#include <string>

/**
 * 题目召回项目中的定义
 */
namespace arch {
    //题目召回接口的输入类型
    struct In {
        int expected;                                                   // 召回题目数（最终题目数相等于该值）
        std::map<std::string, int32_t> questions_assement;              // 本节课全部题目ID及题目作答结果
        std::map<std::string, double> current_knowledge_points;         // 本节课全部知识点及测评结果
        std::map<std::string, int32_t> preceding_questions_assement;    // 同一学生前序课堂全部作答题目的ID及作答结果
        std::map<std::string, double> knowledge_points;                 // 同一学生前序课堂全部知识点及测评结果
    };

    // 题目召回接口的返回类型
    struct Out {
        int32_t code ;                      // 召回状态（用于召回逻辑是否成功以及各种预定义的状态）
        std::vector<std::string> payload;   // 召回的题目ID列表
    };
}

#endif //GRAPHCOMPUTE_QUESTION_RECALL_DEFINE_H
