//
// Created by QWERTY on 6/23/2022.
//

#include "interpret.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct tape_t {
    long long tape[NBR_TAPE_SIZE];
    long long ptr;
};

inline static void apply_offsets(struct tape_t *tape, const struct cfg_basic_loop_tape_offset_table *table) {
    assert(tape->tape[tape->ptr] % table->offsets[NBR_OFFSET_TABLE_CENTER] == 0);

    long long multiplier = -(tape->tape[tape->ptr] / table->offsets[NBR_OFFSET_TABLE_CENTER]);

    long long *TAPE_PTR = tape->tape + tape->ptr - (NBR_OFFSET_TABLE_CENTER - table->min_ptr);
    const long long *TABLE_PTR_BEGIN = table->offsets + table->min_ptr;
    const long long *TABLE_PTR_END = table->offsets + table->max_ptr;

    for (const long long *TABLE_PTR = TABLE_PTR_BEGIN; TABLE_PTR <= TABLE_PTR_END; TABLE_PTR++, TAPE_PTR++)
        *TAPE_PTR += (*TABLE_PTR) * multiplier;
}

void nbr_interpret(struct interpret_context_t *context, struct cfg_container *container,
                   struct nbr_opt_profile *opt_profile) {
    struct tape_t *tape = malloc(sizeof(struct tape_t));

    memset(tape->tape, 0, NBR_TAPE_SIZE * sizeof(long long));
    tape->ptr = NBR_TAPE_CENTER;

    struct cfg_node_t *current_node = container->buf_start;

    while (current_node < container->buf_start + container->actual_siz) {
        bool goto_next = true;

        switch (current_node->kind) {
            case ADD:
                tape->tape[tape->ptr] += current_node->attached_data.val;
                break;
            case SHIFT_HEAD:
                tape->ptr += current_node->attached_data.val;
                break;
            case START_LOOP:
                if (tape->tape[tape->ptr] == 0) {
                    current_node = current_node->loop_ptr; // keep goto_next = true so that it goes to the next node
                }

                if (!opt_profile->opt_loops) {
                    goto START_LOOP_NO_OPT;
                }

                if (current_node->attached_data.loop_data != NULL) {
                    struct cfg_basic_loop_tape_offset_table *table = current_node->attached_data.loop_data;

                    apply_offsets(tape, table);

                    current_node = current_node->loop_ptr;
                }


            START_LOOP_NO_OPT:

                break;
            case END_LOOP:
                if (tape->tape[tape->ptr] != 0) {
                    current_node = current_node->loop_ptr;
                    goto_next = false;
                }
                break;
            case READ:
                tape->tape[tape->ptr] = fgetc(context->in);
                break;
            case PRINT:
                fputc((char) tape->tape[tape->ptr], context->out);
                fflush(context->out);
                break;
            case COL:
                break;
        }

        if (goto_next) {
            if (current_node->next != NULL) current_node = current_node->next;
            else current_node++;
        }
    }

    free(tape);
}
