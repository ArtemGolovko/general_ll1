#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE *file;
    size_t pos;

    /** string */
    char *buffer;
} Reader;


Reader new_Reader(FILE *file);
void free_Reader(Reader *reader);

char Reader_peek(Reader *reader, size_t n);

void Reader_consume(Reader *reader, size_t n);

bool Reader_is_eof(Reader *reader);


