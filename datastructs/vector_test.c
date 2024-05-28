#define TEST

#include "vector.h"
#include <assert.h>

int main() {
    char *test_vector = (char *)new_vector(sizeof(char), 0);
    vector_metadata_t metadata = get_vector_metadata(test_vector);

    assert(metadata.item_size == 1);
    assert(metadata.cap == 5);
    assert(metadata.length == 0);

    free_vector(test_vector);
    return 0;
}
