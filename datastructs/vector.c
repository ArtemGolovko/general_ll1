#include "vector.h"

#include <stdlib.h>
#include <string.h>

#define GROWH_RATE 5

typedef struct {
    size_t item_size;
    size_t cap;
    size_t length;
} vector_metadata_t;


void *new_vector(size_t item_size, size_t cap) {
    cap = max(cap, GROWH_RATE);

    vector_metadata_t metadata = {
        item_size,
        cap,
        0
    };

    char *vector = (char *)malloc(sizeof(vector_metadata_t) + item_size * cap);
    memcpy(vector, &metadata, sizeof(vector_metadata_t));
    
    vector += sizeof(vector_metadata_t);

    return vector;
}

void free_vector(void *vector) {
    char *begin = (char *)vector;
    begin -= sizeof(vector_metadata_t);

    free(begin);
}

vector_metadata_t *get_vector_metadata(void *vector) {
    char *begin = (char *)vector;
    begin -= sizeof(vector_metadata_t);

    return (vector_metadata_t *)begin;
}

size_t vector_len(void *vector) {
    vector_metadata_t *metadata = get_vector_metadata(vector);
    return metadata->length;
}

void *vector_grow(void *vector) {
    vector_metadata_t *metadata = get_vector_metadata(vector);

    metadata->cap += GROWH_RATE;

    char *begin = (char *)vector;
    begin -= sizeof(vector_metadata_t);

    char *new_vector = realloc(begin, sizeof(vector_metadata_t) + metadata->item_size * metadata->cap);

    new_vector += sizeof(vector_metadata_t);

    return new_vector;
}

void vector_push(void **vector, const void *item) {
    vector_metadata_t *metadata = get_vector_metadata(*vector);

    if (metadata->cap <= metadata->length) {
        *vector = vector_grow(*vector);
        metadata = get_vector_metadata(*vector);
    }

    char *begin = *vector;
    memcpy(begin + metadata->item_size * metadata->length, item, metadata->item_size);
    metadata->length += 1;
}

void vector_push_front(void **vector, const void *item) {
    vector_metadata_t *metadata = get_vector_metadata(*vector);

    if (metadata->cap <= metadata->length) {
        *vector = vector_grow(*vector);
        metadata = get_vector_metadata(*vector);
    }

    char *begin = *vector;
    memmove(begin + metadata->item_size, begin, metadata->item_size * metadata->length);
    memcpy(begin, item, metadata->item_size);
    metadata->length += 1;
}
