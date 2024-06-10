#include "gram_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {
    Epsillon,
    NT_Rules,
    NT_RulesPrime,
    NT_Rule,
    NT_LHS,
    NT_RHS,
    NT_Items,
    NT_ItemsPrime,
    NT_Item,
    T_Invalid,
    T_EOF,
    T_NonTerminal,
    T_Arrow,
    T_Eps,
    T_TerminalLiteral,
    T_Semicolon
} Symbol;

typedef struct {
    Symbol lhs;
    Symbol rhs[4];
    size_t rhs_length;
} Rule;

static const Rule grammar[] = {
    {
        NT_Rules,
        { NT_Rule, NT_RulesPrime },
        2,
    },
    {
        NT_RulesPrime,
        { NT_Rules },
        1
    },
    {
        NT_RulesPrime,
        { Epsillon },
        1
    },
    {
        NT_Rule,
        { NT_LHS, T_Arrow, NT_RHS, T_Semicolon },
        4
    },
    {
        NT_LHS,
        { T_NonTerminal },
        1
    },
    {
        NT_RHS,
        { T_Eps },
        1
    },
    {
        NT_RHS,
        { NT_Items },
        1
    },
    {
        NT_Items,
        { NT_Item, NT_ItemsPrime },
        2
    },
    {
        NT_ItemsPrime,
        { NT_Items },
        1
    },
    {
        NT_ItemsPrime,
        { Epsillon },
        1
    },
    {
        NT_Item,
        { T_NonTerminal },
        1
    },
    {
        NT_Item,
        { T_TerminalLiteral },
        1
    }
};

static const Rule* todo_table[] = {
    NULL,        &grammar[0],  NULL, NULL,        NULL,         NULL,
    &grammar[2], &grammar[1],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[3],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[4],  NULL, NULL,        NULL,         NULL,
    NULL,        &grammar[6],  NULL, &grammar[5], &grammar[6],  NULL,
    NULL,        &grammar[7],  NULL, NULL,        &grammar[7],  NULL,
    NULL,        &grammar[8],  NULL, NULL,        &grammar[8],  &grammar[9],
    NULL,        &grammar[10], NULL, NULL,        &grammar[11], NULL,
};

bool isNonTerminal(Symbol symbol) {
    return symbol > Epsillon && symbol < T_Invalid;
}

bool isTerminal(Symbol symbol) {
    return symbol >= T_Invalid;
}

const Rule *todo_table_get(Symbol nonterm, Symbol term) {
    assert(isNonTerminal(nonterm));
    assert(isTerminal(term));

    if (term == T_Invalid) {
        return NULL;
    }
    
    size_t index = (nonterm - NT_Rules) * 5 + (term - T_EOF);
    return todo_table[index];
}

void parse(const char *filename, FILE *source) {
}
