#-*- coding:utf-8 -*-
from __future__ import division
import os
import json
import pandas as pd
import argparse

parser = argparse.ArgumentParser(description='Simple file download script for Google Drive')
parser.add_argument(
    '-file_path',
    dest='file_path',
    type=str,
    help='kp_question_recall_result_directory',
)
args = parser.parse_args()


# 题目单选标签：
question_is_multiple_choice_path = "../test_data/course_question.csv"
question_is_multiple_choice = pd.read_csv(question_is_multiple_choice_path)
question_id_is_multiple_choice_mapping = question_is_multiple_choice[["question_id", "is_multiple_choice"]].drop_duplicates().set_index("question_id")["is_multiple_choice"].to_dict()
def kp_recall(kp_question_recall_result_directory, test_kp_courseware_question_file_path, recall_top_n, is_multiple_choice=True):
    # 读取全部知识点召回
    kp_question_recall_result_list = dict()
    # 遍历全部知识点的题目召回结果
    for file in os.listdir(kp_question_recall_result_directory):
        # 检查文件正确性
        if 'result' in file:
            if len(file.split('_')) == 2:
                if len(file.split('_')[0].split(':')) == 2:
                    kp_question_recall_result_list[file.split('_')[0].split(':')[1]] = list()
                    with open(kp_question_recall_result_directory + '/' + file, 'r') as f:
                        count = 0
                        for line in f:
                            if count >= recall_top_n:
                                break
                            if len(line.split(':')) == 3:
                                result_node_id = line.split(':')[1]
                                kp_question_recall_result_list[file.split('_')[0].split(':')[1]].append(result_node_id)
                            count += 1
    # print(kp_question_recall_result_list)

    # 初始化每个课件包含的知识点列表
    test_courseware_kp_list = dict()
    # 初始化每个课件包含的题目列表
    test_courseware_question_list = dict()
    # 读取测试集知识点-课件-题目的映射文件
    with open(test_kp_courseware_question_file_path, 'r') as f:
        for line in f:
            json_object = json.loads(line)

            # 增加当前课件包含的知识点
            if json_object['courseware_id'] not in test_courseware_kp_list:
                test_courseware_kp_list[json_object['courseware_id']] = list()
            test_courseware_kp_list[json_object['courseware_id']].append(json_object['kp_id'])

            # 增加当前课件包含的题目
            if json_object['courseware_id'] not in test_courseware_question_list:
                if is_multiple_choice:
                    question_list_ =  [i for i in json_object['question'] if question_id_is_multiple_choice_mapping[i] == 1]
                else:
                    question_list_ = json_object['question']
                test_courseware_question_list[json_object['courseware_id']] = question_list_
    # print(test_courseware_kp_list.keys())
    # print(test_courseware_question_list)

    # 初始化每个课件中召回成功的题目个数列表
    courseware_recall_question_count_list = dict()
    for courseware_id in test_courseware_kp_list:
        # 初始化当前课件的题目召回成功数为0
        courseware_recall_question_count_list[courseware_id] = 0
        # 获取当前课件包含的知识点列表
        kp_list = test_courseware_kp_list[courseware_id]
        # 初始化当前课件包含知识点对应的全部召回题目列表
        all_type_result_node_id_list = list()
        # 遍历当前课中中的每一个知识点
        for kp in kp_list:
            # 将当前知识点对应的题目放入当前课件全部召回题目的列表
            all_type_result_node_id_list.extend(kp_question_recall_result_list[kp])

        # 遍历当前课件包含的全部题目
        for question in test_courseware_question_list[courseware_id]:
            # 判断当前题目是否在当前课件召回的全部题目中
            if question in all_type_result_node_id_list:
                # 如果在，则当前课件的召回总数加1
                courseware_recall_question_count_list[courseware_id] += 1
        # print(len(courseware_recall_question_count_list))

    # 计算总召回数和总召回率
    # 初始化总召回数为0
    recall_sum = 0
    # 初始化测试集中总题目数为0
    positive_sum = 0
    # 遍历每个课件的召回成功数
    for courseware_id in test_courseware_kp_list:
        recall_sum += courseware_recall_question_count_list[courseware_id]
        positive_sum += len(test_courseware_question_list[courseware_id])

    # 输出召回指标
    #print('recall_top_n: ', recall_top_n, 'Recall Count: ', recall_sum, 'Positive Count: ', positive_sum, 'Recall Ratio: ', recall_sum / positive_sum)
    print recall_top_n, '\t', recall_sum, '\t', positive_sum, '\t', recall_sum / positive_sum


if __name__ == '__main__':
    # 知识点推题的题目列表文件目录路径
    # 每个文件为一个知识点的推题结果
    kp_question_recall_result_directory = args.file_path
    # 测试集中知识点-课件-题目的映射文件路径
    test_kp_courseware_question_file_path = '../test_data/kp_courseware_question.json'

    # 设置召回题目的TopN
    recall_top_n_list = [100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
    for n in recall_top_n_list:
        kp_recall(kp_question_recall_result_directory, test_kp_courseware_question_file_path, n, is_multiple_choice=False)

