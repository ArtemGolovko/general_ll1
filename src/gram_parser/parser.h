#pragma once

#include <stdbool.h>

#include "gram_parser/error.h"
#include "gram_parser/ast.h"

typedef struct {
    ASTRules *ast;
    SyntaxError *errors;
} ParsingResult;


ParsingResult parse(const char *filename, const char *source, size_t source_length);
