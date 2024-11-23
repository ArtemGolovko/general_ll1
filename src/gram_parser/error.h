#pragma once

#include "location.h"

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

void free_SyntaxError(SyntaxError *error);

