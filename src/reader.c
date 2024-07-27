#include "reader.h"

#include <assert.h>
#include "datastructs/string.h"

Reader new_Reader(FILE *file) {
    Reader reader = {
        file,
        0,
        new_string(3),
    };

    return reader;
}

void free_Reader(Reader *reader) {
    free_string(reader->buffer);
}
char Reader_peek(Reader *reader, size_t n) {
    assert(n > 0);
    
    if (n <= string_len(reader->buffer)) {
        return reader->buffer[n - 1];
    }
    
    if (feof(reader->file)) {
        return EOF;
    }

    size_t buffer_characters = n - string_len(reader->buffer);

    for (size_t i = 0; i < buffer_characters; i += 1) {
        char c = fgetc(reader->file);

        if (c == EOF || c == 0) {
            break;
        }

        string_push(&reader->buffer, c);
    }

    if (n <= string_len(reader->buffer)) {
        return reader->buffer[n - 1];
    }
    
    return EOF;
}

void Reader_consume(Reader *reader, size_t n) {
    assert(n > 0);

    for (size_t i = 0; i < n; i += 1) {
        if (string_len(reader->buffer) > 0) {
            string_pop(&reader->buffer);
            continue;
        }
        fgetc(reader->file); 

    }

    reader->pos += n;

}
