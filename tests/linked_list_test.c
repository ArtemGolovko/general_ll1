#define TEST

#include <check.h>

#include <stdio.h>

#include "../src/datastructs/linked_list.h"

START_TEST(linked_list_test)
{
    int int_arr[11] = { 12, 19, 17, -6, 3, -18, -16, 5, -1, 10, -19 };
    void *my_list = new_linked_list(sizeof(int));

    for (size_t i = 0; i < 11; i += 1) {
        linked_list_push(my_list, &int_arr[i]);
    }
    ck_assert_uint_eq(linked_list_len(my_list), 11);

    int item;

    while (linked_list_len(my_list) > 0) {
        linked_list_pop(my_list, &item);
        printf("Poped item: %d\n", item);
    }


    ck_assert_uint_eq(linked_list_len(my_list), 0);

    free_linked_list(my_list);
}
END_TEST

Suite *linked_list_suite() {
    Suite *suite = suite_create("Vector Tests");

    TCase *esentials = tcase_create("Esentials");

    tcase_add_test(esentials, linked_list_test);
    
    suite_add_tcase(suite, esentials);

    return suite;
}

 int main()
 {
    Suite *suite = linked_list_suite(); 
    SRunner *srunner = srunner_create(suite);

    srunner_run_all(srunner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(srunner);
    srunner_free(srunner);
    return (number_failed == 0) ? 0 : 1;
 }
