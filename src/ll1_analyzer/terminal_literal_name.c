#include "terminal_literal_name.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// terminal_
const size_t prefix_len = 9;

char *create_terminal_literal_name(size_t id) {
    size_t id_len = floor(log10(id)) + 1;
    char *name = (char *)calloc(prefix_len + id_len + 1, sizeof(char));

    sprintf(name, "terminal_%zd", id);

    return name;
}
