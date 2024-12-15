#pragma once

#include "gram_parser/ast.h"

typedef enum {
    EpsillonType,
    NonTerminal,
    Terminal
} SymbolType;

typedef struct {
    size_t id;
    SymbolType type;
    char *name;
} SymbolRecord;

typedef struct {
    size_t terminal_id;
    char *value;
} TerminalValueRecord;

typedef struct {
    /** vector */
    SymbolRecord *symbols;

    /** vector */
    TerminalValueRecord *terminal_values;

    /** ids of important symbols */
    size_t start_non_termianl_id;
    size_t epsillon_id;
    size_t eof_id;
    size_t invalid_id;
} SymbolicTable;

SymbolicTable build_SymbolicTable(ASTRules *ast_root);
void free_SymbolicTable(SymbolicTable *table);

void sort_SybolicTable(SymbolicTable *table);
void unsort_SybolicTable(SymbolicTable *table);

SymbolRecord *find_symbol_by_name(const char *name);
SymbolRecord *find_terminal_by_value(const char *value);

