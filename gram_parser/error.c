#include "error.h"

#include <math.h>
#include "../datastructs/string.h"

typedef struct {
    size_t pos;
    size_t length;
    const char *message;
    const char *filename;
    FILE *file;
} Error;

void display_error(Error *error) {
    size_t og_pos = ftell(error->file);
    rewind(error->file);

    size_t pos = 0;
    size_t row = 1;
    size_t nl = 0;

    while (pos < error->pos && !feof(error->file)) {
        char c = fgetc(error->file);
        pos += 1;

        if (c == '\n') {
            nl = pos;
            row += 1;
            continue;
        }
    }

    size_t col = pos - nl;
    // fprintf(stderr, "Row: %zd, Col: %zd, NL: %zd\n", row, col, nl);

    char *line = new_string(col);
    if (nl == 0) {
        fseek(error->file, nl, SEEK_SET);
    } else {

        fseek(error->file, nl + 2, SEEK_SET);
    }
    
    char c = fgetc(error->file);
    while (c != '\n' && c != EOF && !feof(error->file)) {
        string_push(&line, c);

        c = fgetc(error->file);
    }

    size_t tab_length = floor(log10(row)) + 1 ;
    // fprintf(stderr, "Line: |%s|\n", line);

    fprintf(stderr, "%s:%zd:%zd\n", error->filename, row, col);

    for (size_t i = 0; i < tab_length; i += 1) {
        fprintf(stderr, " ");
    }

    fprintf(stderr, " |\n%zd | %s\n", row, line);

    for (size_t i = 0; i < tab_length; i += 1) {
        fprintf(stderr, " ");
    }

    fprintf(stderr, " | ");
    
    for (size_t i = 0; i < col - 1; i += 1) {
        fprintf(stderr, " ");
    }

    for (size_t i = 0; i < error->length; i += 1) {
        fprintf(stderr, "^");
    }

    fprintf(stderr, " %s\n\n", error->message);
    
    fseek(error->file, og_pos, SEEK_SET);
}

void display_not_matching_terminal_error(Lexer *lexer, Symbol top, Token *token) {
    char *message = "display_not_matching_terminal_error";
    Error error = {
        token->pos,
        token->length,
        message,
        lexer->filename,
        lexer->source
    };

    display_error(&error);
}

void display_not_matching_rule_error(Lexer *lexer, Symbol top, Token *token) {
    char *message = "display_not_matching_rule_error";
    Error error = {
        token->pos,
        token->length,
        message,
        lexer->filename,
        lexer->source
    };

    display_error(&error);
}
