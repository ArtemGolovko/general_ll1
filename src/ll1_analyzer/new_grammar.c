#include "new_grammar.h"

#include <stdio.h>

#include "datastructs/vector.h"
#include "gram_parser/ast.h"
#include "gram_parser/grammar.h"
#include "ll1_analyzer/symbolic_table.h"

Grammar build_grammar(ASTRules *ast_root, SymbolicTable *table) {
    sort_SybolicTable(table);

    Grammar new_grammar = new_vector(sizeof(GrammarRule), 10);

    ASTRules *rules = ast_root;

    while (rules != NULL) {
        const char *name = rules->rule->lhs->non_terminal->value; 
        ASTNode *ast_items = rules->rule->rhs->items_or_eps; 
    
        const SymbolRecord *symbol = find_symbol_by_name(table, name);
        GrammarRule grammar_rule = { symbol->id, new_vector(sizeof(size_t), 10) };

        if (ast_items->type == T_Eps) {
            vector_push((void **)&grammar_rule.rhs, &table->epsillon_id);
        } else {
            ASTItems *items = ast_items; 

            while (items != NULL) {
                ASTNodeValue *items_value = items->item->non_terminal_or_terminal_literal;
                const SymbolRecord *rhs_symbol;
                
                if (items_value->type == T_NonTerminal) {
                    rhs_symbol = find_symbol_by_name(table, items_value->value);
                } else {
                    rhs_symbol = find_terminal_by_value(table, items_value->value);
                }

                if (rhs_symbol == NULL) {
                    perror("TODO: Add error handling to new_grammar.h");
                    free_vector(grammar_rule.rhs);
                    free_grammar(new_grammar);
                    return NULL;
                }

                vector_push((void **)&grammar_rule.rhs, &rhs_symbol->id);

                items = items->items_prime->items;
            }
        }

        vector_push((void **)&new_grammar, &grammar_rule);

        rules = rules->rules_prime->rules;
    }

    return new_grammar;
}


void free_grammar(Grammar grammar) {
    for (size_t i = 0; i < vector_len(grammar); i += 1) {
        free_vector(grammar[i].rhs);
    }

    free_vector(grammar);
}
