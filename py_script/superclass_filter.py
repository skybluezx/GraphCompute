import json


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
    filtrate = []
    # 获取题目的主知识点的前置列表，若为知识点子集选择该题目
    for q in question_dict.keys():
        question_pre = []
        for k in question_dict[q]:
            if k in kp_pre_total_dict.keys():
                question_pre += kp_pre_total_dict[k]
            else:
                question_pre += [k]
        if set(knowledge_pre) >= set(question_pre):
            filtrate.append(q)
    return filtrate


if __name__ == "__main__":
    with open('data/question_filtrate/kp_pre_total.txt', 'r') as f:
        kp_pre_total_dict = json.loads(f.read())
