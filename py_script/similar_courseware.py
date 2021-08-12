import os
import json
from itertools import combinations
from tqdm import tqdm


def main():
    # 主知识点-课件-题目关系文件路径
    main_kp_courseware_question_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/test_data/kp_courseware_question.json'

    # 课件包含知识点列表
    courseware_main_kp_dict = dict()
    with open(main_kp_courseware_question_file_path, 'r') as f:
        for line in f:
            json_obj = json.loads(line)

            if json_obj['courseware_id'] not in courseware_main_kp_dict:
                courseware_main_kp_dict[json_obj['courseware_id']] = list()

            if json_obj['kp_id'] not in courseware_main_kp_dict[json_obj['courseware_id']]:
                courseware_main_kp_dict[json_obj['courseware_id']].append(json_obj['kp_id'])
    print(len(courseware_main_kp_dict['26ECAE4116C7480EA08F450416953D74']))
    print(len(courseware_main_kp_dict['695963A370B44E32BA9A54EC1FFBEF8E']))

    courseware_similar_dict = dict()
    for courseware_a, main_kp_list_a in courseware_main_kp_dict.items():
        courseware_similar_dict[courseware_a] = list()
        for courseware_b, main_kp_list_b in courseware_main_kp_dict.items():
            if courseware_a == courseware_b:
                continue
            result = list(set(main_kp_list_a) & set(main_kp_list_b))
            # if len(result) > 0:
            #     courseware_similar_dict[courseware_a].append({courseware_b: result})
            if len(result) == len(set(main_kp_list_a)) and len(result) == len(set(main_kp_list_b)) - 2:
                courseware_similar_dict[courseware_a].append({courseware_b: result})
                print(courseware_a, courseware_b)
    # print(courseware_similar_dict)

    courseware_similar_main_kp_count_dict = dict()
    for courseware in courseware_similar_dict.keys():
        courseware_similar_main_kp_count_dict[courseware] = dict()
        for similar_courseware in courseware_similar_dict[courseware]:
            if courseware == list(similar_courseware.keys())[0]:
                continue

            similar_main_kp_count = len(similar_courseware[list(similar_courseware.keys())[0]])
            if similar_main_kp_count not in courseware_similar_main_kp_count_dict[courseware]:
                courseware_similar_main_kp_count_dict[courseware][similar_main_kp_count] = 0
            courseware_similar_main_kp_count_dict[courseware][similar_main_kp_count] += 1
    # print(courseware_similar_main_kp_count_dict)

    target_similar_main_kp_count = 10
    target_similar_courseware_dict = dict()
    for courseware in courseware_similar_main_kp_count_dict.items():
        sum = 0
        for count in courseware[1].items():
            if count[0] >= target_similar_main_kp_count:
                sum += count[1]
        target_similar_courseware_dict[courseware[0]] = sum
    # print(target_similar_courseware_dict)

    # for courseware in target_similar_courseware_dict.items():
    #     print(courseware[0], courseware[1])


if __name__ == '__main__':
    main()
