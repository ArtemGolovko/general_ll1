#ifndef ANALYZER_H
#define ANALYZER_H

#include "gram_parser.h"

typedef enum {
    Terminal,
    NonTerminal,
    Epsillon
} symbol_type_t;

typedef struct {
    size_t id;
    symbol_type_t type;
    char *value;
} symbol_t;

typedef struct {
    /** vector */
    symbol_t *symbols;

    /**
     * Indicates start of terminals sector
     *
     * There is no indicator for start of 
     * non terminals sector, beacose it is
     * starts at 1
     */
    size_t terminals_index;
} symbolic_table_t;

typedef struct {
    symbolic_table_t symbolic_table;
} analysis_result_t;

void free_symbolic_table(symbolic_table_t *symbolic_table);
void display_symbolic_table(symbolic_table_t *table);
analysis_result_t analyze(ast_rules_t *ast_root);

#endif
