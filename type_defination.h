//
// Created by 翟霄 on 2021/7/25.
//

#ifndef GRAPHCOMPUTE_TYPE_DEFINATION_H
#define GRAPHCOMPUTE_TYPE_DEFINATION_H

/**
 * 在图中游走的方向
 */
enum WalkingDirection {
    WIDE,   // 广度优先
    DEEP    // 深度优先
};

/**
 * 在图中游走时的边选择策略
 */
enum EdgeChooseStrategy {
    FIRST,              // 链表中的第一条边
    LAST,               // 链表中的最后一条边
    RANDOM,             // 链表中随机的一条边
    FIRST_NO_VISIT,     // 链表中未访问过的第一条边
    LAST_NO_VISIT,      // 链表中未访问过的最后一条边
    RANDOM_NO_VISIT,    // 链表中未访问的随机一条边
    CUSTOMIZED          // 自定义的边选择策略
};

#endif //GRAPHCOMPUTE_TYPE_DEFINATION_H
