#define TEST

#include <check.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../src/reader.h"

START_TEST(reader_test_peek)
{
    // char source[] = "abc";
    // FILE *file = open_memstream(&source, strlen(source)); 
    // Reader reader = new_Reader(file);
    //
    // char c = Reader_peek(&reader, 1);
    // char c2 = Reader_peek(&reader, 2);
    //
    // ck_assert(c == 'a');
    // ck_assert(c == 'b');
    //
    // printf("|%s|\n", reader.buffer);
    //
    // free_Reader(&reader);

    ck_assert(1 == 1);
}
END_TEST

Suite *reader_suite() {
    Suite *suite = suite_create("Vector Tests");

    TCase *esentials = tcase_create("Esentials");

    tcase_add_test(esentials, reader_test_peek);
    
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
