#pragma once

#include <stdbool.h>

bool sorted(const void *base, size_t num, size_t size, int (*compar)(const void*, const void*));
// bool lsearch(const void *key, const void *base, size_t num, size_t size, int (*compar)(const void*, const void*));
