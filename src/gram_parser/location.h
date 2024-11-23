#pragma once

typedef struct {
    const char *filename;

    size_t pos;
    size_t col;
    size_t row;
} Location;

