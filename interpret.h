//
// Created by QWERTY on 6/23/2022.
//

#ifndef NBR_INTERPRET_H
#define NBR_INTERPRET_H

#include <stdio.h>
#include "cfg.h"

struct interpret_context_t {
    FILE *in, *out;
};

void nbr_interpret(struct interpret_context_t* context, struct cfg_container* container, struct nbr_opt_profile* opt_profile);

#endif //NBR_INTERPRET_H
