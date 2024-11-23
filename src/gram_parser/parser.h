#pragma once

#include <stdbool.h>

bool parse(const char *filename, const char *source, size_t source_length);
void test_lexer(const char *filename, const char *source, size_t source_length);
