#include "symbolic_table.h"

#include "datastructs/linked_list.h"
#include "datastructs/vector.h"
#include "gram_parser/ast.h"
#include "gram_parser/grammar.h"
#include <string.h>

SymbolRecord *linear_search_SymbolicTable(SymbolicTable *table, const char *name) {
    for (size_t i = 0; i < vector_len(table->symbols); i += 1) {
        const char *symbol_name = table->symbols[i].name;
        
        if (symbol_name != NULL && strcmp(symbol_name, name) == 0) {
            return &table->symbols[i];
        }
    }

    return NULL;
}

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
        NULL
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

        if (linear_search_SymbolicTable(&table, name) == NULL) {
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
