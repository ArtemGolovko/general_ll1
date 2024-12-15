#include "symbolic_table.h"

#include <string.h>

#include "datastructs/linked_list.h"
#include "datastructs/vector.h"
#include "gram_parser/ast.h"
#include "gram_parser/grammar.h"
#include "ll1_analyzer/terminal_literal_name.h"

SymbolRecord *linear_search_symbols(SymbolicTable *table, const char *name) {
    for (size_t i = 0; i < vector_len(table->symbols); i += 1) {
        const char *symbol_name = table->symbols[i].name;
        
        if (symbol_name != NULL && strcmp(symbol_name, name) == 0) {
            return &table->symbols[i];
        }
    }

    return NULL;
}

TerminalValueRecord *linear_search_terminal_values(SymbolicTable *table, const char *value) {
    for (size_t i = 0; i < vector_len(table->terminal_values); i += 1) {
        const char *terminal_value = table->terminal_values[i].value;
        
        if (strcmp(terminal_value, value) == 0) {
            return &table->terminal_values[i];
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

        if (linear_search_symbols(&table, name) == NULL) {
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
        strdup("eof")
    };
    vector_push((void **)&table.symbols, &eof_record);
    table.eof_id = id;
    id += 1;

    SymbolRecord invalid_record = {
        id,
        Terminal,
        strdup("invalid")
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

            if (item_value->type == T_TerminalLiteral && linear_search_terminal_values(&table, value) == NULL) {
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
