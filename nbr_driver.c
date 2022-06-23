//
// Created by QWERTY on 6/23/2022.
//

#include "nbr_driver.h"
#include "cfg.h"
#include "optimize.h"
#include "interpret.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void nbr_start(char *file, size_t size) {
    char* buf = malloc(size);
    assert(buf != NULL);

    FILE* fp = fopen(file, "rb");
    size_t rd = fread(buf, 1, size, fp);
    assert(rd == size);

    struct cfg_container container = nbr_setup_cfg(size);

    struct nbr_opt_profile opt_profile;
    opt_profile.quick_chains = true;
    opt_profile.opt_loops = true;

    nbr_build_cfg(buf, size, &container, &opt_profile);
//    nbr_dump_cfg(stdout, &container);

    nbr_optimize(&container, &opt_profile);

    struct interpret_context_t interpret_context;
    interpret_context.in = stdin;
    interpret_context.out = stdout;

    nbr_interpret(&interpret_context, &container, &opt_profile);

    nbr_free_container(&container);
}

