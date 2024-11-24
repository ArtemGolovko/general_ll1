#include "parser.h"

#include <string.h>

#include "grammar.h"
#include "lexer.h"
#include "gram_parser/error.h"

#include "datastructs/string.h"
#include "datastructs/vector.h"
#include "datastructs/linked_list.h"



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

SyntaxError create_not_matching_terminal_error(Token *token, Lexer *lexer) {
    SyntaxError error = {
        ParserError,
        strdup("not_matching_terminal_error"), // TODO: improve error messages
        token->length,
        token->loc,
        lexer->source,
        lexer->source_length
    };

    return error;
}

SyntaxError create_not_matching_rule_error(Token *token, Lexer *lexer) {
    SyntaxError error = {
        ParserError,
        strdup("not_matching_rule_error"), // TODO: improve error messages
        token->length,
        token->loc,
        lexer->source,
        lexer->source_length
    };

    return error;
}

ParsingResult parse(const char *filename, const char *source, size_t source_length) {
    Lexer lexer = new_Lexer(filename, source, source_length);
    void *stack = new_linked_list(sizeof(StackItem));
    
    stack_push(stack, T_EOF);
    stack_push(stack, NT_Rules);

    Token token = Lexer_next_token(&lexer);
    SyntaxError *errors = (SyntaxError *)new_vector(sizeof(SyntaxError), 0);

    while (!stack_is_empty(stack)) {
        StackItem top = stack_pop(stack);

        if (is_Terminal(top.type)) {
            if (top.type != token.type) {
                if (token.type == T_Invalid) {
                    vector_push((void **)&errors, &lexer.last_error);
                }

                SyntaxError error = create_not_matching_terminal_error(&token, &lexer);
                vector_push((void **)&errors, &error);

                stack_push(stack, top.type);

                free_string(token.value); // TODO: Remove when adding ast
                token = Lexer_next_token(&lexer);
                continue;
            }

            
            free_string(token.value); // TODO: Remove when adding ast
            token = Lexer_next_token(&lexer);
            continue;
        }

        if (is_NonTerminal(top.type)) {
            const Rule *rule = todo_table_get(top.type, token.type);
            
            if (rule == NULL) {
                if (token.type == T_Invalid) {
                    vector_push((void **)&errors, &lexer.last_error);
                }

                SyntaxError error = create_not_matching_rule_error(&token, &lexer);
                vector_push((void **)&errors, &error);

                stack_push(stack, top.type);

                free_string(token.value); // TODO: Remove when adding ast
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

    ParsingResult result = {
        NULL,
        errors
    };

    return result;
}

