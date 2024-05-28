#ifndef DATASTRUCTS_VECTOR_H
#define DATASTRUCTS_VECTOR_H

void *new_vector(size_t item_size, size_t cap);
void free_vector(void *vector);

void *vector_at(void *vector, size_t index);
void vector_push(void **vector, void *item);

#ifdef TEST

#define GROWH_RATE 5

typedef struct {
    size_t item_size;
    size_t cap;
    size_t length;
} vector_metadata_t;

vector_metadata_t get_vector_metadata(void *vector);
#endif

#endif
