#include "analyzer.h"

#include <stdio.h>

#include "datastructs/vector.h"
#include "ll1_analyzer/symbolic_table.h"

void analyze_ll1(ASTRules *ast_root) {
    SymbolicTable table = build_SymbolicTable(ast_root);
    
    printf(" Id | Type        | Name\n");
    for (size_t i = 0; i < vector_len(table.symbols); i += 1) {
        SymbolRecord record = table.symbols[i];
        
        printf(" %zd | ", record.id);

        switch (record.type) {
            case EpsillonType:
                printf("Epsillon   ");
                break;

            case NonTerminal:
                printf("NonTerminal");
                break;
        
            case Terminal:
                printf("Terminal   ");
                break;
        }

        printf(" | %s\n", record.name);
    }


    free_SymbolicTable(&table);
}
