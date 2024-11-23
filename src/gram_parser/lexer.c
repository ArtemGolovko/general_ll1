#include "lexer.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include "datastructs/string.h"
#include "gram_parser/grammar.h"

Lexer new_Lexer(const char *filename, const char *source, size_t source_length) {
    Location loc = {
        filename,
        0, 
        1,
        1 
    };

    Lexer lexer = {
        source,
        source_length,
        loc,
        0
    };

    return lexer;
}

bool Lexer_in_bounds(Lexer *lexer) {
    return lexer->loc.pos < lexer->source_length;
}

void Lexer_pos_inc(Lexer *lexer) {
    lexer->loc.pos += 1;
    lexer->loc.col += 1;
}

void Lexer_pos_inc_n(Lexer *lexer, unsigned int n) {
    lexer->loc.pos += n;
    lexer->loc.col += n;
}
void Lexer_pos_inc_nl(Lexer *lexer) {
    lexer->ln_pos = lexer->loc.pos;
    lexer->loc.pos += 1;
    lexer->loc.row += 1;
    lexer->loc.col = 1;
}

char Lexer_ch(Lexer *lexer) {
    if (!Lexer_in_bounds(lexer)) {
        return 0;
    }

    return lexer->source[lexer->loc.pos];
}

char Lexer_peek(Lexer *lexer, unsigned int n) {
    if (lexer->loc.pos + n >= lexer->source_length) {
        return 0;
    }

    return lexer->source[lexer->loc.pos + n];
}

void Lexer_skip_multiline_comment(Lexer *lexer) {
    Lexer_pos_inc_n(lexer, 2); // "/*" 

    char c;
    bool is_prev_star = false;

    while (Lexer_in_bounds(lexer)) {
        c = Lexer_ch(lexer);

        if (c == '\n') {
            Lexer_pos_inc_nl(lexer);
            continue;
        }

        if (is_prev_star && c == '/') {
            Lexer_pos_inc(lexer);
            return;
        }

        is_prev_star = c == '*';
        Lexer_pos_inc(lexer);
    }
}

void Lexer_skip_singleline_comment(Lexer *lexer) {
    Lexer_pos_inc_n(lexer, 2); // "//" 

    char c;
    while (Lexer_in_bounds(lexer)) {
        c = Lexer_ch(lexer);

        if (c == '\n') {
            Lexer_pos_inc_nl(lexer);
            return;
        }

        Lexer_pos_inc(lexer);
    }
}
// TODO: Check EOF
void Lexer_skip_whitespace_and_comments(Lexer *lexer) {
    char c;
    while (Lexer_in_bounds(lexer)) {
        c = Lexer_ch(lexer);

        if (c == '\n') {
            Lexer_pos_inc_nl(lexer);
            continue;
        }

        if (isspace(c) || c == -1) {
            Lexer_pos_inc(lexer);
            continue;
        }

        if (c == '/') {
            c = Lexer_peek(lexer, 1);
            
            if (c == '*') {
                Lexer_skip_multiline_comment(lexer);
                continue;
            }

            if (c == '/') {
                Lexer_skip_singleline_comment(lexer);
                continue;
            }

            return;
        }

        return;
    }

}

Token Lexer_read_NonTerminal(Lexer *lexer) {
    Location loc = lexer->loc;

    char c;
    char *value = new_string(5);

    while (Lexer_in_bounds(lexer)) {
        c = Lexer_ch(lexer);
        
        if (!isalpha(c) && c != '_') {
            break;
        }

        string_push(&value, c);
        Lexer_pos_inc(lexer);
    }

    Token token = {
        T_NonTerminal,
        string_len(value),
        loc,
        value // moves out of this function
    };
    
    return token;
}

Token Lexer_read_TerminalLiteral(Lexer *lexer) {
    int len = 2;
    Location loc = lexer->loc;

    Lexer_pos_inc(lexer); // '"'

    char c;
    bool is_backslash = false;
    char *value = new_string(5);

    while (Lexer_in_bounds(lexer)) {
        c = Lexer_ch(lexer);

        if (!is_backslash && c == '"') {
            Lexer_pos_inc(lexer);

            Token token = {
                T_TerminalLiteral,
                len,
                loc,
                value, // moves out of this function
            };

            return token;
        }

        if (is_backslash || c != '\\') {
            string_push(&value, c);
        }        
        
        if (is_backslash && c == '\\') {
            is_backslash = false;
        } else {
            is_backslash = c == '\\';
        }

        len += 1;

        if (c == '\n') {
            Lexer_pos_inc_nl(lexer);
            continue;
        }

        Lexer_pos_inc(lexer);
    }
    
    Token token = {
        T_Invalid,
        len,
        loc,
        NULL
    };

    return token;
}

Token Lexer_next_token(Lexer *lexer) {
    Lexer_skip_whitespace_and_comments(lexer);
    
    if (!Lexer_in_bounds(lexer)) {
        Token token = {
            T_EOF,
            0,
            lexer->loc,
            NULL,
        };

        return token;
    }

    char c = Lexer_ch(lexer);

    if (c == ';') {
        Token token = {
            T_Semicolon,
            1,
            lexer->loc,
            NULL
        };

        Lexer_pos_inc(lexer);

        return token;
    }

    if (c == '-' && Lexer_peek(lexer, 1) == '>') {
        Token token = {
            T_Arrow,
            2,
            lexer->loc,
            NULL
        };

        Lexer_pos_inc_n(lexer, 2);

        return token;
    }

    if (c == 'e'
        && Lexer_peek(lexer, 1) == 'p'
        && Lexer_peek(lexer, 2) == 's'
        /*&& !isalnum(Lexer_peek(lexer, 3))*/) { //TODO: Add helper is allowed after NonTerminal

        Token token = {
            T_Eps,
            3,
            lexer->loc,
            NULL
        };

        Lexer_pos_inc_n(lexer, 3);

        return token;
    }

    if (isalpha(c) && isupper(c)) {
        return Lexer_read_NonTerminal(lexer);
    }

    if (c == '"') {
        return Lexer_read_TerminalLiteral(lexer);
    }
    
    Token token = {
        T_Invalid,
        0,
        lexer->loc,
        NULL,
    };

    return token;
}
