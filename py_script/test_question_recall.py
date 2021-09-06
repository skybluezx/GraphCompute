import os
import json


def get_courseware_main_kp(kp_courseware_question_file_path):
    courseware_main_kp_list = dict()
    with open(kp_courseware_question_file_path, 'r') as f:
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


def get_recall_task_json_from_kp(save_dir_path, courseware_main_kp_list, is_merge, stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount, isSplitStepCount, visitedCountTopN):
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


def get_recall_task_json_from_course(save_dir_path, course_id_list, course_request_node_list, is_merge, stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount, isSplitStepCount, visitedCountTopN):
    for course in course_request_node_list:
        if course not in course_id_list:
            continue

        task_json = dict()

        task_json["name"] = "multi_walk"

        task_json["beginNodeType"] = ["KnowledgePoint", "Question"]

        task_json["beginNodeID"] = list()
        if 'kp' in course_request_node_list[course]:
            task_json["beginNodeID"].append(course_request_node_list[course]['kp'])
        else:
            task_json["beginNodeID"].append(list())
        if 'question' in course_request_node_list[course]:
            task_json["beginNodeID"].append(course_request_node_list[course]['question'])
        else:
            task_json["beginNodeID"].append(list())

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

        with open(save_dir_path + "/" + course + '.json', 'w') as f:
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


def get_question_mastery_dict(question_mastery_file_path):
    question_mastery_dict = dict()
    with open(question_mastery_file_path, 'r') as f:
        obj = json.load(f)
        for course in obj:
            question_mastery_dict[course] = list()
            data_list = obj[course]['data']
            for data in data_list:
                question_mastery_dict[course].append({'id': data['que_id'], 'weight': float(data['que_mastery_level'])})
    return question_mastery_dict


def get_kp_mastery_dict(kp_mastery_file_path):
    kp_mastery_dict = dict()
    with open(kp_mastery_file_path, 'r') as f:
        for line in f:
            if line.strip() == "":
                continue
            obj = json.loads(line)
            for course in obj:
                kp_mastery_dict[course] = list()
                for kp in obj[course]:
                    kp_mastery_dict[course].append({'id': kp, 'weight': obj[course][kp]})
    return kp_mastery_dict


def question_and_kp_merge(question_mastery_dict, kp_mastery_dict):
    request_node_list = dict()

    for course in question_mastery_dict:
        if course not in request_node_list:
            request_node_list[course] = dict()
        request_node_list[course]['question'] = question_mastery_dict[course]

    for course in kp_mastery_dict:
        if course not in request_node_list:
            request_node_list[course] = dict()
        request_node_list[course]['kp'] = kp_mastery_dict[course]

    return request_node_list


if __name__ == "__main__":
    # 客户端路径
    recall_bin_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/bin/macos-clang-12.0.5/GraphComputeClient'

    # 测试知识点-课件-题目文件路径
    test_kp_courseware_question_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/kp_courseware_question.json'
    # 测试题目掌握文件路径
    test_question_mastery_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/question_mastery.json'
    # 测试知识点掌握文件路径
    test_kp_mastery_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/kp_mastery.dat'

    # 生成召回任务的json文件目录路径
    task_json_save_dir_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/test_course_task_json'

    # # 获取全部课件的召回知识点和召回题目
    # course_kp_list = get_courseware_main_kp(test_kp_courseware_question_file_path)

    # 获取全部题目掌握数据
    question_mastery_dict = get_question_mastery_dict(test_question_mastery_file_path)

    # 获取全部知识点掌握数据
    kp_mastery_dict = get_kp_mastery_dict(test_kp_mastery_file_path)

    course_request_node_list = question_and_kp_merge(question_mastery_dict, kp_mastery_dict)
    # print(course_request_node_list)

    is_merge = True
    stepDefine = [
        ["KnowledgePoint", "Question", "Courseware"],
        ["KnowledgePoint", "Question", "Courseware"]
    ]
    auxiliaryEdge = [
        # {"Question": "Courseware"},
        # {"Question": "Courseware"},
    ]
    walkLengthRatio = [0.0, 0.0]
    restartRatio = [0.02, 0.02]
    totalStepCount = [100000, 100000]
    isSplitStepCount = [False, False]
    visitedCountTopN = 100

    # get_recall_task_json_from_kp(task_json_save_dir_path,
    #                              course_kp_list,
    #                              is_merge,
    #                              stepDefine,
    #                              auxiliaryEdge,
    #                              walkLengthRatio,
    #                              restartRatio,
    #                              totalStepCount,
    #                              isSplitStepCount,
    #                              visitedCountTopN)

    course_id_list = ['1685524',
                      '9911711',
                      '10381553',
                      '6086788',
                      '1644252',
                      '1627344',
                      '1652910',
                      '1689361',
                      '1630150',
                      '10490049']
    # get_recall_task_json_from_course(task_json_save_dir_path,
    #                                  course_id_list,
    #                                  course_request_node_list,
    #                                  is_merge,
    #                                  stepDefine,
    #                                  auxiliaryEdge,
    #                                  walkLengthRatio,
    #                                  restartRatio,
    #                                  totalStepCount,
    #                                  isSplitStepCount,
    #                                  visitedCountTopN)
    
    # recall_from_file(recall_bin_path, task_json_save_dir_path, course_kp_list)
