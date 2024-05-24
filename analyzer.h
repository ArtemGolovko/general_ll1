#ifndef ANALYZER_H
#define ANALYZER_H

#include "gram_parser.h"

typedef struct {
    char **non_terminals;
    size_t non_terminals_length;

    char **terminals;
    size_t terminals_length;
} symbolic_table_t;

typedef struct {
    symbolic_table_t symbolic_table;
} analysis_result_t;

void free_symbolic_table(symbolic_table_t *symbolic_table);
analysis_result_t analyze(ast_rules_t *ast_root);

#endif
