#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef int32_t token_type_t;

#define T_INVALID -1
#define T_$ 0
#define T_Id 1
#define T_Terminal 2
#define T_eps 3
#define T_Arrow
#define T_Semicolon

typedef struct {
    token_type_t type;
    char *value;
    size_t row;
    size_t col;
} token_t;


typedef struct {
    FILE *source;
    size_t pos;
    size_t nl;
    size_t row;
} lexer_t;

void init_lexer(lexer_t *lexer, FILE *source) {
    lexer->source = source;
    lexer->pos = 0;
    lexer->nl = 0;
    lexer->row = 1;
}
void lexer_skip_mulitline_comment(lexer_t *lexer) {
    bool is_prev_star = false;
    char c = fgetc(lexer->source);
    while (!feof(lexer->source)) {
        if (c == '*') {
            lexer->pos += 1;
            is_prev_star = true;
            c = fgetc(lexer->source);
            continue;
        }
        if (c == '\n') {
            lexer->nl = lexer->pos;
            lexer->pos += 1;
            lexer->row += 1;
            c = fgetc(lexer->source);
            continue;
        }
        if (is_prev_star && c == '/') {
            lexer->pos += 1;
            return;
        }

        lexer->pos += 1;
        is_prev_star = false;
        c = fgetc(lexer->source);
    }
}

void lexer_skip_singleline_comment(lexer_t *lexer) {
    char c = fgetc(lexer->source);
    while (!feof(lexer->source)) {
        if (c == '\n') {
            lexer->nl = lexer->pos;
            lexer->row += 1;
            lexer->pos += 1;
            return;
        }

        lexer->pos += 1;
        c = fgetc(lexer->source);
    }
}
void lexer_skip_whitespace_and_comments(lexer_t *lexer) {
    char c = fgetc(lexer->source);
    while (!feof(lexer->source)) {
        if (c == '\n') {
            lexer->nl = lexer->pos;
            lexer->pos += 1;
            lexer->row += 1;
            c = fgetc(lexer->source);
            continue;
        }

        if (isspace(c)) {
            lexer->pos += 1;
            c = fgetc(lexer->source);
            continue;
        }

        if (c == '/') {
            c = fgetc(lexer->source);
            lexer->pos += 1;
            if (c == '*') {
                lexer->pos += 1;
                lexer_skip_mulitline_comment(lexer);
                c = fgetc(lexer->source);
                continue;
            }
            if (c == '/') {
                lexer->pos += 1;
                lexer_skip_singleline_comment(lexer);
                c = fgetc(lexer->source);
                continue;
            }
            lexer->pos -= 1;
            fseek(lexer->source, -2, SEEK_CUR);
            return;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        return;
    }
}

token_t lexer_next_token(lexer_t *lexer) {
    lexer_skip_whitespace_and_comments(lexer);
    char c = fgetc(lexer->source);
    printf("First character after comment '%c'", c);

    token_t token;
    token.type = T_INVALID;
    token.value = NULL;
    token.row = lexer->row;
    token.col = lexer->pos - lexer->nl;
    return token;
}


void parse(FILE *source) {
    lexer_t lexer;
    init_lexer(&lexer, source);
    token_t token = lexer_next_token(&lexer);
    printf("Token row: %d, col: %d.\n", token.row, token.col);
}
