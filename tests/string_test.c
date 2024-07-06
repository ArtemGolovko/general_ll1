#define TEST

#include <check.h>

#include <string.h>
#include <stdio.h>

#include "../src/datastructs/string.h"


START_TEST(string_test)
{
    char *test_string = new_string(0);

    for (char i = 'a'; i <= 'z'; i += 1) {
        string_push(&test_string, i);
    }
    printf("Resulting string: %s\nlength: %zd\nstrlen(): %zd\n", test_string, string_len(test_string), strlen(test_string));

    free_string(test_string);
}
END_TEST

Suite *linked_list_suite() {
    Suite *suite = suite_create("String Tests");

    TCase *esentials = tcase_create("Esentials");

    tcase_add_test(esentials, string_test);
    
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
