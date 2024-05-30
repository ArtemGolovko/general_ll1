#include <stdio.h>
#define TEST

#include "linked_list.h"

int main() {
    int int_arr[11] = { 12, 19, 17, -6, 3, -18, -16, 5, -1, 10, -19 };
    void *my_list = new_linked_list(sizeof(int));

    for (size_t i = 0; i < 11; i += 1) {
        linked_list_push(my_list, &int_arr[i]);
    }

    printf("Linked list length: %zd\n", linked_list_len(my_list));

    int item;

    while (linked_list_len(my_list) > 0) {
        linked_list_pop(my_list, &item);
        printf("Poped item: %d\n", item);
    }

    free_linked_list(my_list);

    return 0;
}
