#define TEST

#include "vector.h"
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

    return 0;
}
