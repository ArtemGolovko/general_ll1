#ifndef DATASTRUCTS_STRING_H
#define DATASTRUCTS_STRING_H

char *new_string(size_t cap);
void free_string(char *string);

void string_push(char **string, char item);

size_t string_len(char *string);

#endif
