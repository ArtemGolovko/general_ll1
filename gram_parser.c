#include "gram_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "datastructs/string.h"

// Grammar and todo table

typedef enum {
    Epsillon,
    NT_Rules,
    NT_RulesPrime,
    NT_Rule,
    NT_LHS,
    NT_RHS,
    NT_Items,
    NT_ItemsPrime,
    NT_Item,
    T_Invalid,
    T_EOF,
    T_NonTerminal,
    T_Arrow,
    T_Eps,
    T_TerminalLiteral,
    T_Semicolon
} Symbol;

typedef struct {
    Symbol lhs;
    Symbol rhs[4];
    size_t rhs_length;
} Rule;

static const Rule grammar[] = {
    {
        NT_Rules,
        { NT_Rule, NT_RulesPrime },
        2,
    },
    {
        NT_RulesPrime,
        { NT_Rules },
        1
    },
    {
        NT_RulesPrime,
        { Epsillon },
        1
    },
    {
        NT_Rule,
        { NT_LHS, T_Arrow, NT_RHS, T_Semicolon },
        4
    },
    {
        NT_LHS,
        { T_NonTerminal },
        1
    },
    {
        NT_RHS,
        { T_Eps },
        1
    },
    {
        NT_RHS,
        { NT_Items },
        1
    },
    {
        NT_Items,
        { NT_Item, NT_ItemsPrime },
        2
    },
    {
        NT_ItemsPrime,
        { NT_Items },
        1
    },
    {
        NT_ItemsPrime,
        { Epsillon },
        1
    },
    {
        NT_Item,
        { T_NonTerminal },
        1
    },
    {
        NT_Item,
        { T_TerminalLiteral },
        1
    }
};

static const Rule* todo_table[] = {
    NULL,        &grammar[0],  NULL, NULL,        NULL,         NULL,
    &grammar[2], &grammar[1],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[3],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[4],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[6],  NULL, &grammar[5], &grammar[6],  NULL,
    NULL,        &grammar[7],  NULL, NULL,        &grammar[7],  NULL,
    NULL,        &grammar[8],  NULL, NULL,        &grammar[8],  &grammar[9],
    NULL,        &grammar[10], NULL, NULL,        &grammar[11], NULL,
};

bool is_NonTerminal(Symbol symbol) {
    return symbol > Epsillon && symbol < T_Invalid;
}

bool is_Terminal(Symbol symbol) {
    return symbol >= T_Invalid;
}

const Rule *todo_table_get(Symbol nonterm, Symbol term) {
    assert(is_NonTerminal(nonterm));
    assert(is_Terminal(term));

    if (term == T_Invalid) {
        return NULL;
    }
    
    size_t index = (nonterm - NT_Rules) * 5 + (term - T_EOF);
    return todo_table[index];
}

// Lexer

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
    const char *filename;
} Token;

Token new_Token(Lexer *lexer, Symbol type, size_t length) {
    Token token = {
        type,
        ftell(lexer->source) - length,
        length,
        NULL,
        lexer->filename
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
        lexer->filename
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

        if (isspace(c) || c == EOF || c == '-' || c == ';' || c == '"') {
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

    fseek(lexer->source, -1, SEEK_CUR);

    return new_Token_with_value(lexer, T_NonTerminal, value);
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
            return new_Token(lexer, T_Arrow, 2);
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

    return Lexer_capture_invalid(lexer);
}

void parse(const char *filename, FILE *source) {
    Lexer lexer = new_Lexer(filename, source);
}
