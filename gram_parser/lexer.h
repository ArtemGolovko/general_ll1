#pragma once

#include <stdio.h>
#include "grammar.h"

typedef struct {
    const char *filename;
    FILE *source;
} Lexer;

typedef struct {
    Symbol type;
    size_t pos;
    size_t length;

    /** string */
    char *value;
} Token;


Lexer new_Lexer(const char *filename, FILE *source);


Token Lexer_next_token(Lexer *lexer);

// Debug

void Token_print(Token *token);
