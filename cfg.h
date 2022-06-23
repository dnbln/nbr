//
// Created by QWERTY on 6/23/2022.
//

#ifndef NBR_CFG_H
#define NBR_CFG_H

#include <stddef.h>
#include <stdio.h>
#include "cfg_node.h"
#include "opt_profile.h"

struct cfg_container {
    struct cfg_node_t *buf_start;
    size_t buf_siz;
    size_t actual_siz;
};

struct cfg_container nbr_setup_cfg(size_t size);
void nbr_build_cfg(const char* file, size_t file_size, struct cfg_container* container, struct nbr_opt_profile* opt_profile);
void nbr_dump_cfg(FILE *fp, struct cfg_container *container);
void nbr_free_container(struct cfg_container* container);

#endif //NBR_CFG_H
