import os
import json


def get_test_courseware_main_kp(test_kp_courseware_question_file_path):
    courseware_main_kp_list = dict()
    with open(test_kp_courseware_question_file_path, 'r') as f:
        for line in f:
            json_object = json.loads(line)

            if json_object['courseware_id'] not in courseware_main_kp_list:
                courseware_main_kp_list[json_object['courseware_id']] = list()
                courseware_main_kp_list[json_object['courseware_id']].append(list())
                courseware_main_kp_list[json_object['courseware_id']].append(list())

                for question_id in json_object['question']:
                    courseware_main_kp_list[json_object['courseware_id']][1].append({'id': question_id, 'weight': 1.0})

            courseware_main_kp_list[json_object['courseware_id']][0].append({'id': json_object['kp_id'], 'weight': 1.0})

    return courseware_main_kp_list


def get_test_recall_task_json(save_dir_path, courseware_main_kp_list, is_merge, stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount, isSplitStepCount, visitedCountTopN):
    for cw in courseware_main_kp_list:
        task_json = dict()

        task_json["name"] = "multi_walk"

        task_json["beginNodeType"] = ["KnowledgePoint", "Question"]

        task_json["beginNodeID"] = list()
        task_json["beginNodeID"].append(courseware_main_kp_list[cw][0])
        task_json["beginNodeID"].append(courseware_main_kp_list[cw][1])

        task_json["is_merge"] = is_merge

        task_json["stepDefine"] = stepDefine

        task_json["auxiliaryEdge"] = auxiliaryEdge

        task_json["walkLengthRatio"] = walkLengthRatio

        task_json["restartRatio"] = restartRatio

        task_json["totalStepCount"] = totalStepCount

        task_json["isSplitStepCount"] = isSplitStepCount

        task_json["targetNodeType"] = "Question"

        task_json["visitedCountTopN"] = visitedCountTopN

        # print(task_json)
        # break

        with open(save_dir_path + "/" + cw + '.json', 'w') as f:
            json.dump(task_json, f)


def recall_from_file(recall_bin_path, save_dir_path, course_kp_list):
    # 遍历全部课件
    for cw in course_kp_list:
        recall_command_json_file_path = save_dir_path + "/" + cw + '.json'
        # print(recall_command_json_file_path)
        command = recall_bin_path + ' --server_name=main --json=' + recall_command_json_file_path
        print(command)

        os.system(command)
        break


if __name__ == "__main__":
    recall_bin_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/bin/macos-clang-12.0.5/GraphComputeClient'
    test_kp_courseware_question_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/kp_courseware_question.json'
    task_json_save_dir_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/test_courseware_task_json'

    # 获取全部课件的召回知识点和召回题目
    course_kp_list = get_test_courseware_main_kp(test_kp_courseware_question_file_path)

    # is_merge = True
    # stepDefine = [
    #     ["KnowledgePoint", "Question", "Courseware"],
    #     ["KnowledgePoint", "Question", "Courseware"]
    # ]
    # auxiliaryEdge = [
    #     # {"Question": "Courseware"},
    #     # {"Question": "Courseware"},
    # ]
    # walkLengthRatio = [1.0, 1.0]
    # restartRatio = [0.0, 0.0]
    # totalStepCount = [100000, 100000]
    # isSplitStepCount = [False, False]
    # visitedCountTopN = 100
    #
    # get_test_recall_task_json(task_json_save_dir_path,
    #                           course_kp_list,
    #                           is_merge,
    #                           stepDefine,
    #                           auxiliaryEdge,
    #                           walkLengthRatio,
    #                           restartRatio,
    #                           totalStepCount,
    #                           isSplitStepCount,
    #                           visitedCountTopN)

    recall_from_file(recall_bin_path, task_json_save_dir_path, course_kp_list)
