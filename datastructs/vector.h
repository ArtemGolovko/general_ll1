#ifndef DATASTRUCTS_VECTOR_H
#define DATASTRUCTS_VECTOR_H

void *new_vector(size_t item_size, size_t cap);
void free_vector(void *vector);

void *vector_at(void *vector, size_t index);
void vector_push(void **vector, void *item);

#endif
