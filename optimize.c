//
// Created by QWERTY on 6/23/2022.
//

#include <stdlib.h>
#include <string.h>
#include "optimize.h"

static struct cfg_basic_loop_tape_offset_table *check_basic_loop(struct cfg_node_t *start, struct cfg_node_t *end) {
    struct cfg_basic_loop_tape_offset_table *table = malloc(sizeof(struct cfg_basic_loop_tape_offset_table));

    memset(table->offsets, 0, sizeof(long long) * NBR_OFFSET_TABLE_SIZE);

    struct cfg_node_t *ptr = start + 1;
    size_t curr_offset = NBR_OFFSET_TABLE_CENTER;
    table->min_ptr = table->max_ptr = curr_offset;

    while (ptr < end) {
        switch (ptr->kind) {
            case ADD:
                if (curr_offset < 0 || curr_offset >= NBR_OFFSET_TABLE_SIZE) {
                    goto FAIL;
                }

                table->offsets[curr_offset] += ptr->attached_data.val;

                if (curr_offset < table->min_ptr) {
                    table->min_ptr = curr_offset;
                }

                if (curr_offset > table->max_ptr) {
                    table->max_ptr = curr_offset;
                }

                break;
            case SHIFT_HEAD:
                curr_offset += (int) ptr->attached_data.val;

                break;
            case COL:
                break;
            default:
                goto FAIL;
        }

        ptr++;
    }

    if (curr_offset != NBR_OFFSET_TABLE_CENTER)
        goto FAIL;


    return table;

    FAIL:
    free(table);
    return NULL;
}

void nbr_optimize(struct cfg_container *container, struct nbr_opt_profile *opt_profile) {
    if (!opt_profile->opt_loops) {
        goto FINISH_OPT_LOOPS;
    }

    for (size_t i = 0; i < container->actual_siz; i++) {
        struct cfg_node_t *ptr = &container->buf_start[i];

        if (ptr->kind == START_LOOP) {
            struct cfg_node_t *start = ptr, *end = ptr->loop_ptr;
            struct cfg_basic_loop_tape_offset_table *off_table = check_basic_loop(start, end);

            if (off_table != NULL) {
                start->attached_data.loop_data = off_table;
            }
        }
    }


    FINISH_OPT_LOOPS:
    return;
}
