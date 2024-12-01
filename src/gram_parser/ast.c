#include "ast.h"

#include <stdbool.h>
#include <stdlib.h>

#include "datastructs/linked_list.h"
#include "datastructs/string.h"
#include "gram_parser/grammar.h"

bool is_ast_type_supported(Symbol type) {
    switch (type) {
    case Epsillon:
    case T_Arrow:
    case T_Invalid:
    case T_EOF:
    case T_Semicolon:
        return false;

    default:
        return true;
    }
}

size_t get_ast_node_size(Symbol type) {
    switch (type) {
    case NT_Rules: 
        return sizeof(ASTRules);

    case NT_RulesPrime: 
        return sizeof(ASTRulesPrime);

    case NT_Rule: 
        return sizeof(ASTRule);

    case NT_LHS: 
        return sizeof(ASTLHS);

    case NT_RHS: 
        return sizeof(ASTRHS);

    case NT_Items: 
        return sizeof(ASTItems);

    case NT_ItemsPrime: 
        return sizeof(ASTItemsPrime);

    case NT_Item: 
        return sizeof(ASTItem);

    case T_NonTerminal: 
        return sizeof(ASTNodeValue);

    case T_Eps: 
        return sizeof(ASTNode);

    case T_TerminalLiteral: 
        return sizeof(ASTNodeValue);

    default:
        return 0;
    }
};

ASTNode *create_ast_node(Symbol type) {
    if (!is_ast_type_supported(type)) {
        return NULL;
    }

    ASTNode *ast_node = (ASTNode *)malloc(get_ast_node_size(type));
    ast_node->type = type;

    return ast_node;
}

void free_ast(ASTNode *ast_root) {
    void *stack = new_linked_list(sizeof(ASTNode *));
    linked_list_push(stack, &ast_root);

    ASTNode *ast_node;
    while (linked_list_len(stack) > 0) {
        linked_list_pop(stack, &ast_node);
        
        if (ast_node->type == T_TerminalLiteral || ast_node->type == T_NonTerminal) {
            ASTNodeValue *ast_node_value = (ASTNodeValue *)ast_node;
            free_string(ast_node_value->value);

            free(ast_node_value);
            continue;
        }

        size_t ast_node_size = get_ast_node_size(ast_node->type);
        size_t ast_node_refs = (ast_node_size - sizeof(ASTNode)) / sizeof(ASTNode *);

        ASTNode *ast_node_ptr = (ASTNode *)((char *)ast_node + sizeof(Symbol));

        for (size_t i = 0; i < ast_node_refs; i += 1) {
            ASTNode *ast_node_to_remove = ast_node_ptr + i;
            linked_list_push(stack, &ast_node_to_remove);
        }

        free(ast_node);
    }
}
