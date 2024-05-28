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

vector_metadata_t get_vector_metadata(void *vector) {
    char *begin = (char *)vector;
    begin -= sizeof(vector_metadata_t);

    vector_metadata_t metadata = *(vector_metadata_t *)begin;

    return metadata;
}
