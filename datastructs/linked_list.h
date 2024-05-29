#ifndef DATASTRUCTS_LINKED_LIST_H
#define DATASTRUCTS_LINKED_LIST_H


void *new_linked_list(size_t item_size);
void free_linked_list(void *list);

void linked_list_push(void *list, const void *item);
void linked_list_pop(void *list, void *item);

size_t linked_list_len(void *list);

#endif
