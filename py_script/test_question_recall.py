import json


def get_test_courseware_main_kp(test_kp_courseware_question_file_path):
    courseware_main_kp_list = dict()
    with open(test_kp_courseware_question_file_path, 'r') as f:
        for line in f:
            json_object = json.loads(line)

            if json_object['courseware_id'] not in courseware_main_kp_list:
                courseware_main_kp_list[json_object['courseware_id']] = list()
                courseware_main_kp_list[json_object['courseware_id']].append(list())
                courseware_main_kp_list[json_object['courseware_id']].append(json_object['question'])

            courseware_main_kp_list[json_object['courseware_id']][0].append(json_object['kp_id'])

    return courseware_main_kp_list


def get_test_recall_task_json(courseware_main_kp_list, save_dir_path, stepDefine, auxiliaryEdge, walkLengthRatio, restartRatio, totalStepCount, isSplitStepCount, visitedCountTopN):
    for cw in courseware_main_kp_list:
        task_json = dict()

        task_json["name"] = "multi_walk"

        task_json["beginNodeType"] = ["KnowledgePoint", "Question"]

        task_json["beginNodeID"] = list()
        task_json["beginNodeID"].append(courseware_main_kp_list[cw][0])
        task_json["beginNodeID"].append(courseware_main_kp_list[cw][1])

        task_json["is_merge"] = True

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


if __name__ == "__main__":
    test_kp_courseware_question_file_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/kp_courseware_question.json'
    course_kp_list = get_test_courseware_main_kp(test_kp_courseware_question_file_path)

    task_json_save_dir_path = '/Users/zhaixiao/workplace/c_cpp/GraphCompute/build/test_data/test_courseware_task_json'
    stepDefine = [
        ["KnowledgePoint", "Question"],
        ["KnowledgePoint", "Question"]
    ]
    auxiliaryEdge = [
        {"Question": "Courseware"},
        {"Question": "Courseware"},
    ]
    walkLengthRatio = [1.0, 1.0]
    restartRatio = [0.0, 0.0]
    totalStepCount = [100000, 100000]
    isSplitStepCount = [False, False]
    visitedCountTopN = 100

    get_test_recall_task_json(course_kp_list,
                              task_json_save_dir_path,
                              stepDefine,
                              auxiliaryEdge,
                              walkLengthRatio,
                              restartRatio,
                              totalStepCount,
                              isSplitStepCount,
                              visitedCountTopN)
