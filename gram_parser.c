#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    Invalid,
    Eof,
    Id,
    Terminal,
    Eps,
    Arrow,
    Semicolon
} token_type_t;


typedef struct {
    token_type_t type;
    char *value;
    size_t row;
    size_t col;
    size_t pos;
} token_t;

typedef struct {
    FILE *source;
    size_t nl;
    size_t row;
} lexer_t;

void init_token(token_t *token, lexer_t* lexer, token_type_t type, size_t length) {
    token->type = type;
    token->row = lexer->row;
    token->pos = ftell(lexer->source) - length;
    token->col = token->pos - lexer->nl + 1; 
    token->value = NULL;
}

void init_lexer(lexer_t *lexer, FILE *source) {
    lexer->source = source;
    lexer->nl = 0;
    lexer->row = 1;
}

void lexer_skip_mulitline_comment(lexer_t *lexer) {
    char c;
    bool is_prev_star = false;

    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);

        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            continue;
        }

        if (is_prev_star && c == '/') {
            return;
        }

        is_prev_star = c == '*';
    }
}

void lexer_skip_singleline_comment(lexer_t *lexer) {
    char c;
    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);
        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            return;
        }
    }
}

void lexer_skip_whitespace_and_comments(lexer_t *lexer) {
    char c;
    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);
        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            continue;
        }

        if (isspace(c) || c == -1) {
            continue;
        }

        if (c == '/') {
            c = fgetc(lexer->source);
            if (c == '*') {
                lexer_skip_mulitline_comment(lexer);
                continue;
            }

            if (c == '/') {
                lexer_skip_singleline_comment(lexer);
                continue;
            }

            fseek(lexer->source, -2, SEEK_CUR);
            return;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        return;
    }

}

token_t lexer_read_Id(lexer_t *lexer) {
    char c;
    char *buffer = (char *)malloc(15 * sizeof(char));
    size_t buffer_length = 14;
    size_t buffer_usage = 0;

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);
        if (isalpha(c) || c == '_') {
            if (buffer_usage + 1 >= buffer_length) {
                buffer_length += 15;
                buffer = realloc(buffer, buffer_length * sizeof(char));
            }

            buffer[buffer_usage] = c;
            buffer_usage += 1;
            continue;
        }

        
        fseek(lexer->source, -1, SEEK_CUR);
        token_t token;

        init_token(&token, lexer, Id, buffer_usage);
        token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

        buffer[buffer_usage] = '\0';
        strcpy_s(token.value, buffer_usage + 1, buffer);

        free(buffer);

        return token;
    }

    fseek(lexer->source, -1, SEEK_CUR);
    token_t token;

    init_token(&token, lexer, Id, buffer_usage);
    token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

    buffer[buffer_usage] = '\0';
    strcpy_s(token.value, buffer_usage + 1, buffer);

    free(buffer);

    return token;
}

token_t lexer_read_Terminal(lexer_t *lexer) {
    char c;
    bool is_prev_backslash = false;
    char *buffer = (char *)malloc(15 * sizeof(char));
    size_t buffer_length = 14;
    size_t buffer_usage = 0;

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);

        if (!is_prev_backslash && c == '"') {
            token_t token;

            init_token(&token, lexer, Terminal, buffer_usage);
            token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

            buffer[buffer_usage] = '\0';
            strcpy_s(token.value, buffer_usage + 1, buffer);

            free(buffer);

            return token;
        }
        
        is_prev_backslash = c == '\\';

        if (buffer_usage + 1 >= buffer_length) {
            buffer_length += 15;
            buffer = realloc(buffer, buffer_length * sizeof(char));
        }

        buffer[buffer_usage] = c;
        buffer_usage += 1;
    }
    
    free(buffer);

    token_t token;
    init_token(&token, lexer, Invalid, buffer_usage + 1);
    return token;
}

token_t lexer_next_token(lexer_t *lexer) {
    lexer_skip_whitespace_and_comments(lexer);
    
    token_t token;

    if (feof(lexer->source)) {
        init_token(&token, lexer, Eof, 0); 
        return token;
    }

    char c = fgetc(lexer->source);

    if (c == ';') {
        init_token(&token, lexer, Semicolon, 1);
        return token; 
    }

    if (c == '-') {
        c = fgetc(lexer->source);
        if (c == '>') {
            init_token(&token, lexer, Arrow, 2);
            return token;
        }
        fseek(lexer->source, -1, SEEK_CUR);
        init_token(&token, lexer, Invalid, 1); 
        return token;
    }

    if (c == 'e') {
        c = fgetc(lexer->source);
        if (c == 'p') {
            c = fgetc(lexer->source);
            if (c == 's') {
                init_token(&token, lexer, Eps, 3);
                return token;
            }
            
            fseek(lexer->source, -2, SEEK_CUR);
            init_token(&token, lexer, Invalid, 1);
            return token;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        init_token(&token, lexer, Invalid, 1);
        return token;
    }

    if (isalpha(c) && isupper(c)) {
        fseek(lexer->source, -1, SEEK_CUR);
        token = lexer_read_Id(lexer);
        return token;
    }

    if (c == '"') {
        token = lexer_read_Terminal(lexer);
        return token;
    }

    init_token(&token, lexer,  Invalid, 1);
    return token;
}

void parse(FILE *source) {
    lexer_t lexer;
    init_lexer(&lexer, source);

    token_t token;
    while (true) {
        token = lexer_next_token(&lexer);

        printf("Token type: %d\n", token.type);
        printf("Token row: %zd, col: %zd, pos: %zd\n", token.row, token.col, token.pos);
        printf("Token value: %s\n", token.value);

        if (token.value != NULL) {
            free(token.value);
        }

        if (token.type == Eof || token.type == Invalid) {
            break;
        }
    }
}
