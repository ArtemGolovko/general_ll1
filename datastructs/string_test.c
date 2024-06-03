#define TEST

#include "string.h"

#include <stdio.h>
#include <string.h>

int main() {
    char *test_string = new_string(0);

    for (char i = 'a'; i <= 'z'; i += 1) {
        string_push(&test_string, i);
    }

    printf("Resulting string: %s\nlength: %zd\nstrlen(): %zd\n", test_string, string_len(test_string), strlen(test_string));

    free_string(test_string);

    return 0;
}
