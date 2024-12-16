#include "array_utils.h"

bool sorted(const void *base, size_t num, size_t size, int (*compar)(const void *, const void *)) {
    const char *ptr = base;
    
    for (size_t i = 1; i < num; i += 1) {
        if (compar(ptr + ((i - 1) * size), ptr + (i * size)) >= 1) {
            return false;
        }
    }

    return true;
}
