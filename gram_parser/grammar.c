#include "grammar.h"

#include <assert.h>

bool is_NonTerminal(Symbol symbol) {
    return symbol > Epsillon && symbol < T_Invalid;
}

bool is_Terminal(Symbol symbol) {
    return symbol >= T_Invalid;
}

const Rule *todo_table_get(Symbol nonterm, Symbol term) {
    assert(is_NonTerminal(nonterm));
    assert(is_Terminal(term));

    if (term == T_Invalid) {
        return NULL;
    }
    
    size_t index = (nonterm - NT_Rules) * 6 + (term - T_EOF);
    return todo_table[index];
}


// Debug


const char *Symbol_to_string(Symbol symbol) {
    switch (symbol) {
        case Epsillon: return "Epsillon";
        case NT_Rules: return "NT_Rules";
        case NT_RulesPrime: return "NT_RulesPrime";
        case NT_Rule: return "NT_Rule";
        case NT_LHS: return "NT_LHS";
        case NT_RHS: return "NT_RHS";
        case NT_Items: return "NT_Items";
        case NT_ItemsPrime: return "NT_ItemsPrime";
        case NT_Item: return "NT_Item";
        case T_Invalid: return "T_Invalid";
        case T_EOF: return "T_EOF";
        case T_NonTerminal: return "T_NonTerminal";
        case T_Arrow: return "T_Arrow";
        case T_Eps: return "T_Eps";
        case T_TerminalLiteral: return "T_TerminalLiteral";
        case T_Semicolon: return "T_Semicolon";
    }
}
