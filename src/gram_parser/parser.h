#pragma once

#include "gram_parser/error.h"
#include <stdbool.h>

typedef struct {
    void *ast;
    SyntaxError *errors;
} ParsingResult;


ParsingResult parse(const char *filename, const char *source, size_t source_length);
