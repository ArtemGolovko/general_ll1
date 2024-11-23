#include "error.h"

#include <stdlib.h>

void free_SyntaxError(SyntaxError *error) {
    free(error->message);
}
