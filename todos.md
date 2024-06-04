# Todolist

## Stage 1: Gram parser
- [x] Move public definitions from gram_parser.c to gram_parser.h
- [x] Fix ordering issues in AST
- [x] Create function to free AST 

## Refactor 1: Introduce common data structures
- [x] Vector (dynamic array)
- [x] Double linked list
- [x] String (growable string)

## Grammar fix
As for the time this text is written, the gram grammar allows invalid LL(1)
grammars to be parsed.

Examples of borken grammars:
```
S -> eps eps;
S -> A eps;
S -> eps "a";
```

If non terminal has an empty production, the its rhs must contain an single
item of epsillon.

- [ ] Fix gram spec
- [ ] Fix gram parser


## Stage 2: Analyzer
- [ ] Construct symbolic table
- [ ] Convert AST to grammar
- [ ] Find firsts and follows of all non-treminals in grammar
- [ ] Construct LL(1) parsing table

## Stage 3: Generic parser
N/D
