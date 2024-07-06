#define TEST

#include "vector.h"
#include <stdio.h>
#include <assert.h>

int main() {
    char *test_vector = (char *)new_vector(sizeof(char), 0);
    vector_metadata_t *metadata = get_vector_metadata(test_vector);

    assert(metadata->item_size == 1);
    assert(metadata->cap == 5);
    assert(vector_len(test_vector) == 0);

    char ch = 'A';
    vector_push((void **)&test_vector, &ch); 
    metadata = get_vector_metadata(test_vector);

    assert(metadata->cap == 5);
    assert(vector_len(test_vector) == 1);
    assert(test_vector[0] == 'A');

    free_vector(test_vector);

    int int_arr[11] = { 12, 19, 17, -6, 3, -18, -16, 5, -1, 10, -19 };
    int *int_vec = (int *)new_vector(sizeof(int), 0);

    for (size_t i = 0; i < 11; i += 1) {
        vector_push((void **)&int_vec, &int_arr[i]);
        metadata = get_vector_metadata(int_vec);

        printf("Inedx: %zd\n", i);
        printf("\tcap: %zd\n\tlen: %zd\n\n", metadata->cap, metadata->length);
    }

    free_vector(int_vec);

    int_vec = (int *)new_vector(sizeof(int), 0);

    for (size_t i = 0; i < 11; i += 1) {
        vector_push_front((void **)&int_vec, &int_arr[i]);
    }

    for (size_t i = 0; i < vector_len(int_vec); i += 1) {
        printf("[%zd]: %d\n", i, int_vec[i]);
    }

    free_vector(int_vec);

    return 0;
}
