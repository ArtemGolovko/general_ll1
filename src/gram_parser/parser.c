#include "parser.h"

#include <string.h>

#include "gram_parser/grammar.h"
#include "gram_parser/lexer.h"
#include "gram_parser/error.h"
#include "gram_parser/ast.h"

#include "datastructs/string.h"
#include "datastructs/vector.h"
#include "datastructs/linked_list.h"

typedef struct {
    Symbol type;
    ASTNode *ast_node;
} StackItem;

void stack_push(void *stack, Symbol type, ASTNode *ast_node) {
    StackItem item = {
        type,
        ast_node
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
        strdup("Unexpected token"),
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
        strdup("Invalid syntax"),
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
    
    ASTRules *ast_root = (ASTRules *)create_ast_node(NT_Rules);

    stack_push(stack, T_EOF, NULL);
    stack_push(stack, NT_Rules, (ASTNode *)ast_root);

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

                stack_push(stack, top.type, top.ast_node);

                free_string(token.value);
                token = Lexer_next_token(&lexer);
                continue;
            }

            if (top.ast_node != NULL && is_ast_value_type(token.type)) {
                ((ASTNodeValue *)top.ast_node)->value = token.value;
            }

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

                stack_push(stack, top.type, top.ast_node);

                free_string(token.value);
                token = Lexer_next_token(&lexer);
                continue;
            }
            

            size_t ast_node_size = get_ast_node_size(top.ast_node->type);
            size_t index = (ast_node_size - sizeof(ASTNode)) / sizeof(ASTNode *) - 1;

            for (size_t i = rule->rhs_length; i > 0; i -= 1) {
                if (top.ast_node != NULL && is_ast_type_supported(rule->rhs[i - 1])) {
                    AST_NTH_SUBNODE(top.ast_node, index) = create_ast_node(rule->rhs[i - 1]);
                    stack_push(stack, rule->rhs[i - 1], AST_NTH_SUBNODE(top.ast_node, index));
                    index -= 1;
                    continue;
                }
                stack_push(stack, rule->rhs[i - 1], NULL);
            }
            continue;
        }
    }

    free_linked_list(stack);

    ParsingResult result = {
        ast_root,
        errors
    };

    return result;
}

