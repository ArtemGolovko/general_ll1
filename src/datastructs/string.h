#pragma once

char *new_string(size_t cap);
void free_string(char *string);

void string_push(char **string, char item);
char string_pop(char **string);

size_t string_len(char *string);

