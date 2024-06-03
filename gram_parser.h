#ifndef GRAM_PARSER_H
#define GRAM_PARSER_H

#include <stdio.h>

typedef enum {
    ASTItemTerminal,
    ASTItemNonterminal,
    ASTItemEpsillon
} ast_item_type_t;

typedef struct {
    ast_item_type_t type;

    /** string */
    char *value;
} ast_item_t;

typedef struct {
    /** vector */
    ast_item_t *items;
} ast_production_t;

typedef struct {
    /** string */
    char *name;

    ast_production_t *production;
} ast_rule_t;

typedef struct {
    /** vector */
    ast_rule_t *rules;    
} ast_rules_t;

void free_AST(ast_rules_t *ast_root);
ast_rules_t *parse(FILE *source);

#endif
