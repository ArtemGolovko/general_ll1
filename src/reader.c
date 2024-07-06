#include "reader.h"

#include <assert.h>
#include "datastructs/string.h"

Reader new_Reader(FILE *file) {
    Reader reader = {
        file,
        0,
        false,
        new_string(3),
    };

    return reader;
}


char Reader_peek(Reader *reader, size_t n) {
    assert(n > 0);

    if (reader->eof) {
        return EOF;
    }

    size_t number_of_characters_to_get = string_len(reader->buffer) - n;

    for (size_t i = 0; i < number_of_characters_to_get; i += 1) {
        char c = fgetc(reader->file);

        if (c == '\0') {
            c = EOF;
        }

        string_push(&reader->buffer, c);
    }

    return reader->buffer[n - 1]; 
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
