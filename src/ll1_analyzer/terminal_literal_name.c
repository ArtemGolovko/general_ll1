#include "terminal_literal_name.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/**
* The underscore character prefix is used to indicate program definied
* symbols in symbolic table. It aims to remove chance of name collision
*/

// _terminal_
const size_t prefix_len = 10;

char *create_terminal_literal_name(size_t id) {
    size_t id_len = floor(log10(id)) + 1;
    char *name = (char *)calloc(prefix_len + id_len + 1, sizeof(char));

    sprintf(name, "_terminal_%zd", id);

    return name;
}
