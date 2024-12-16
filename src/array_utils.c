#include "array_utils.h"

bool sorted(const void *base, size_t num, size_t size, int (*compar)(const void *, const void *)) {
    const char *ptr = base;

    if (num == 0) {
        return true;
    }
    
    for (size_t i = 0; i < num - 1; i += 1) {
        if (compar(ptr + (i * size), ptr + ((i + 1) * size)) >= 1) {
            return false;
        }
    }

    return true;
}
