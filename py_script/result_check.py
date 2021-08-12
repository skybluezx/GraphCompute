import os
import json


def main():
    recommend_result_directory = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/result/main'
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

    # 读取测试集中全部课件对应的主知识点列表和题目列表
    test_type_list = list()
    test_courseware_type_list = dict()
    test_courseware_node_id_list = dict()
    with open(test_type_node_id_file_path, 'r') as f:
        for line in f:
            test_type_node = json.loads(line)

            if test_type_node['kp_id'] not in test_type_list:
                test_type_list.append(test_type_node['kp_id'])

            if test_type_node['courseware_id'] not in test_courseware_type_list:
                test_courseware_type_list[test_type_node['courseware_id']] = list()
            test_courseware_type_list[test_type_node['courseware_id']].append(test_type_node['kp_id'])

            if test_type_node['courseware_id'] not in test_courseware_node_id_list:
                test_courseware_node_id_list[test_type_node['courseware_id']] = test_type_node['question']
    # print(test_type_list)
    # print(test_courseware_type_list.keys())
    # print(test_courseware_node_id_list)

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

    recall_sum = 0
    positive_sum = 0
    for courseware_id in test_courseware_type_list:
        recall_sum += courseware_recall_node_count_list[courseware_id]
        positive_sum += len(test_courseware_node_id_list[courseware_id])

    print('Recall Count: ', recall_sum, 'Positive Count: ', positive_sum, 'Recall Ratio: ', recall_sum / positive_sum)


if __name__ == '__main__':
    main()
