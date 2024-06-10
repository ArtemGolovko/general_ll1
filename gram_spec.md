# Gram

## Gram kanguage grammar
```
/**
 * Gram language grammar written in Gram language.
 **/
/* 1: */ Rules -> Rule RulesPrime;
/* 2: */ RulesPrime -> Rules;
/* 3: */ RulesPrime -> eps;

/* 4: */ Rule -> LHS "->" RHS ";";
/* 5: */ LHS -> NonTerminal;

/* 6: */ RHS -> "eps";
/* 7: */ RHS -> Items;

/* 8: */ Items -> Item ItemsPrime;
/* 9: */ ItemsPrime -> Items;
/* 10: */ ItemsPrime -> eps;

/* 11: */ Item -> NonTerminal;
/* 12: */ Item -> TerminalLiteral;

// Terminals 
// This syntax is not a part of Gram language.
NonTerminal -> [A-Z] [A-Za-z_]+;
TerminalLiteral -> "\"" string "\"";
```

## Non-Terminals

| Non-Terminal | First                               | Follow                            |
| ------------ | ----------------------------------- | --------------------------------- |
| Rules        | NonTerminal                         | NonTerminal, $                    |
| RulesPrime   | NonTerminal, eps                    | $                                 |
| Rule         | NonTerminal                         | NonTerminal, $                    |
| LHS          | NonTerminal                         | "->"                              |
| RHS          | NonTerminal, TerminalLiteral, "eps" | ";"                               |
| Items        | NonTerminal, TerminalLiteral        | NonTerminal, TerminalLiteral, ";" |
| ItemsPrime   | NonTerminal, TerminalLiteral, eps   | NonTerminal, TerminalLiteral, ";" |
| Item         | NonTerminal, TerminalLiteral        | NonTerminal, TerminalLiteral, ";" |

## LL(1) Todo table

| Non-Terminal | $ | NonTerminal | "eps" | TerminalLiteral | ";" |
| ------------ | - | ----------- | ----- | --------------- | --- |
| Rules        |   | 1           |       |                 |     |
| RulesPrime   | 3 | 2           |       |                 |     |
| Rule         |   | 4           |       |                 |     |
| LHS          |   | 5           |       |                 |     |
| RHS          |   | 7           | 6     | 7               |     |
| Items        |   | 8           |       | 8               |     |
| ItemsPrime   |   | 9           |       | 9               | 10  |
| Item         |   | 11          |       | 12              |     |

