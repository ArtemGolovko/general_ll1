#pragma once

#include "gram_parser/location.h"

typedef enum {
    LexerError,
    ParserError
} SyntaxErrorOrigin;

typedef struct {
    SyntaxErrorOrigin origin;

    char *message;

    unsigned int token_length;
    Location loc;

    const char *source;
    int source_length;
} SyntaxError;

void SyntaxError_display(SyntaxError *error);

void free_SyntaxError(SyntaxError *error);

