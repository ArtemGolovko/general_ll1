#include "analyzer.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


symbolic_table_t construct_symbolic_table(ast_rules_t *ast_root) {
    
    symbolic_table_t symbolic_table = { 0 };
    symbolic_table.non_terminals = (char **)calloc(ast_root->length, sizeof(char *));

    ast_rule_t rule;
    bool match_found;
    size_t max_terminals_length = 0;

    for (size_t i = 0; i < ast_root->length; i += 1) {
        rule = ast_root->rules[i];
        match_found = false;

        if (max_terminals_length < rule.production->length) {
            max_terminals_length = rule.production->length;
        }

        for (size_t j = 0; j < symbolic_table.non_terminals_length; j += 1) {
            if (strcmp(rule.name, symbolic_table.non_terminals[j]) == 0) {
                match_found = true;
                break;
            }
        }

        if (match_found) {
            continue;
        }

        size_t name_length = strlen(rule.name) + 1;
        symbolic_table.non_terminals[symbolic_table.non_terminals_length] = (char *)calloc(name_length, sizeof(char));
        strcpy_s(symbolic_table.non_terminals[symbolic_table.non_terminals_length], name_length, rule.name);
        symbolic_table.non_terminals_length += 1; 
    }


    symbolic_table.terminals = (char **)calloc(max_terminals_length, sizeof(char *));
    
    ast_production_t *production;
    ast_item_t item;

    for (size_t i = 0; i < ast_root->length; i += 1) {
        production = ast_root->rules[i].production;

        for (size_t j = 0; j < production->length; j += 1) {
            item = production->items[j];
            
            match_found = false;
            if (item.type != ASTItemTerminal) {
                continue;
            }

            for (size_t k = 0; k < symbolic_table.terminals_length; k += 1) {
                if (strcmp(item.value, symbolic_table.terminals[k]) == 0) {
                    match_found = true;
                    break;
                }
            }

            if (match_found) {
                continue;
            }

            size_t value_length = strlen(item.value) + 1;
            symbolic_table.terminals[symbolic_table.terminals_length] = (char *)calloc(value_length, sizeof(char));
            strcpy_s(symbolic_table.terminals[symbolic_table.terminals_length], value_length, item.value);
            symbolic_table.terminals_length += 1;
        }
    }

    return symbolic_table;
}

void free_symbolic_table(symbolic_table_t *symbolic_table) {
    size_t i;
    for (i = 0; i < symbolic_table->terminals_length; i += 1) {
        free(symbolic_table->terminals[i]);
    }
    
    for (i = 0; i < symbolic_table->non_terminals_length; i += 1) {
        free(symbolic_table->non_terminals[i]);
    }

    free(symbolic_table->terminals);
    free(symbolic_table->non_terminals);
}

analysis_result_t analyze(ast_rules_t *ast_root) {
    symbolic_table_t symbolic_table = construct_symbolic_table(ast_root);
    analysis_result_t result = { symbolic_table };

    return result;
}

