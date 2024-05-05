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

typedef struct stack_item_struct {
    stack_item_type_t type;
    token_type_t terminal;
    non_terminal_t non_terminal;
    struct stack_item_struct *production;
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

stack_item_t create_stack_item_pop(non_terminal_t non_terminal, stack_item_t *production) {
    stack_item_t item;
    item.type = Pop;
    item.non_terminal = non_terminal;
    item.production = production;
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
    EpsillonToken
} semantic_stack_item_type_t;

typedef struct {
    semantic_stack_item_type_t type;
    token_t token;
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

void parse(FILE *source) {
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

    while(!stack_is_empty(&stack)) {
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
                
                stack_push(&stack, create_stack_item_pop(stack_top.non_terminal, production));

                for (size_t i = 0; production[i].type != None; i += 1) {
                    stack_push(&stack, production[i]); 
                }

                
                break;
            }

            case Epsillon: {
                semantic_stack_push(&semantic_stack, create_semantic_stak_item_epsillon());
                break;
            }

            case Pop: {
                // TODO build ast
                free(stack_top.production);
                break;
            }

            case None: break;
        }
    }

    free(stack.arr);
    free(semantic_stack.arr);
    printf("parsing success");
}
