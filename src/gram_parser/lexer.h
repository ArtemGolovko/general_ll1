#pragma once

#include "location.h"
#include "gram_parser/error.h"
#include "grammar.h"


typedef struct {
    const char *source;
    size_t source_length;

    Location loc;
    size_t ln_pos;

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

SyntaxError Lexer_get_last_error(Lexer *lexer);

// Debug

void Token_print(Token *token);
