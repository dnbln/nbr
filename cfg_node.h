//
// Created by QWERTY on 6/23/2022.
//

#ifndef NBR_CFG_NODE_H
#define NBR_CFG_NODE_H

#include "config.h"

enum cfg_node_kind {
    ADD, SHIFT_HEAD, START_LOOP, END_LOOP, READ, PRINT, COL
};

struct cfg_basic_loop_tape_offset_table {
    long long offsets[NBR_OFFSET_TABLE_SIZE];
    size_t min_ptr, max_ptr;
};

union cfg_node_attached_data_t {
    long long val;
    struct cfg_basic_loop_tape_offset_table* loop_data;
};

struct cfg_node_t {
    enum cfg_node_kind kind;

    union cfg_node_attached_data_t attached_data;

    struct cfg_node_t* next;
    struct cfg_node_t* loop_ptr;
};

#endif //NBR_CFG_NODE_H
