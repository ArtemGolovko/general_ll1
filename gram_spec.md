# Gram

## Gram kanguage grammar
```
/**
 * Gram language grammar written in Gram language.
 **/
Rules -> Rule Rules_Prime;
Rules_Prime -> Rules;
Rules_Prime -> eps;

Rule -> Id "->" Production ";";
Production -> Item Prodution_Prime;
Production_Prime -> Production;
Production_Prime -> eps;

Item -> Id;
Item -> Terminal;
Item -> "eps";

// Terminals 
// This syntax is not a part of Gram language.
Terminal -> "\"" string "\"";
Id -> [A-Z] [A-Za-z_]+;
```

## Non-Terminals

| Non-Terminal     | First                   | Follow                  |
| ---------------- | ----------------------- | ----------------------- |
| Rules            | Id                      | Id, $                   |
| Rules_Prime      | Id, eps                 | $                       |
| Rule             | Id                      | Id, $                   |
| Production       | Id, Termial, "eps"      | ";"                     |
| Production_Prime | Id, Termial, "eps", eps | Id, Termial, "eps", ";" |
| Item             | Id, Termial, "eps"      | Id, Termial, "eps", ";" |

## LL(1)  Parsing table

| Non-Terminal    | Id                                 | Termial                            | "eps"                              | ";"                     | $                  |
| --------------- | ---------------------------------- | ---------------------------------- | ---------------------------------- | ----------------------- | ------------------ |
| Rules           | Rules -> Rule Rules_Prime          |                                    |                                    |                         |                    |
| Rules_Prime     | Rules_Prime -> Rules               |                                    |                                    |                         | Rules_Prime -> eps |
| Rule            | Id "->" Production ";"             |                                    |                                    |                         |                    |
| Production      | Production -> Item Prodution_Prime | Production -> Item Prodution_Prime | Production -> Item Prodution_Prime |                         |                    |
| Prodution_Prime | Production_Prime -> Production     | Production_Prime -> Production     | Production_Prime -> Production     | Production_Prime -> eps |                    |
| Item            | Item -> Id                         | Item -> Terminal                   | Item -> "eps"                      |                         |                    |
