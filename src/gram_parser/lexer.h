#pragma once

#include "location.h"
#include "gram_parser/error.h"
#include "grammar.h"


typedef struct {
    const char *source;
    size_t source_length;

    Location loc;

    SyntaxError last_error;
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
