#include "gram_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "datastructs/string.h"
#include "datastructs/linked_list.h"

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
    
    size_t index = (nonterm - NT_Rules) * 6 + (term - T_EOF);
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
} Token;

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

const char *Symbol_to_string(Symbol symbol) {
    switch (symbol) {
        case Epsillon: return "Epsillon";
        case NT_Rules: return "NT_Rules";
        case NT_RulesPrime: return "NT_RulesPrime";
        case NT_Rule: return "NT_Rule";
        case NT_LHS: return "NT_LHS";
        case NT_RHS: return "NT_RHS";
        case NT_Items: return "NT_Items";
        case NT_ItemsPrime: return "NT_ItemsPrime";
        case NT_Item: return "NT_Item";
        case T_Invalid: return "T_Invalid";
        case T_EOF: return "T_EOF";
        case T_NonTerminal: return "T_NonTerminal";
        case T_Arrow: return "T_Arrow";
        case T_Eps: return "T_Eps";
        case T_TerminalLiteral: return "T_TerminalLiteral";
        case T_Semicolon: return "T_Semicolon";
    }
}

void Token_print(Token *token) {
    printf("Token of type %s at poition %zd with ", Symbol_to_string(token->type), token->pos);
    if (token->value != NULL) {
        printf("value: %s", token->value);
    } else {
        printf("no value");
    }
    printf(". Length: %zd.\n", token->length);
}

typedef struct {
    Symbol type;
} StackItem;

void stack_push(void *stack, Symbol type) {
    StackItem item = {
        type
    };

    linked_list_push(stack, &item);
}

StackItem stack_pop(void *stack) {
    StackItem item;
    linked_list_pop(stack, &item);

    return item;
}

bool stack_is_empty(void *stack) {
    return linked_list_len(stack) == 0;
}
// displaying errors

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

bool parse(const char *filename, FILE *source) {
    Lexer lexer = new_Lexer(filename, source);
    void *stack = new_linked_list(sizeof(StackItem));
    
    stack_push(stack, T_EOF);
    stack_push(stack, NT_Rules);

    Token token = Lexer_next_token(&lexer);
    bool accepted = true;

    while (!stack_is_empty(stack)) {
        StackItem top = stack_pop(stack);

        if (is_Terminal(top.type)) {
            if (top.type != token.type) {
                accepted = false;
                display_not_matching_terminal_error(&lexer, top.type, &token);

                stack_push(stack, top.type);
                token = Lexer_next_token(&lexer);
                continue;
            }

            
            token = Lexer_next_token(&lexer);
            continue;
        }

        if (is_NonTerminal(top.type)) {
            const Rule *rule = todo_table_get(top.type, token.type);
            
            if (rule == NULL) {
                accepted = false;
                display_not_matching_rule_error(&lexer, top.type, &token);

                stack_push(stack, top.type);
                token = Lexer_next_token(&lexer);
                continue;
            }

            for (size_t i = rule->rhs_length; i > 0; i -= 1) {
                stack_push(stack, rule->rhs[i - 1]);
            }
            continue;
        }

        if (top.type == Epsillon) {
            // Left for ast constuction
            continue;
        }
    }
    free_linked_list(stack);

    return accepted;
}
