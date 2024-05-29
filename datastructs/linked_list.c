#include "linked_list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t item_size;

    void *head;
    void *tail;

    size_t length;
} linked_list_t;

typedef struct {
    void *next;
    void *prev;
} node_metadata_t;

void *new_linked_list(size_t item_size) {
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));

    list->item_size = item_size;
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}


void free_linked_list(void *list) {
    linked_list_t *linked_list = list;

    void *head = linked_list->head;
    void *tail = linked_list->tail;

    while(head != tail) {
        node_metadata_t *metadata = head;    
        void *next_head = metadata->next;

        free(head);
        head = next_head;
    }

    free(tail);
    free(list);
}

void linked_list_push(void *list, const void *item) {
    linked_list_t *linked_list = list;

    void *node = malloc(sizeof(node_metadata_t) + linked_list->item_size);
    char *node_begin = node;

    memcpy(node_begin + sizeof(node_metadata_t), item, linked_list->item_size);

    if (linked_list->tail != NULL) {
        node_metadata_t *tail_metadata = linked_list->tail; 
        node_metadata_t *node_metadata = node;

        tail_metadata->next = node;
        node_metadata->prev = linked_list->tail;
        node_metadata->next = NULL;

        linked_list->tail = node;

    } else {
        node_metadata_t *node_metadata = node;

        node_metadata->prev = NULL;
        node_metadata->next = NULL;

        linked_list->head = node;
        linked_list->tail = node;
    }

    linked_list->length += 1;
}

void linked_list_pop(void *list, void *item) {
    linked_list_t *linked_list = list;

    assert(linked_list->length > 0);

    char *tail_begin = linked_list->tail;
    memcpy(item, tail_begin + sizeof(node_metadata_t), linked_list->item_size);

    node_metadata_t *tail_metadata = linked_list->tail;

    if (tail_metadata->prev == NULL) {
        free(linked_list->tail);
        
        linked_list->head = NULL;
        linked_list->tail = NULL;

        linked_list->length = 0;

        return;
    }

    node_metadata_t *prev_metadata = tail_metadata->prev;
    
    prev_metadata->next = NULL;
    free(linked_list->tail);

    linked_list->tail = (void *)prev_metadata;
    linked_list -= 1;
}

size_t linked_list_len(void *list) {
    linked_list_t *linked_list = list;

    return linked_list->length;
}
