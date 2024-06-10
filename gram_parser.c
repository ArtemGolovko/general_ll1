#include <stdlib.h>

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
    NULL,        &grammar[0],  NULL,        NULL,         NULL,
    &grammar[2], &grammar[1],  NULL,        NULL,         NULL,
    NULL,        &grammar[3],  NULL,        NULL,         NULL,
    NULL,        &grammar[4],  NULL,        NULL,         NULL,
    NULL,        &grammar[6],  &grammar[5], &grammar[6],  NULL,
    NULL,        &grammar[7],  NULL,        &grammar[7],  NULL,
    NULL,        &grammar[8],  NULL,        &grammar[8],  &grammar[9],
    NULL,        &grammar[10], NULL,        &grammar[11], NULL,
};
