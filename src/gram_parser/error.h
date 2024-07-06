#pragma once

#include "grammar.h"
#include "lexer.h"


void display_not_matching_terminal_error(Lexer *lexer, Symbol top, Token *token);

void display_not_matching_rule_error(Lexer *lexer, Symbol top, Token *token);
