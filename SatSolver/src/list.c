#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
    while (s->head != NULL) {
        ListItem* to_be_removed = s->head;
        s->head = to_be_removed->next;
        free(to_be_removed);
    }
}

void push(List* s, void* data) {
    ListItem* new_element = malloc(sizeof(*new_element));
    new_element->data = data;
    new_element->next = s->head;
    s->head = new_element;
}

void* peek(List* s) {
    if (s->head != NULL) {
        return s->head->data;
    } else
        err("empty list peek");
    return NULL;
}

void pop(List* s) {
    ListItem* to_be_removed = s->head;
    if (to_be_removed != NULL) {
        s->head = to_be_removed->next;
        free(to_be_removed);
    } else {
        err("empty list push");
    }
}

char isEmpty(List* s) { return (s->head == NULL); }

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }
