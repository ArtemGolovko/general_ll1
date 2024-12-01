# Todolist

## Stage 1: Gram parser
- [x] Move public definitions from gram_parser.c to gram_parser.h
- [x] Fix ordering issues in AST
- [x] Create function to free AST 

## Refactor 1: Introduce common data structures
- [x] Vector (dynamic array)
- [x] Double linked list
- [x] String (growable string)

## Gram parser rewrite
- New fixed grammar
- Top Down AST construction
- Error recovery

- [x] Symbols, Grammar, Todo Table
- [x] Lexer
- [x] Remake lexer (current version is too bugged)
- [x] Parser
- [x] AST
- [x] Panic mode error recovery
- [ ] Parsing level error recovery (Maybe)

## Stage 2: Analyzer
- [ ] Construct symbolic table
- [ ] Convert AST to grammar
- [ ] Find firsts and follows of all non-treminals in grammar
- [ ] Construct LL(1) parsing table

## Stage 3: Generic parser
N/D
