#define TEST

#include <check.h>

#include <stdio.h>
#include "datastructs/vector.h"


START_TEST(vector_test_internals)
{

    char *test_vector = (char *)new_vector(sizeof(char), 0);
    vector_metadata_t *metadata = get_vector_metadata(test_vector);

    ck_assert_uint_eq(metadata->item_size, 1);
    ck_assert_uint_eq(metadata->cap, 5);
    ck_assert_uint_eq(vector_len(test_vector), 0);

    char ch = 'A';
    vector_push((void **)&test_vector, &ch); 
    metadata = get_vector_metadata(test_vector);

    ck_assert_uint_eq(metadata->cap, 5);
    ck_assert_uint_eq(vector_len(test_vector), 1);
    ck_assert_uint_eq(test_vector[0], 'A');

    free_vector(test_vector);
}
END_TEST

START_TEST(vector_test_push_method)
{

    int int_arr[11] = { 12, 19, 17, -6, 3, -18, -16, 5, -1, 10, -19 };
    int *int_vec = (int *)new_vector(sizeof(int), 0);
    vector_metadata_t *metadata;

    for (size_t i = 0; i < 11; i += 1) {
        vector_push((void **)&int_vec, &int_arr[i]);
        metadata = get_vector_metadata(int_vec);

        printf("Inedx: %zd\n", i);
        printf("\tcap: %zd\n\tlen: %zd\n\n", metadata->cap, metadata->length);
    }

    ck_assert_uint_eq(metadata->length, 11);

    free_vector(int_vec);
}
END_TEST

START_TEST(vector_test_push_front_method)
{
    int int_arr[11] = { 12, 19, 17, -6, 3, -18, -16, 5, -1, 10, -19 };
    int *int_vec = (int *)new_vector(sizeof(int), 0);

    for (size_t i = 0; i < 11; i += 1) {
        vector_push_front((void **)&int_vec, &int_arr[i]);
    }

    vector_metadata_t *metadata = get_vector_metadata(int_vec);
    ck_assert_uint_eq(metadata->length, 11);

    for (size_t i = 0; i < vector_len(int_vec); i += 1) {
        printf("[%zd]: %d\n", i, int_vec[i]);
    }
    
    free_vector(int_vec);
}
END_TEST

Suite *reader_suite() {
    Suite *suite = suite_create("Vector Tests");

    TCase *esentials = tcase_create("Esentials");

    tcase_add_test(esentials, vector_test_internals);
    tcase_add_test(esentials, vector_test_push_method);
    tcase_add_test(esentials, vector_test_push_front_method);
    
    suite_add_tcase(suite, esentials);

    return suite;
}

 int main()
 {
    Suite *suite = reader_suite(); 
    SRunner *srunner = srunner_create(suite);

    srunner_run_all(srunner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(srunner);
    srunner_free(srunner);
    return (number_failed == 0) ? 0 : 1;
 }
