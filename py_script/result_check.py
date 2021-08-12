import os
import json
from itertools import combinations
from tqdm import tqdm


def main():
    recommend_result_directory = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/result/main'
    # recommend_result_directory = '/Users/zhaixiao/workplace/c_cpp/temp/GraphCompute/result'
    test_courseware_id_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/test_data/test_courseware_id.dat'
    test_type_node_id_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/test_data/kp_courseware_question.json'

    # 读取全部知识点召回结果
    recommend_result_list = dict()
    for file in os.listdir(recommend_result_directory):
        if 'result' in file:
            if len(file.split('_')) == 2:
                if len(file.split('_')[0].split(':')) == 2:
                    recommend_result_list[file.split('_')[0].split(':')[1]] = list()
                    with open(recommend_result_directory + '/' + file, 'r') as f:
                        count = 0
                        for line in f:
                            if count >= 100:
                                break
                            if len(line.split(':')) == 3:
                                result_node_id = line.split(':')[1]
                                recommend_result_list[file.split('_')[0].split(':')[1]].append(result_node_id)
                            count += 1
    # print(recommend_result_list)

    # test_courseware_id_list = list()
    # with open(test_courseware_id_file_path, 'r') as f:
    #     for line in f:
    #         for courseware_id in line.split(','):
    #             test_courseware_id_list.append(courseware_id)
    # # print(test_courseware_id_list)

    # 读取测试集中全部课件对应的主知识点列表和题目列表
    temp = dict()
    test_type_list = list()
    test_courseware_type_list = dict()
    test_courseware_node_id_list = dict()
    with open(test_type_node_id_file_path, 'r') as f:
        for line in f:
            test_type_node = json.loads(line)

            # if test_type_node['courseware_id'] not in test_courseware_id_list:
            #     continue

            if test_type_node['kp_id'] not in test_type_list:
                test_type_list.append(test_type_node['kp_id'])
                temp[test_type_node['kp_id']] = list()

            temp[test_type_node['kp_id']].append(test_type_node['courseware_id'])

            if test_type_node['courseware_id'] not in test_courseware_type_list:
                test_courseware_type_list[test_type_node['courseware_id']] = list()
            test_courseware_type_list[test_type_node['courseware_id']].append(test_type_node['kp_id'])

            if test_type_node['courseware_id'] not in test_courseware_node_id_list:
                test_courseware_node_id_list[test_type_node['courseware_id']] = test_type_node['question']
    # print(test_type_list)
    # print(test_courseware_type_list)
    # print(test_courseware_node_id_list)
    # print(temp)

    kp_list = list()
    for item in test_courseware_type_list.items():
        for kp in item[1]:
            if kp not in kp_list:
                kp_list.append(kp)
    # print(kp_list)

    con_num = 4
    kp_con = list(combinations(kp_list, con_num))
    # print(kp_con)
    print(123123)
    for kp_con_item in tqdm(kp_con):
        flag = True
        result = list()
        # print(kp_con_item)
        for kp in kp_con_item:
            # print(kp)
            result.append(temp[kp])
        # print(result)
        result_set = set(result[0]).intersection(*result[1:])
        # print(result_set)
        # break
        if len(result_set) > 0:
            print(kp_con_item)


    # temp_dict = dict()
    # with open('/Users/zhaixiao/workplace/c_cpp/GraphCompute/test_data/kp_id_name_mapping.txt', 'r') as f:
    #     for line in f:
    #         item_list = line.split(':')
    #         temp_dict[item_list[0]] = item_list[1].strip()
    # print(temp_dict)

    # new_temp = dict()
    # for item in temp.items():
    #     new_temp[temp_dict[item[0]]] = len(item[1])
    #
    # for ttt in new_temp.items():
    #     print(ttt[0], ttt[1])
    # print(new_temp)


    # sum = 0
    # for item in test_courseware_type_list.items():
    #     sum += len(item[1])
    # print(sum / len(test_courseware_type_list))

    # for item in new_temp.items():
    #     print(item[0], len(item[1]))

    courseware_recall_node_count_list = dict()
    for courseware_id in test_courseware_type_list:
        courseware_recall_node_count_list[courseware_id] = 0
        type_list = test_courseware_type_list[courseware_id]

        all_type_result_node_id_list = list()
        for type in type_list:
            all_type_result_node_id_list.extend(recommend_result_list[type])

        for node_id in test_courseware_node_id_list[courseware_id]:
            if node_id in all_type_result_node_id_list:
                courseware_recall_node_count_list[courseware_id] += 1

        # print(len(type_list))
        # print(len(all_type_result_node_id_list))

    # print(courseware_recall_node_count_list['A92959AD33434162A8545E8C7FCC5616'])
    # print(len(test_courseware_node_id_list['A92959AD33434162A8545E8C7FCC5616']))

    recall_sum = 0
    positive_sum = 0
    for courseware_id in test_courseware_type_list:
        recall_sum += courseware_recall_node_count_list[courseware_id]
        positive_sum += len(test_courseware_node_id_list[courseware_id])

    print('Recall Count: ', recall_sum, 'Positive Count: ', positive_sum, 'Recall Ratio: ', recall_sum / positive_sum)


if __name__ == '__main__':
    main()
