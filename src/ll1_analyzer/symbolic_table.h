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

    /**
    * The underscore character prefix is used to indicate program definied
    * symbols in symbolic table. It aims to remove chance of name collision
    */
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

/**
 * This function sorts symbols in the symbolic table by name and terminals by value.
 * This allows for faster search by name. However searching by id becomes slower
 */
void sort_SybolicTable(SymbolicTable *table);

/**
 * This function reverts the symbolic table to it's original state.
 * This allows for instant access by id
 */
void unsort_SybolicTable(SymbolicTable *table);

const SymbolRecord *find_symbol_by_id(const SymbolicTable *table, size_t id);
const SymbolRecord *find_symbol_by_name(const SymbolicTable *table, const char *name);

const TerminalValueRecord *find_termianl_value_by_id(const SymbolicTable *table, size_t id);
const SymbolRecord *find_terminal_by_value(const SymbolicTable *table, const char *value);

