#include "analyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "datastructs/vector.h"
#include "gram_parser.h"

void symbolic_table_push_termianl(symbolic_table_t *table, const char *terminal) {
    symbol_t symbol = {
        vector_len(table->symbols),
        Terminal,
        (char *)calloc(strlen(terminal) + 1, sizeof(char))
    };

    strcpy(symbol.value, terminal); 

    vector_push((void **)&table->symbols, &symbol);
}

void symbolic_table_push_non_termial(symbolic_table_t *table, const char *non_terminal) {
    symbol_t symbol = {
        vector_len(table->symbols),
        NonTerminal,
        (char *)calloc(strlen(non_terminal) + 1, sizeof(char))
    };

    strcpy(symbol.value, non_terminal); 

    vector_push((void **)&table->symbols, &symbol);
}

size_t symbolic_table_find_non_terminal(symbolic_table_t *table, const char *non_terminal) {
    size_t stop = table->terminals_index;
    if (stop == 0) {
        stop = vector_len(table->symbols);
    }

    size_t index = 0;
    for (size_t i = 1; i < stop; i += 1) {
        symbol_t symbol = table->symbols[i];

        if (strcmp(symbol.value, non_terminal) == 0) {
            index = i;
            break;
        }
    }

    return index;
}

size_t symbolic_table_find_terminal(symbolic_table_t *table, const char *terminal) {
    assert(table->terminals_index != 0);
    
    size_t index = 0;

    for (size_t i = table->terminals_index + 1; i < vector_len(table->symbols); i += 1) {
        symbol_t symbol = table->symbols[i];

        if (strcmp(symbol.value, terminal) == 0) {
            index = i;
            break;
        }
    }

    return index;
}

symbolic_table_t construct_symbolic_table(ast_rules_t *ast_root) {
    symbolic_table_t table = {
        new_vector(sizeof(symbol_t), vector_len(ast_root->rules)),
        0
    };


    symbol_t epsillon_symbol = {
        vector_len(table.symbols),
        Epsillon,
        NULL
    };

    vector_push((void **)&table.symbols, &epsillon_symbol);

    for (size_t i = 0; i < vector_len(ast_root->rules); i += 1) {
        ast_rule_t rule = ast_root->rules[i];
        if (symbolic_table_find_non_terminal(&table, rule.name) == 0) {
            symbolic_table_push_non_termial(&table, rule.name);
        }
    }
    
    table.terminals_index = vector_len(table.symbols);
    symbol_t eof_symbol = {
        vector_len(table.symbols),
        Terminal,
        NULL
    };

    vector_push((void **)&table.symbols, &eof_symbol);

    for (size_t i = 0; i < vector_len(ast_root->rules); i += 1) {
        ast_production_t *production = ast_root->rules[i].production;
        for (size_t j = 0; j < vector_len(production->items); j += 1) {
            ast_item_t item = production->items[j];
            switch (item.type) {
                case ASTItemEpsillon: continue;
                case ASTItemNonterminal: {
                    if (symbolic_table_find_non_terminal(&table, item.value) == 0) {
                        fprintf(stderr, "Nonterminal %s in not defined", item.value);
                        exit(1);
                    }
                    break;
                }
                case ASTItemTerminal: {
                    if (symbolic_table_find_terminal(&table, item.value) == 0) {
                        symbolic_table_push_termianl(&table, item.value);
                    }
                    break;
                }
            }
        }
    }

    return table;
}

void display_symbolic_table(symbolic_table_t *table) {
    printf("Id\tType\t\tValue\tExtra\n\n");
    
    for (size_t i = 0; i < vector_len(table->symbols); i += 1) {
        symbol_t symbol = table->symbols[i];
        char *extra = NULL;

        if (symbol.type == Epsillon) {
            extra = "Resembles empty production";
        }

        if (symbol.type == Terminal && symbol.value == NULL) {
            extra = "End Of File indicator";
        }

        char *type;

        switch (symbol.type) {
            case Terminal:
                type = "Terminal";
                break;
            case NonTerminal:
                type = "NonTerminal";
                break;
            case Epsillon:
                type = "Epsillon";
                break;
        }

        printf("%zd\t%s\t%s\t%s\n", symbol.id, type, symbol.value, extra);
    }
}

void free_symbolic_table(symbolic_table_t *symbolic_table) {
    for (size_t i = 0; i < vector_len(symbolic_table->symbols); i += 1) {
        symbol_t symbol = symbolic_table->symbols[i];
        free(symbol.value);
    }

    free_vector(symbolic_table->symbols);
}

analysis_result_t analyze(ast_rules_t *ast_root) {
    symbolic_table_t symbolic_table = construct_symbolic_table(ast_root);
    analysis_result_t result = { symbolic_table };

    return result;
}

