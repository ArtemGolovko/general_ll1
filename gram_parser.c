#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
    char *buffer = (char *)malloc(15 * sizeof(char));
    size_t buffer_length = 14;
    size_t buffer_usage = 0;

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);
        if (isalpha(c) || c == '_') {
            if (buffer_usage + 1 >= buffer_length) {
                buffer_length += 15;
                buffer = realloc(buffer, buffer_length * sizeof(char));
            }

            buffer[buffer_usage] = c;
            buffer_usage += 1;
            continue;
        }

        
        fseek(lexer->source, -1, SEEK_CUR);
        token_t token;

        init_token(&token, lexer, Id, buffer_usage);
        token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

        buffer[buffer_usage] = '\0';
        strcpy_s(token.value, buffer_usage + 1, buffer);

        free(buffer);

        return token;
    }

    fseek(lexer->source, -1, SEEK_CUR);
    token_t token;

    init_token(&token, lexer, Id, buffer_usage);
    token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

    buffer[buffer_usage] = '\0';
    strcpy_s(token.value, buffer_usage + 1, buffer);

    free(buffer);

    return token;
}

token_t lexer_read_Terminal(lexer_t *lexer) {
    char c;
    bool is_prev_backslash = false;
    char *buffer = (char *)malloc(15 * sizeof(char));
    size_t buffer_length = 14;
    size_t buffer_usage = 0;

    while (!feof(lexer->source) && c != -1) {
        c = fgetc(lexer->source);

        if (!is_prev_backslash && c == '"') {
            token_t token;

            init_token(&token, lexer, Terminal, buffer_usage);
            token.value = (char *)malloc((buffer_usage + 1) * sizeof(char));

            buffer[buffer_usage] = '\0';
            strcpy_s(token.value, buffer_usage + 1, buffer);

            free(buffer);

            return token;
        }
        
        is_prev_backslash = c == '\\';

        if (buffer_usage + 1 >= buffer_length) {
            buffer_length += 15;
            buffer = realloc(buffer, buffer_length * sizeof(char));
        }

        buffer[buffer_usage] = c;
        buffer_usage += 1;
    }
    
    free(buffer);

    token_t token;
    init_token(&token, lexer, Invalid, buffer_usage + 1);
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
    /* Terminates colletion of items. */
    None
} stack_item_type_t;

typedef struct {
    stack_item_type_t type;
    token_type_t terminal;
    non_terminal_t non_terminal;
} stack_item_t;

stack_item_t create_stack_item_terminal(token_type_t terminal) {
    stack_item_t item;
    item.type = TerminalItem;
    item.terminal = terminal;
    return item;
}

stack_item_t create_stack_item_non_terminal(non_terminal_t non_terminal) {
    stack_item_t item;
    item.type = NonTerminal;
    item.non_terminal = non_terminal;
    return item;
}

stack_item_t create_stack_item_epsillon() {
    stack_item_t item;
    item.type = Epsillon;
    return item;
}

stack_item_t create_stack_item_pop(non_terminal_t non_terminal) {
    stack_item_t item;
    item.type = Pop;
    item.non_terminal = non_terminal;
    return item;
}

stack_item_t create_stack_item_none() {
    stack_item_t item;
    item.type = None;
    return item;
}

typedef struct {
    stack_item_t *arr;
    size_t arr_size;
    size_t top;
} stack_t;

void init_stack(stack_t *stack) {
    stack->arr = (stack_item_t *)malloc(15 * sizeof(stack_item_t));
    stack->arr_size = 15;
    stack->top = -1;
}

void stack_push(stack_t *stack, stack_item_t item) {
    if (stack->top >= stack->arr_size - 1) {
        stack->arr_size += 15;
        stack->arr = (stack_item_t *)realloc(stack->arr,  stack->arr_size*sizeof(stack_item_t));
    }

    stack->top += 1;
    stack->arr[stack->top] = item;
}

stack_item_t stack_pop(stack_t *stack) {
    assert(stack->top != -1);
    stack_item_t item = stack->arr[stack->top];

    stack->top -= 1;
    return item;
}

bool stack_is_empty(stack_t *stack) {
    return stack->top == -1;
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

semantic_stack_item_t create_semantic_stack_item_token(token_t token) {
    semantic_stack_item_t item;
    item.type = Token; 
    item.token = token;
    return item;
}

semantic_stack_item_t create_semantic_stak_item_epsillon() {
    semantic_stack_item_t item;
    item.type = EpsillonToken;
    return item;
}

semantic_stack_item_t create_semantic_stak_item_ast(void *ast_node) {
    semantic_stack_item_t item;
    item.type = AST;
    item.ast_node = ast_node;
    return item;
}

typedef struct {
    semantic_stack_item_t *arr;
    size_t arr_size;
    size_t top;
} semantic_stack_t;

void init_semantic_stack(semantic_stack_t *stack) {
    stack->arr = (semantic_stack_item_t *)malloc(15 * sizeof(semantic_stack_item_t));
    stack->arr_size = 15;
    stack->top = -1;
}

void semantic_stack_push(semantic_stack_t *stack, semantic_stack_item_t item) {
    if (stack->top >= stack->arr_size - 1) {
        stack->arr_size += 15;
        stack->arr = (semantic_stack_item_t *)realloc(stack->arr,  stack->arr_size*sizeof(semantic_stack_item_t));
    }

    stack->top += 1;
    stack->arr[stack->top] = item;
}

semantic_stack_item_t semantic_stack_pop(semantic_stack_t *stack) {
    assert(stack->top != -1);
    semantic_stack_item_t item = stack->arr[stack->top];

    stack->top -= 1;
    return item;
}

stack_item_t *parsing_table_get(non_terminal_t non_terminal, token_type_t token) {
    if (non_terminal == Rules && token == Id) {
        stack_item_t *items = (stack_item_t *)malloc(3 * sizeof(stack_item_t));
        items[0] = create_stack_item_non_terminal(Rules_Prime);
        items[1] = create_stack_item_non_terminal(Rule);
        items[2] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Rules_Prime && token == Id) {
        stack_item_t *items = (stack_item_t *)malloc(2 * sizeof(stack_item_t));
        items[0] = create_stack_item_non_terminal(Rules);
        items[1] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Rules_Prime && token == Eof) {
        stack_item_t *items = (stack_item_t *)malloc(2 * sizeof(stack_item_t));
        items[0] = create_stack_item_epsillon();
        items[1] = create_stack_item_none();
        return items;
    }


    if (non_terminal == Rule && token == Id) {
        stack_item_t *items = (stack_item_t *)malloc(5 * sizeof(stack_item_t));
        items[0] = create_stack_item_terminal(Semicolon);
        items[1] = create_stack_item_non_terminal(Production);
        items[2] = create_stack_item_terminal(Arrow);
        items[3] = create_stack_item_terminal(Id);
        items[4] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Production && (token == Id || token == Terminal || token == Eps)) {
        stack_item_t *items = (stack_item_t *)malloc(3 * sizeof(stack_item_t));
        items[0] = create_stack_item_non_terminal(Production_Prime);
        items[1] = create_stack_item_non_terminal(Item);
        items[2] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Production_Prime && (token == Id || token == Terminal || token == Eps)) {
        stack_item_t *items = (stack_item_t *)malloc(2 * sizeof(stack_item_t));
        items[0] = create_stack_item_non_terminal(Production);
        items[1] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Production_Prime && token == Semicolon) {
        stack_item_t *items = (stack_item_t *)malloc(2 * sizeof(stack_item_t));
        items[0] = create_stack_item_epsillon();
        items[1] = create_stack_item_none();
        return items;
    }

    if (non_terminal == Item && (token == Id || token == Terminal || token == Eps)) {
        stack_item_t *items = (stack_item_t *)malloc(2 * sizeof(stack_item_t));
        items[0] = create_stack_item_terminal(token);
        items[1] = create_stack_item_none();
        return items;
    }

    return NULL;
}

typedef enum {
    ASTItemTerminal,
    ASTItemNonterminal,
    ASTItemEpsillon
} ast_item_type_t;

typedef struct {
    ast_item_type_t type;
    char *value;
} ast_item_t;

typedef struct {
    ast_item_t *items;
    size_t length;
} ast_production_t;

typedef struct {
    char *name;
    ast_production_t *production;
} ast_rule_t;

typedef struct {
    ast_rule_t *rules;    
    size_t length;
} ast_rules_t;

/*
 * Fixes ordering in AST.
 **/
void AST_reverse(ast_rules_t *ast_root) {
    size_t start = 0;
    size_t end = ast_root->length - 1;
    ast_rule_t tmp_rule;

    while (start < end) {
        tmp_rule = ast_root->rules[start];
        ast_root->rules[start] = ast_root->rules[end];
        ast_root->rules[end] = tmp_rule;

        start += 1;
        end -= 1;
    }

    ast_production_t *production;
    ast_item_t tmp_item;

    for (size_t i = 0; i < ast_root->length; i += 1) {
        production = ast_root->rules[i].production;
        start = 0;
        end = production->length - 1;

        while (start < end) {
            tmp_item = production->items[start];
            production->items[start] = production->items[end];
            production->items[end] = tmp_item;

            start += 1;
            end -= 1;
        }
    }
}

void free_AST(ast_rules_t *ast_root) {
    ast_production_t *production;
    for (size_t i = 0; i < ast_root->length; i += 1) {
        free(ast_root->rules[i].name);
        production = ast_root->rules[i].production;

        for (size_t j = 0; j < production->length; j += 1) {
            free(production->items[j].value);
        }

        free(production->items);
        free(production);
    }

    free(ast_root->rules);
    free(ast_root);
}

ast_rules_t *parse(FILE *source) {
    lexer_t lexer;
    init_lexer(&lexer, source);

    stack_t stack;
    init_stack(&stack);

    stack_push(&stack, create_stack_item_terminal(Eof));
    stack_push(&stack, create_stack_item_non_terminal(Rules));

    semantic_stack_t semantic_stack;
    init_semantic_stack(&semantic_stack);

    token_t current_token = lexer_next_token(&lexer);
    stack_item_t stack_top;

    while (!stack_is_empty(&stack)) {
        stack_top = stack_pop(&stack);
        switch (stack_top.type) {
            case TerminalItem: {
                if (stack_top.terminal != current_token.type) {
                    perror("Parsing error");
                    exit(1);
                }

                if (current_token.type != Eof) {
                    semantic_stack_push(&semantic_stack, create_semantic_stack_item_token(current_token));
                }

                current_token = lexer_next_token(&lexer);

                break;
            }

            case NonTerminal: {
                stack_item_t *production = parsing_table_get(stack_top.non_terminal, current_token.type);
                if (production == NULL) {
                    perror("Parsing error 2");
                    exit(1);
                }
                
                stack_push(&stack, create_stack_item_pop(stack_top.non_terminal));

                for (size_t i = 0; production[i].type != None; i += 1) {
                    stack_push(&stack, production[i]); 
                }

                free(production);
                
                break;
            }

            case Epsillon: {
                semantic_stack_push(&semantic_stack, create_semantic_stak_item_epsillon());
                break;
            }

            case Pop: {
                switch (stack_top.non_terminal) {
                    case Rules: {
                        ast_rules_t *rules_prime = (ast_rules_t *)semantic_stack_pop(&semantic_stack).ast_node;
                        ast_rule_t *rule = (ast_rule_t *)semantic_stack_pop(&semantic_stack).ast_node;

                        ast_rules_t *new_node = (ast_rules_t *)malloc(sizeof(ast_rules_t));
                        new_node->length = rules_prime->length + 1;
                        new_node->rules = (ast_rule_t *)realloc(rules_prime->rules, new_node->length * sizeof(ast_rule_t));

                        new_node->rules[new_node->length - 1] = *rule;
                        free(rules_prime);
                        free(rule);

                        semantic_stack_item_t item = create_semantic_stak_item_ast(new_node);
                        semantic_stack_push(&semantic_stack, item);
                        break;
                    }
                    case Rules_Prime: {
                        semantic_stack_item_t item = semantic_stack_pop(&semantic_stack);

                        if (item.type == EpsillonToken) {
                            ast_rules_t *new_node = (ast_rules_t *)malloc(sizeof(ast_rules_t));
                            new_node->length = 0;
                            new_node->rules = (ast_rule_t *)malloc(0);

                            semantic_stack_item_t new_item = create_semantic_stak_item_ast(new_node);
                            semantic_stack_push(&semantic_stack, new_item);
                            break;
                        }

                        semantic_stack_push(&semantic_stack, item);

                        break;
                    }
                    case Rule: {
                        semantic_stack_pop(&semantic_stack);
                        ast_production_t *production = (ast_production_t *)semantic_stack_pop(&semantic_stack).ast_node;
                        semantic_stack_pop(&semantic_stack);
                        token_t id = semantic_stack_pop(&semantic_stack).token;

                        ast_rule_t *new_node = (ast_rule_t *)malloc(sizeof(ast_rule_t));
                        new_node->name = id.value;
                        new_node->production = production;

                        semantic_stack_item_t item = create_semantic_stak_item_ast(new_node);
                        semantic_stack_push(&semantic_stack, item);
                        break;
                    }
                    case Production: {
                        ast_production_t *prodution_prime = (ast_production_t *)semantic_stack_pop(&semantic_stack).ast_node;
                        ast_item_t *item = (ast_item_t *)semantic_stack_pop(&semantic_stack).ast_node;

                        ast_production_t *new_node = (ast_production_t *)malloc(sizeof(ast_production_t));
                        new_node->length = prodution_prime->length + 1;
                        new_node->items = (ast_item_t *)realloc(prodution_prime->items, new_node->length * sizeof(ast_item_t));

                        new_node->items[new_node->length - 1] = *item;
                        free(prodution_prime);
                        free(item);
                        
                        semantic_stack_item_t new_item = create_semantic_stak_item_ast(new_node);
                        semantic_stack_push(&semantic_stack, new_item);
                        break;
                    }
                    case Production_Prime: {
                        semantic_stack_item_t item = semantic_stack_pop(&semantic_stack);

                        if (item.type == EpsillonToken) {
                            ast_production_t *new_node = (ast_production_t *)malloc(sizeof(ast_production_t));
                            new_node->length = 0;
                            new_node->items = (ast_item_t *)malloc(0);

                            semantic_stack_item_t new_item = create_semantic_stak_item_ast(new_node);
                            semantic_stack_push(&semantic_stack, new_item);
                            break;
                        }

                        semantic_stack_push(&semantic_stack, item);
                        break;
                    }
                    case Item: {
                        token_t token = semantic_stack_pop(&semantic_stack).token;

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

                        semantic_stack_item_t item = create_semantic_stak_item_ast(new_node);
                        semantic_stack_push(&semantic_stack, item);
                        break;
                    }
                }
                break;
            }

            case None: break;
        }
    }

    ast_rules_t *ast_root = (ast_rules_t *)semantic_stack_pop(&semantic_stack).ast_node;
    AST_reverse(ast_root);

    free(stack.arr);
    free(semantic_stack.arr);
    printf("parsing success");

    return ast_root;
}
