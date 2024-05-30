#include "string.h"

#include <stdlib.h>
#include <string.h>

#define GROWH_RATE 6

typedef struct {
    size_t cap;
    size_t length;
} string_metadata_t;


char *new_string(size_t cap) {
    cap = max(cap, GROWH_RATE);

    string_metadata_t metadata = {
        cap,
        0
    };

    char *string = (char *)malloc(sizeof(string_metadata_t) + sizeof(char) * cap);
    memcpy(string, &metadata, sizeof(string_metadata_t));
    
    string += sizeof(string_metadata_t);
    string[0] = '\0';

    return string;
}

void free_string(char *string) {
    string -= sizeof(string_metadata_t);

    free(string);
}

string_metadata_t *get_string_metadata(char *string) {
    string -= sizeof(string_metadata_t);

    return (string_metadata_t *)string;
}

size_t string_len(char *string) {
    string_metadata_t *metadata = get_string_metadata(string);
    return metadata->length;
}

char *string_grow(char *string) {
    string_metadata_t *metadata = get_string_metadata(string);
    metadata->cap += GROWH_RATE;

    string -= sizeof(string_metadata_t);

    char *new_string = realloc(string, sizeof(string_metadata_t) + sizeof(char) * metadata->cap);

    new_string += sizeof(string_metadata_t);

    return new_string;
}

void string_push(char **string, char item) {
    string_metadata_t *metadata = get_string_metadata(*string);

    if (metadata->cap <= metadata->length + 1) {
        *string = string_grow(*string);
        metadata = get_string_metadata(*string);
    }
    
    char *begin = *string;
    begin[metadata->length] = item;
    begin[metadata->length + 1] = '\0';

    metadata->length += 1;
}
