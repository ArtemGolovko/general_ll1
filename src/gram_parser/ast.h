#pragma once

#include <stdbool.h>
#include "grammar.h"

struct ASTNode_s {
    Symbol type;
};

struct ASTNodeValue_s {
    Symbol type;

    /** string */
    char *value;
};

struct ASTRules_s {
    Symbol type;

    struct ASTRule_s *rule;
    struct ASTRulesPrime_s *rules_prime;
};

struct ASTRulesPrime_s {
    Symbol type;

    struct ASTRules_s *rules;
};

struct ASTRule_s {
    Symbol type;

    struct ASTLHS_s *lhs;
    struct ASTRHS_s *rhs;
};

struct ASTLHS_s {
    Symbol type;

    struct ASTNodeValue_s *non_terminal;
};

struct ASTRHS_s {
    Symbol type;

    struct ASTNode_s *items_or_eps;
};

struct ASTItems_s {
    Symbol type;

    struct ASTItem_s *item;
    struct ASTItemsPrime_s *items_prime;
};

struct ASTItemsPrime_s {
    Symbol type;

    struct ASTItems_s *items;
};

struct ASTItem_s {
    Symbol type;
    
    struct ASTNodeValue_s *non_terminal_or_terminal_literal;
};

typedef struct ASTNode_s ASTNode;
typedef struct ASTNodeValue_s ASTNodeValue;
typedef struct ASTRules_s ASTRules;
typedef struct ASTRulesPrime_s ASTRulesPrime;
typedef struct ASTRule_s ASTRule;
typedef struct ASTLHS_s ASTLHS;
typedef struct ASTRHS_s ASTRHS;
typedef struct ASTItems_s ASTItems;
typedef struct ASTItemsPrime_s ASTItemsPrime;
typedef struct ASTItem_s ASTItem;

bool is_ast_type_supported(Symbol type);
size_t get_ast_node_size(Symbol type);
ASTNode *create_ast_node(Symbol type);
void free_ast(ASTNode *ast_root);

#define AST_NTH_SUBNODE(ast_node, n) (*((ASTNode **)((char *)(ast_node) + (((n) + 1) * 8))))
