#include "parser.h"

#include "grammar.h"
#include "lexer.h"
#include "error.h"
#include "datastructs/linked_list.h"

#include <stdio.h>

void test_lexer(const char *filename, const char *source, size_t source_length) {
    Lexer lexer = new_Lexer(filename, source, source_length);
    Token token;
    while ((token = Lexer_next_token(&lexer)), token.type != T_EOF) {
        printf("%s(%s, len: %d, pos: %d[%d:%d])\n", Symbol_to_string(token.type), token.value, token.length, token.loc.pos, token.loc.row, token.loc.col);
        free(token.value);
    }
    printf("%s(%s, len: %d, pos: %d[%d:%d])\n", Symbol_to_string(token.type), token.value, token.length, token.loc.pos, token.loc.row, token.loc.col);
    free(token.value);
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

bool parse(const char *filename, const char *source, size_t source_length) {
    Lexer lexer = new_Lexer(filename, source, source_length);
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
