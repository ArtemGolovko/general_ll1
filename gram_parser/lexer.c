#include "lexer.h"

#include <string.h>
#include <ctype.h>
#include "../datastructs/string.h"

Token new_Token(Lexer *lexer, Symbol type, size_t length) {
    Token token = {
        type,
        ftell(lexer->source) - length,
        length,
        NULL,
    };

    return token;
}

// Moves ownership of value to the token 
Token new_Token_with_value(Lexer *lexer, Symbol type, char *value) {
    size_t length = strlen(value);
    Token token = {
        type,
        ftell(lexer->source) - length,
        length,
        value,
    };

    return token;
}

Lexer new_Lexer(const char *filename, FILE *source) {
    Lexer lexer = {
        filename,
        source
    };

    return lexer;
}

void Lexer_skip_mulitline_comment(Lexer *lexer) {
    bool is_prev_star = false;

    while (!feof(lexer->source)) {
        char c = fgetc(lexer->source);

        if (is_prev_star && c == '/') {
            return;
        }

        is_prev_star = c == '*';
    }
}

void Lexer_skip_singleline_comment(Lexer *lexer) {
    while (!feof(lexer->source)) {
        char c = fgetc(lexer->source);

        if (c == '\n') {
            return;
        }
    }
}

void Lexer_skip_whitespace_and_comments(Lexer *lexer) {
    while (!feof(lexer->source)) {
        char c = fgetc(lexer->source);

        if (isspace(c) || c == EOF) {
            continue;
        }

        if (c == '/') {
            c = fgetc(lexer->source);
            if (c == '*') {
                Lexer_skip_mulitline_comment(lexer);
                continue;
            }

            if (c == '/') {
                Lexer_skip_singleline_comment(lexer);
                continue;
            }

            fseek(lexer->source, -2, SEEK_CUR);
            return;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        return;
    }
}

Token Lexer_capture_invalid(Lexer *lexer) {
    char c = fgetc(lexer->source);
    size_t length = 1;

    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);

        if (
               isspace(c) 
            || c == EOF
            || c == '-'
            || c == ';'
            || c == '"'
            || (isalpha(c) && isupper(c))
        ) {
            fseek(lexer->source, -1, SEEK_CUR);
            return new_Token(lexer, T_Invalid, length);
        }

        length += 1;
    }

    return new_Token(lexer, T_Invalid, length);
}

Token Lexer_read_NonTerminal(Lexer *lexer) {
    char c = fgetc(lexer->source);
    char *value = new_string(1);

    string_push(&value, c);

    while (!feof(lexer->source) && c != EOF) {
        c = fgetc(lexer->source);
        if (!isalpha(c) && c != '_') {
            break;
        }
        
        string_push(&value, c);
    }

    Token token = new_Token_with_value(lexer, T_NonTerminal, value);
    fseek(lexer->source, -1, SEEK_CUR);

    return token;
}

Token Lexer_read_TerminalLiteral(Lexer *lexer) {
    char c = 0;
    bool is_prev_backslash = false;
    char *value = new_string(0);

    while (!feof(lexer->source) && c != EOF) {
        c = fgetc(lexer->source);

        if (!is_prev_backslash && c == '"') {
            return new_Token_with_value(lexer, T_TerminalLiteral, value);
        }

        is_prev_backslash = c == '\\';

        string_push(&value, c);
    }

    Token token = new_Token(lexer, T_Invalid, string_len(value));
    free_string(value);

    return token;
}

Token Lexer_next_token(Lexer *lexer) {
    Lexer_skip_whitespace_and_comments(lexer);

    if (feof(lexer->source)) {
        return new_Token(lexer, T_EOF, 0); 
    }

    char c = fgetc(lexer->source);

    if (c == ';') {
        return new_Token(lexer, T_Semicolon, 1);
    }

    if (c == '-') {
        c = fgetc(lexer->source);

        if (c == '>') {
            Token token = new_Token(lexer, T_Arrow, 2);
            token.pos -= 1; 
            return token;
        }
        fseek(lexer->source, -1, SEEK_CUR);
        return Lexer_capture_invalid(lexer);
    }

    if (c == 'e') {
        c = fgetc(lexer->source);
        if (c == 'p') {
            c = fgetc(lexer->source);
            if (c == 's') {
                return new_Token(lexer, T_Eps, 3);
            }

            fseek(lexer->source, -2, SEEK_CUR);
            return Lexer_capture_invalid(lexer);
        }

        fseek(lexer->source, -1, SEEK_CUR);
        return Lexer_capture_invalid(lexer);
    }

    if (isalpha(c) && isupper(c)) {
        fseek(lexer->source, -1, SEEK_CUR);
        return Lexer_read_NonTerminal(lexer);
    }

    if (c == '"') {
        return Lexer_read_TerminalLiteral(lexer);
    }

    fseek(lexer->source, -1, SEEK_CUR);
    return Lexer_capture_invalid(lexer);
}

// Debug

void Token_print(Token *token) {
    printf("Token of type %s at poition %zd with ", Symbol_to_string(token->type), token->pos);
    if (token->value != NULL) {
        printf("value: %s", token->value);
    } else {
        printf("no value");
    }
    printf(". Length: %zd.\n", token->length);
}
