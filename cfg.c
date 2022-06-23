//
// Created by QWERTY on 6/23/2022.
//

#include "cfg.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>


struct cfg_container nbr_setup_cfg(size_t size) {
    struct cfg_container result;
    result.buf_siz = size;
    result.buf_start = malloc(size * sizeof(struct cfg_node_t));
    result.actual_siz = 0;

    assert(result.buf_start != NULL);

    return result;
}

static struct cfg_node_t *
nbr_container_append(struct cfg_container *container, enum cfg_node_kind kind, long long attached_data,
                     struct cfg_node_t *next,
                     struct cfg_node_t *loop_ptr);

void nbr_build_cfg(const char *file, size_t file_size, struct cfg_container *container,
                   struct nbr_opt_profile *opt_profile) {
    struct cfg_node_t **loop_stack = malloc(file_size / 2 * sizeof(struct cfg_node_t *));
    size_t loop_stack_ptr = 0;
    struct cfg_node_t *prev_node = NULL;

    if (opt_profile->quick_chains)
        goto QUICK_CHAINS;
    else
        goto NO_QUICK_CHAINS;

    QUICK_CHAINS:

    for (size_t i = 0; i < file_size; i++) {
        switch (file[i]) {
            case '+':
                if (prev_node != NULL && prev_node->kind == ADD) {
                    prev_node->attached_data.val++;
                } else {
                    prev_node = nbr_container_append(container, ADD, 1, NULL, NULL);
                }
                break;
            case '-':
                if (prev_node != NULL && prev_node->kind == ADD) {
                    prev_node->attached_data.val--;
                } else {
                    prev_node = nbr_container_append(container, ADD, -1, NULL, NULL);
                }
                break;
            case '<':
                if (prev_node != NULL && prev_node->kind == SHIFT_HEAD) {
                    prev_node->attached_data.val--;
                } else {
                    prev_node = nbr_container_append(container, SHIFT_HEAD, -1, NULL, NULL);
                }
                break;
            case '>':
                if (prev_node != NULL && prev_node->kind == SHIFT_HEAD) {
                    prev_node->attached_data.val++;
                } else {
                    prev_node = nbr_container_append(container, SHIFT_HEAD, 1, NULL, NULL);
                }
                break;
            case '.':
                prev_node = nbr_container_append(container, PRINT, 0, NULL, NULL);
                break;
            case ',':
                prev_node = nbr_container_append(container, READ, 0, NULL, NULL);
                break;
            case '[':
                prev_node = loop_stack[loop_stack_ptr++] = nbr_container_append(container, START_LOOP, 0, NULL, NULL);
                break;
            case ']':
                assert(loop_stack_ptr > 0);
                struct cfg_node_t *start_node = loop_stack[loop_stack_ptr - 1];
                struct cfg_node_t *end_node = prev_node = nbr_container_append(container, END_LOOP, 0, NULL, start_node);
                start_node->loop_ptr = end_node;

                loop_stack_ptr--;

                break;
        }
    }

    return;

    NO_QUICK_CHAINS:

    for (size_t i = 0; i < file_size; i++) {
        switch (file[i]) {
            case '+':
                nbr_container_append(container, ADD, 1, NULL, NULL);
                break;
            case '-':
                nbr_container_append(container, ADD, -1, NULL, NULL);
                break;
            case '<':
                nbr_container_append(container, SHIFT_HEAD, -1, NULL, NULL);
                break;
            case '>':
                nbr_container_append(container, SHIFT_HEAD, 1, NULL, NULL);
                break;
            case '.':
                nbr_container_append(container, PRINT, 0, NULL, NULL);
                break;
            case ',':
                nbr_container_append(container, READ, 0, NULL, NULL);
                break;
            case '[':
                loop_stack[loop_stack_ptr++] = nbr_container_append(container, START_LOOP, 0, NULL, NULL);
                break;
            case ']':
                assert(loop_stack_ptr > 0);
                struct cfg_node_t *start_node = loop_stack[loop_stack_ptr - 1];
                struct cfg_node_t *end_node = nbr_container_append(container, END_LOOP, 0, NULL, start_node);
                start_node->loop_ptr = end_node;

                loop_stack_ptr--;

                break;
        }
    }

    return;
}


static struct cfg_node_t *
nbr_container_append(struct cfg_container *container, enum cfg_node_kind kind, long long attached_data,
                     struct cfg_node_t *next,
                     struct cfg_node_t *loop_ptr) {
    assert(container->actual_siz < container->buf_siz);

    container->buf_start[container->actual_siz].kind = kind;
    container->buf_start[container->actual_siz].attached_data.val = attached_data;
    container->buf_start[container->actual_siz].next = next;
    container->buf_start[container->actual_siz].loop_ptr = loop_ptr;

    return &container->buf_start[container->actual_siz++];
}

void nbr_dump_cfg(FILE *fp, struct cfg_container *container) {
    for (size_t i = 0; i < container->actual_siz; i++) {
        char kind_t[20];

        struct cfg_node_t *ptr = &container->buf_start[i];

        switch (ptr->kind) {
            case ADD:
                strcpy(kind_t, "ADD");
                break;
            case SHIFT_HEAD:
                strcpy(kind_t, "SHIFT_HEAD");
                break;
            case START_LOOP:
                strcpy(kind_t, "START_LOOP");
                break;
            case END_LOOP:
                strcpy(kind_t, "END_LOOP");
                break;
            case READ:
                strcpy(kind_t, "READ");
                break;
            case PRINT:
                strcpy(kind_t, "PRINT");
                break;
            case COL:
                strcpy(kind_t, "COL");
                break;
        }

        fprintf(fp, "%ld: %s [%ld]", i, kind_t, ptr->attached_data);

        if (ptr->loop_ptr != NULL)
            fprintf(fp, " [LOOP: %ld]", ptr->loop_ptr - container->buf_start);

        fprintf(fp, "\n");
    }
}

void nbr_free_container(struct cfg_container *container) {
    free(container->buf_start);
}
