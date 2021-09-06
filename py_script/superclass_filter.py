import json
import os


kp_pre_total_dict = dict()


def filtrate(question_dict, knowledge_list):
    """
    防超纲题目筛选
    :param question_dict: 题目-主知识点字典
    :param knowledge_list: 课件-知识点列表
    :return: 符合条件的题目列表
    """
    # 获取知识点的前置与同级列表
    knowledge_pre = []
    for i in knowledge_list:
        # 若知识点无前置知识点，把此知识点放入同级
        if i in kp_pre_total_dict.keys():
            knowledge_pre += kp_pre_total_dict[i]
        else:
            knowledge_pre += [i]
    filtrate_result = []
    # 获取题目的主知识点的前置列表，若为知识点子集选择该题目
    for q in question_dict.keys():
        question_pre = []
        for k in question_dict[q]:
            if k in kp_pre_total_dict.keys():
                question_pre += kp_pre_total_dict[k]
            else:
                question_pre += [k]
        if set(knowledge_pre) >= set(question_pre):
            filtrate_result.append(q)
    return filtrate_result


if __name__ == "__main__":
    ################
    #    预处理     #
    ################

    # 加载防超纲前置知识点字典
    with open('/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/kp_pre_total.txt', 'r') as f:
        kp_pre_total_dict = json.loads(f.read())

    # # 获取题目字典
    # question_kp_dict = dict()
    # # 读取知识点-题目Graph定义文件
    # question_kp_dict_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/graph_define/kp_is_main_all_question_courseware_remove/tripartite_data.csv'
    # with open(question_kp_dict_file_path, 'r') as f:
    #     # 遍历Graph定义文件中的每一行
    #     for line in f:
    #         pair = line.split('\t')
    #         if len(pair) == 2:
    #             begin = pair[0].split(':')
    #             end = pair[1].split(':')
    #             if len(begin) == 2 and len(end) == 2:
    #                 # 判断是否为记录知识点和题目的行
    #                 if 'KnowledgePoint' == begin[0].strip() and 'Question' == end[0].strip():
    #                     question_kp_dict[end[1].strip()] = begin[1].strip()
    #                 if 'KnowledgePoint' == end[0].strip() and 'Question' == begin[0].strip():
    #                     question_kp_dict[begin[1].strip()] = end[1].strip()

    # # 存储题目字典
    question_kp_dict_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/question_dict.csv'
    # # with open(question_kp_dict_file_path, 'w') as f:
    # #     for key in question_kp_dict.keys():
    # #         f.write(key + ',' + question_kp_dict[key] + '\n')

    # 读取题目字典
    question_kp_dict = dict()
    with open(question_kp_dict_file_path, 'r') as f:
        for line in f:
            question_kp_dict[line.split(',')[0].strip()] = line.split(',')[1].strip()

    # 遍历全部召回的题目
    recall_result_dir = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/question_recall_test'
    kp_recall_dir = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/question_kp_recall_test'
    recall_file_list = os.listdir(recall_result_dir)
    for recall_file_name in recall_file_list:
        # 读取当前课堂召回的全部题目并拼接该题的知识点
        question_list = dict()
        recall_file_path = os.path.join(recall_result_dir, recall_file_name)
        with open(recall_file_path, 'r') as f:
            for question in f:
                question_id = question.split(':')[1].strip()
                question_list[question_id] = question_kp_dict[question_id]

        # 输出当前课堂的题目及知识点列表
        with open(os.path.join(kp_recall_dir, recall_file_name), 'w') as f:
            for key in question_list.keys():
                f.write(key + ',' + question_list[key] + '\n')

    ################
    # 执行防超纲策略  #
    ################

    # 遍历全部课堂的召回结果
    recall_task_dir = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/task/question_recall_test'
    recall_task_file_list = os.listdir(recall_task_dir)
    for recall_task_file in recall_task_file_list:
        # 读取当前课堂的知识点列表
        course_kp_list = list()
        with open(os.path.join(recall_task_dir, recall_task_file), 'r') as f:
            recall_task_json = json.load(f)
            for kp_pair in recall_task_json['beginNodeID'][0]:
                course_kp_list.append(kp_pair['id'])

        # 读入当前课堂的题目-知识点列表
        question_list = dict()
        kp_recall_file_name = os.path.join(kp_recall_dir, recall_task_file + '_result.dat')
        with open(kp_recall_file_name, 'r') as f:
            for line in f:
                if len(line.split(',')) == 2:
                    question_list[line.split(',')[0].strip()] = list()
                    question_list[line.split(',')[0].strip()].append(line.split(',')[1].strip())

        print('召回题目数：', len(question_list))
        filtrate_result = filtrate(question_list, course_kp_list)
        print('防超纲过滤后题目数：', len(filtrate_result))

        # 输出当前课堂防超纲过滤后的题目列表
        final_result_file_path = os.path.join('/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/question_final_recall_test', recall_task_file)
        with open(final_result_file_path, 'w') as f:
            for question in filtrate_result:
                f.write(question + '\n')
