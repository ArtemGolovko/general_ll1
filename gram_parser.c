#include "gram_parser.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "datastructs/string.h"
#include "datastructs/linked_list.h"
#include "datastructs/vector.h"

typedef enum {
    Invalid,
    Eof,
    Id,
    Terminal,
    Eps,
    Arrow,
    Semicolon
} token_type_t;


typedef struct {
    token_type_t type;

    /** string */
    char *value;

    size_t row;
    size_t col;
    size_t pos;
} token_t;

typedef struct {
    FILE *source;
    size_t nl;
    size_t row;
} lexer_t;

void init_token(token_t *token, lexer_t* lexer, token_type_t type, size_t length) {
    token->type = type;
    token->row = lexer->row;
    token->pos = ftell(lexer->source) - length;
    token->col = token->pos - lexer->nl + 1; 
    token->value = NULL;
}

void init_lexer(lexer_t *lexer, FILE *source) {
    lexer->source = source;
    lexer->nl = 0;
    lexer->row = 1;
}

void lexer_skip_mulitline_comment(lexer_t *lexer) {
    char c;
    bool is_prev_star = false;

    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);

        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            continue;
        }

        if (is_prev_star && c == '/') {
            return;
        }

        is_prev_star = c == '*';
    }
}

void lexer_skip_singleline_comment(lexer_t *lexer) {
    char c;
    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);
        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            return;
        }
    }
}

void lexer_skip_whitespace_and_comments(lexer_t *lexer) {
    char c;
    while (!feof(lexer->source)) {
        c = fgetc(lexer->source);
        if (c == '\n') {
            lexer->nl = ftell(lexer->source);
            lexer->row += 1;
            continue;
        }

        if (isspace(c) || c == -1) {
            continue;
        }

        if (c == '/') {
            c = fgetc(lexer->source);
            if (c == '*') {
                lexer_skip_mulitline_comment(lexer);
                continue;
            }

            if (c == '/') {
                lexer_skip_singleline_comment(lexer);
                continue;
            }

            fseek(lexer->source, -2, SEEK_CUR);
            return;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        return;
    }

}

token_t lexer_read_Id(lexer_t *lexer) {
    char c;
    char *value = new_string(0);

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);
        if (isalpha(c) || c == '_') {

            string_push(&value, c);
            continue;
        }

        
        fseek(lexer->source, -1, SEEK_CUR);
        token_t token;

        init_token(&token, lexer, Id, string_len(value));
        token.value = value; 


        return token;
    }

    fseek(lexer->source, -1, SEEK_CUR);
    token_t token;

    init_token(&token, lexer, Id, string_len(value));
    token.value = value;

    return token;
}

token_t lexer_read_Terminal(lexer_t *lexer) {
    char c;
    bool is_prev_backslash = false;
    char *value = new_string(0);

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);

        if (!is_prev_backslash && c == '"') {
            token_t token;

            init_token(&token, lexer, Terminal, string_len(value));
            token.value = value;

            return token;
        }

        is_prev_backslash = c == '\\';

        string_push(&value, c);
    }


    token_t token;
    init_token(&token, lexer, Invalid, string_len(value));

    free_string(value);
    return token;
}

token_t lexer_next_token(lexer_t *lexer) {
    lexer_skip_whitespace_and_comments(lexer);
    
    token_t token;

    if (feof(lexer->source)) {
        init_token(&token, lexer, Eof, 0); 
        return token;
    }

    char c = fgetc(lexer->source);

    if (c == ';') {
        init_token(&token, lexer, Semicolon, 1);
        return token; 
    }

    if (c == '-') {
        c = fgetc(lexer->source);
        if (c == '>') {
            init_token(&token, lexer, Arrow, 2);
            return token;
        }
        fseek(lexer->source, -1, SEEK_CUR);
        init_token(&token, lexer, Invalid, 1); 
        return token;
    }

    if (c == 'e') {
        c = fgetc(lexer->source);
        if (c == 'p') {
            c = fgetc(lexer->source);
            if (c == 's') {
                init_token(&token, lexer, Eps, 3);
                return token;
            }
            
            fseek(lexer->source, -2, SEEK_CUR);
            init_token(&token, lexer, Invalid, 1);
            return token;
        }

        fseek(lexer->source, -1, SEEK_CUR);
        init_token(&token, lexer, Invalid, 1);
        return token;
    }

    if (isalpha(c) && isupper(c)) {
        fseek(lexer->source, -1, SEEK_CUR);
        token = lexer_read_Id(lexer);
        return token;
    }

    if (c == '"') {
        token = lexer_read_Terminal(lexer);
        return token;
    }

    init_token(&token, lexer,  Invalid, 1);
    return token;
}

typedef enum {
    Rules,
    Rules_Prime,
    Rule,
    Production,
    Production_Prime,
    Item
} non_terminal_t;

typedef enum {
    TerminalItem,
    NonTerminal,
    Epsillon,
    Pop,
} stack_item_type_t;

typedef struct {
    stack_item_type_t type;
    token_type_t terminal;
    non_terminal_t non_terminal;
} stack_item_t;

void *new_stack() {
    return new_linked_list(sizeof(stack_item_t));
}

void stack_push(void *stack, stack_item_t item) {
    linked_list_push(stack, &item);
}

void stack_push_termianl(void *stack, token_type_t terminal) {
    stack_item_t item;
    item.type = TerminalItem;
    item.terminal = terminal;

    linked_list_push(stack, &item);
}

void stack_push_non_terminal(void *stack, non_terminal_t non_terminal) {
    stack_item_t item;
    item.type = NonTerminal;
    item.non_terminal = non_terminal;

    linked_list_push(stack, &item);
}

void stack_push_epsillon(void *stack) {
    stack_item_t item;
    item.type = Epsillon;

    linked_list_push(stack, &item);
}

void stack_push_pop(void *stack, non_terminal_t non_termianl) {
    stack_item_t item;
    item.type = Pop;
    item.non_terminal = non_termianl;

    linked_list_push(stack, &item);
}

stack_item_t stack_pop(void *stack) {
    stack_item_t item;
    linked_list_pop(stack, &item);

    return item;
}

bool stack_is_empty(void *stack) {
    return linked_list_len(stack) == 0;
}

void free_stack(void *stack) {
    free_linked_list(stack);
}

typedef enum {
    Token,
    EpsillonToken,
    AST
} semantic_stack_item_type_t;

typedef struct {
    semantic_stack_item_type_t type;
    token_t token;
    void *ast_node;
} semantic_stack_item_t;

void *new_semantic_stack() {
    return new_linked_list(sizeof(semantic_stack_item_t));
}

void semantic_stack_push(void *stack, semantic_stack_item_t item) {
    linked_list_push(stack, &item);
}
void semantic_stack_push_token(void *stack, token_t token) {
    semantic_stack_item_t item;
    item.type = Token; 
    item.token = token;

    linked_list_push(stack, &item);
}

void semantic_stack_push_epsillion(void *stack) {
    semantic_stack_item_t item;
    item.type = EpsillonToken; 

    linked_list_push(stack, &item);
}

void semantic_stack_push_ast(void *stack, void *ast_node) {
    semantic_stack_item_t item;
    item.type = AST; 
    item.ast_node = ast_node;

    linked_list_push(stack, &item);
}

semantic_stack_item_t semantic_stack_pop(void *stack) {
    semantic_stack_item_t item;
    linked_list_pop(stack, &item);

    return item;
}

void free_semantic_stack(void *stack) {
    free_linked_list(stack);
}

/** Returns stack */
void *parsing_table_get(non_terminal_t non_terminal, token_type_t token) {
    void *prod = new_stack();

    if (non_terminal == Rules && token == Id) {
        stack_push_non_terminal(prod, Rule);
        stack_push_non_terminal(prod, Rules_Prime);
        return prod;
    }

    if (non_terminal == Rules_Prime && token == Id) {
        stack_push_non_terminal(prod, Rules);
        return prod;
    }

    if (non_terminal == Rules_Prime && token == Eof) {
        stack_push_epsillon(prod);
        return prod;
    }


    if (non_terminal == Rule && token == Id) {
        stack_push_termianl(prod, Id);
        stack_push_termianl(prod, Arrow);
        stack_push_non_terminal(prod, Production);
        stack_push_termianl(prod, Semicolon);
        return prod;
    }

    if (non_terminal == Production && (token == Id || token == Terminal || token == Eps)) {
        stack_push_non_terminal(prod, Item);
        stack_push_non_terminal(prod, Production_Prime);
        return prod;
    }

    if (non_terminal == Production_Prime && (token == Id || token == Terminal || token == Eps)) {
        stack_push_non_terminal(prod, Production);
        return prod;
    }

    if (non_terminal == Production_Prime && token == Semicolon) {
        stack_push_epsillon(prod);
        return prod;
    }

    if (non_terminal == Item && (token == Id || token == Terminal || token == Eps)) {
        stack_push_termianl(prod, token);
        return prod;
    }

    return prod;
}

void free_AST(ast_rules_t *ast_root) {
    ast_production_t *production;
    for (size_t i = 0; i < vector_len(ast_root->rules); i += 1) {
        free_string(ast_root->rules[i].name);
        production = ast_root->rules[i].production;

        for (size_t j = 0; j < vector_len(production->items); j += 1) {
            free_string(production->items[j].value);
        }

        free_vector(production->items);
        free(production);
    }

    free_vector(ast_root->rules);
    free(ast_root);
}

ast_rules_t *parse(FILE *source) {
    lexer_t lexer;
    init_lexer(&lexer, source);

    void *stack = new_stack();

    stack_push_termianl(stack, Eof);
    stack_push_non_terminal(stack, Rules);

    void *semantic_stack = new_semantic_stack();

    token_t current_token = lexer_next_token(&lexer);
    stack_item_t stack_top;

    while (!stack_is_empty(stack)) {
        stack_top = stack_pop(stack);
        switch (stack_top.type) {
            case TerminalItem: {
                if (stack_top.terminal != current_token.type) {
                    perror("Parsing error");
                    exit(1);
                }

                if (current_token.type != Eof) {
                    semantic_stack_push_token(semantic_stack, current_token);
                }

                current_token = lexer_next_token(&lexer);

                break;
            }

            case NonTerminal: {
                void *production = parsing_table_get(stack_top.non_terminal, current_token.type);

                if (stack_is_empty(production)) {
                    free_stack(production);

                    perror("Parsing error 2");
                    exit(1);
                }
                
                stack_push_pop(stack, stack_top.non_terminal);
                
                while (!stack_is_empty(production)) {
                    stack_push(stack, stack_pop(production)); 
                }

                free_stack(production);
                
                break;
            }

            case Epsillon: {
                semantic_stack_push_epsillion(semantic_stack);
                break;
            }

            case Pop: {
                switch (stack_top.non_terminal) {
                    case Rules: {
                        ast_rules_t *rules_prime = (ast_rules_t *)semantic_stack_pop(semantic_stack).ast_node;
                        ast_rule_t *rule = (ast_rule_t *)semantic_stack_pop(semantic_stack).ast_node;

                        ast_rules_t *new_node = (ast_rules_t *)malloc(sizeof(ast_rules_t));

                        new_node->rules = rules_prime->rules;
                        vector_push_front((void **)&new_node->rules, rule);

                        free(rules_prime);
                        free(rule);

                        semantic_stack_push_ast(semantic_stack, new_node);
                        break;
                    }
                    case Rules_Prime: {
                        semantic_stack_item_t item = semantic_stack_pop(semantic_stack);

                        if (item.type == EpsillonToken) {
                            ast_rules_t *new_node = (ast_rules_t *)malloc(sizeof(ast_rules_t));
                            new_node->rules = new_vector(sizeof(ast_rule_t), 0);

                            semantic_stack_push_ast(semantic_stack, new_node);
                            break;
                        }

                        semantic_stack_push(semantic_stack, item);

                        break;
                    }
                    case Rule: {
                        semantic_stack_pop(semantic_stack);
                        ast_production_t *production = (ast_production_t *)semantic_stack_pop(semantic_stack).ast_node;
                        semantic_stack_pop(semantic_stack);
                        token_t id = semantic_stack_pop(semantic_stack).token;

                        ast_rule_t *new_node = (ast_rule_t *)malloc(sizeof(ast_rule_t));
                        new_node->name = id.value;
                        new_node->production = production;

                        semantic_stack_push_ast(semantic_stack, new_node);
                        break;
                    }
                    case Production: {
                        ast_production_t *prodution_prime = (ast_production_t *)semantic_stack_pop(semantic_stack).ast_node;
                        ast_item_t *item = (ast_item_t *)semantic_stack_pop(semantic_stack).ast_node;

                        ast_production_t *new_node = (ast_production_t *)malloc(sizeof(ast_production_t));
                        new_node->items = prodution_prime->items;

                        vector_push_front((void **)&new_node->items, item);

                        free(prodution_prime);
                        free(item);
                        
                        semantic_stack_push_ast(semantic_stack, new_node);
                        break;
                    }
                    case Production_Prime: {
                        semantic_stack_item_t item = semantic_stack_pop(semantic_stack);

                        if (item.type == EpsillonToken) {
                            ast_production_t *new_node = (ast_production_t *)malloc(sizeof(ast_production_t));
                            new_node->items = new_vector(sizeof(ast_item_t), 0);

                            semantic_stack_push_ast(semantic_stack, new_node);
                            break;
                        }

                        semantic_stack_push(semantic_stack, item);
                        break;
                    }
                    case Item: {
                        token_t token = semantic_stack_pop(semantic_stack).token;

                        ast_item_t *new_node =  (ast_item_t *)malloc(sizeof(ast_item_t));
                        if (token.type == Terminal) {
                            new_node->type = ASTItemTerminal;
                            new_node->value = token.value;
                        }

                        if (token.type == Id) {
                            new_node->type = ASTItemNonterminal;
                            new_node->value = token.value;
                        }

                        if (token.type == Eps) {
                            new_node->type = ASTItemEpsillon;
                            new_node->value = NULL;
                        }

                        semantic_stack_push_ast(semantic_stack, new_node);
                        break;
                    }
                }
                break;
            }
        }
    }

    ast_rules_t *ast_root = (ast_rules_t *)semantic_stack_pop(semantic_stack).ast_node;

    free_stack(stack);
    free_semantic_stack(semantic_stack);

    return ast_root;
}
