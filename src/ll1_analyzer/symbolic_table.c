#include "symbolic_table.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"
#include "datastructs/linked_list.h"
#include "datastructs/vector.h"
#include "gram_parser/ast.h"
#include "gram_parser/grammar.h"
#include "ll1_analyzer/terminal_literal_name.h"


SymbolicTable build_SymbolicTable(ASTRules *ast_root) {
    void *items_stack = new_linked_list(sizeof(ASTItems *));

    SymbolicTable table = {
        new_vector(sizeof(SymbolRecord), 20),
        new_vector(sizeof(TerminalValueRecord), 10),
        1,
        0,
        0,
        0
    };

    SymbolRecord epsillon_record = {
        0,
        EpsillonType,
        strdup("_Epsillon")
    };

    vector_push((void **)&table.symbols, &epsillon_record);
    
    size_t id = 1;
    ASTRules *rules = ast_root;

    while (rules != NULL) {
        const char *name = rules->rule->lhs->non_terminal->value; 
        ASTNode *ast_items = rules->rule->rhs->items_or_eps; 

        if (ast_items->type != T_Eps) {
            linked_list_push(items_stack, &ast_items);
        }

        if (find_symbol_by_name(&table, name) == NULL) {
            SymbolRecord record = {
                id,
                NonTerminal,
                strdup(name)
            };
            vector_push((void **)&table.symbols, &record);
            id += 1;
        }
        
        rules = rules->rules_prime->rules;
    }
    
    SymbolRecord eof_record = {
        id,
        Terminal,
        strdup("_eof")
    };
    vector_push((void **)&table.symbols, &eof_record);
    table.eof_id = id;
    id += 1;

    SymbolRecord invalid_record = {
        id,
        Terminal,
        strdup("_invalid")
    };
    vector_push((void **)&table.symbols, &invalid_record);
    table.invalid_id = id;
    id += 1;

    ASTItems* items;

    while (linked_list_len(items_stack) != 0) {
        linked_list_pop(items_stack, &items);

        while (items != NULL) {
            ASTNodeValue *item_value = items->item->non_terminal_or_terminal_literal;
            const char *value = item_value->value;

            if (item_value->type == T_TerminalLiteral && find_terminal_by_value(&table, value) == NULL) {
                SymbolRecord record = {
                    id,
                    Terminal,
                    create_terminal_literal_name(id)
                };
                vector_push((void **)&table.symbols, &record);

                TerminalValueRecord terminal_record = {
                    id,
                    strdup(value)
                };
                vector_push((void **)&table.terminal_values, &terminal_record);

                id += 1;
            }

            items = items->items_prime->items;
        }
    }

    free_linked_list(items_stack);

    return table;
}

void free_SymbolicTable(SymbolicTable *table) {
    for (size_t i = 0; i < vector_len(table->symbols); i += 1) {
        SymbolRecord record = table->symbols[i];
        free(record.name);
    }

    for (size_t i = 0; i < vector_len(table->terminal_values); i += 1) {
        TerminalValueRecord record = table->terminal_values[i];
        free(record.value);
    }

    free_vector(table->symbols);
    free_vector(table->terminal_values);
}

int compare_SymbolRecord_id(const void *_a, const void *_b) {
    const SymbolRecord *a = _a;
    const SymbolRecord *b = _b;

    return a->id - b->id; 
}

int compare_SymbolRecord_name(const void *_a, const void *_b) {
    const SymbolRecord *a = _a;
    const SymbolRecord *b = _b;

    return strcmp(a->name, b->name); 
}

int compare_TerminalValueRecord_terminal_id(const void *_a, const void *_b) {
    const TerminalValueRecord *a = _a;
    const TerminalValueRecord *b = _b;

    return a->terminal_id - b->terminal_id;
}

int compare_TerminalValueRecord_value(const void *_a, const void *_b) {
    const TerminalValueRecord *a = _a;
    const TerminalValueRecord *b = _b;

    return strcmp(a->value, b->value);
}

void sort_SybolicTable(SymbolicTable *table) {
    qsort(table->symbols, vector_len(table->symbols), sizeof(SymbolRecord), compare_SymbolRecord_name);
    qsort(table->terminal_values, vector_len(table->terminal_values), sizeof(TerminalValueRecord), compare_TerminalValueRecord_value);
};

void unsort_SybolicTable(SymbolicTable *table) {
    qsort(table->symbols, vector_len(table->symbols), sizeof(SymbolRecord), compare_SymbolRecord_id);
    qsort(table->terminal_values, vector_len(table->terminal_values), sizeof(TerminalValueRecord), compare_TerminalValueRecord_terminal_id);
};

const SymbolRecord *find_symbol_by_id(const SymbolicTable *table, size_t id) {
    size_t symbols_len = vector_len(table->symbols);

    bool is_sorted = sorted(table->symbols, symbols_len, sizeof(SymbolRecord), compare_SymbolRecord_id); 

    SymbolRecord search_record = { id, EpsillonType, NULL };

    if (is_sorted) {
        return &table->symbols[id];
    }

    return _lfind(&search_record, table->symbols, &symbols_len, sizeof(SymbolRecord), compare_SymbolRecord_id);
}

const SymbolRecord *find_symbol_by_name(const SymbolicTable *table, const char *name) {
    size_t symbols_len = vector_len(table->symbols);
    bool is_sorted = sorted(table->symbols, symbols_len, sizeof(SymbolRecord), compare_SymbolRecord_name); 
    SymbolRecord search_record = { 0, EpsillonType, name };

    if (is_sorted) {
        return bsearch(&search_record, table->symbols, symbols_len, sizeof(SymbolRecord), compare_SymbolRecord_name);
    }

    return _lfind(&search_record, table->symbols, &symbols_len, sizeof(SymbolRecord), compare_SymbolRecord_name);
}

const TerminalValueRecord *find_terminal_by_terminal_id(const SymbolicTable *table, size_t id) {
    size_t terminal_values_len = vector_len(table->terminal_values);
    bool is_sorted = sorted(table->terminal_values, terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_terminal_id); 
    TerminalValueRecord search_record = { id, NULL };

    if (is_sorted) {
        return bsearch(&search_record, table->terminal_values, terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_terminal_id);
    }

    return _lfind(&search_record, table->terminal_values, &terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_terminal_id);
}

const SymbolRecord *find_terminal_by_value(const SymbolicTable *table, const char *value) {
    size_t terminal_values_len = vector_len(table->terminal_values);
    bool is_sorted = sorted(table->terminal_values, terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_value); 
    TerminalValueRecord search_record = { 0, value };

    if (is_sorted) {
        const TerminalValueRecord *record = bsearch(&search_record, table->terminal_values, terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_value);
        if (record == NULL) {
            return NULL;
        }

        return find_symbol_by_id(table, record->terminal_id);
    }

    const TerminalValueRecord *record = _lfind(&search_record, table->terminal_values, &terminal_values_len, sizeof(TerminalValueRecord), compare_TerminalValueRecord_value);

    if (record == NULL) {
        return NULL;
    }

    return find_symbol_by_id(table, record->terminal_id);
}
