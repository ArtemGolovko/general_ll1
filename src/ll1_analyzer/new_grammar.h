#pragma once

#include "gram_parser/ast.h"
#include "ll1_analyzer/symbolic_table.h"

typedef struct {

    size_t lhs;

    /** vector */
    size_t *rhs;

} GrammarRule;

/** vector */
typedef GrammarRule * Grammar;


Grammar build_grammar(ASTRules *ast_root, SymbolicTable *table);
void free_grammar(Grammar grammar);
