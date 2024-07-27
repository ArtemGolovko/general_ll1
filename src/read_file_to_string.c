#include "read_file_to_string.h"

#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 1024

ReadInfo read_file_to_string(const char *filename) {
    ReadInfo info = {
        malloc(sizeof(char) * CHUNK_SIZE),
        0
    };
    
    // size in chunks
    size_t buffer_size = 1;
    size_t chunks_read = 0;
    
    FILE *file = fopen(filename, "r");

    while (!feof(file)) {
        if (chunks_read >= buffer_size) {
            buffer_size += 1;
            info.buffer = (char *)realloc(info.buffer, buffer_size * CHUNK_SIZE * sizeof(char));
        }

        size_t bytes_read = fread(info.buffer + (chunks_read * CHUNK_SIZE), sizeof(char), CHUNK_SIZE, file);

        chunks_read += 1;

        if (bytes_read < CHUNK_SIZE) {
            info.length = (chunks_read * CHUNK_SIZE) - (CHUNK_SIZE - bytes_read);
            break;
        }
    }
    
    fclose(file);

    return info;
}
