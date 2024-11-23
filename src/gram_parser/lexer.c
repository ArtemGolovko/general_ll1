#include "lexer.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
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
    Lexer_pos_inc(lexer); // '/'
    Lexer_pos_inc(lexer); // '*'

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
    Lexer_pos_inc(lexer); // '/'
    Lexer_pos_inc(lexer); // '/'

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

Token Lexer_next_token(Lexer *lexer) {
    Lexer_skip_whitespace_and_comments(lexer);

    Token token = {
        T_EOF,
        0,
        lexer->loc,
        NULL,
    };

    return token;
}
