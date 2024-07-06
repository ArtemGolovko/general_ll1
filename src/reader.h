#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE *file;
    size_t pos;
    bool eof;

    /** string */
    char *buffer;
} Reader;


Reader new_Reader(FILE *file);

char Reader_peek(Reader *reader, size_t n);

void Reader_consume(Reader *reader, size_t n);

bool Reader_is_eof(Reader *reader);
