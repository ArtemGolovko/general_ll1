#pragma once

#include "grammar.h"

typedef struct {
    const char *filename;

    size_t pos;
    size_t col;
    size_t row;
} Location;

typedef struct {
    const char *source;
    size_t source_length;

    Location loc;
    size_t ln_pos;
} Lexer;


typedef struct {
    Symbol type;
    size_t length;
    Location loc;

    /** string */
    char *value;
} Token;


Lexer new_Lexer(const char *filename, const char *source, size_t source_length);


Token Lexer_next_token(Lexer *lexer);

// Debug

void Token_print(Token *token);
