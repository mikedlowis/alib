/**
    Intrusive doubly-linked list implementation.
    
    Copyright 2017, Michael D. Lowis
    
    Permission to use, copy, modify, and/or distribute this software
    for any purpose with or without fee is hereby granted, provided
    that the above copyright notice and this permission notice appear
    in all copies.
    
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
*/

typedef struct list_node_t {
    struct list_node_t* next;
    struct list_node_t* prev;
} list_node_t;

typedef struct {
    list_node_t* head;
    list_node_t* tail;
} list_t;

static void list_init(list_t* list) {
    list->head = NULL;
    list->tail = NULL;
}

static bool list_empty(list_t* list) {
    return (list->head == NULL);
}

static size_t list_size(list_t* list) {
    size_t sz = 0;
    list_node_t* node = list->head;
    while (node != NULL) {
        sz++;
        node = node->next;
    }
    return sz;
}

static list_node_t* list_front(list_t* list) {
    return list->head;
}

static void list_push_front(list_t* list, list_node_t* node) {
    node->prev = NULL;
    node->next = list->head;
    list->head = node;
    if (list->tail == NULL)
        list->tail = node;
}

static list_node_t* list_pop_front(list_t* list) {
    list_node_t* node = list->head;
    list->head = node->next;
    if (list->head == NULL)
        list->tail = NULL;
    node->next = NULL;
    return node;
}

static list_node_t* list_back(list_t* list) {
    return list->tail;
}

static void list_push_back(list_t* list, list_node_t* node) {
    node->next = NULL;
    node->prev = list->tail;
    list->tail = node;
    if (list->head == NULL)
        list->head = node;
}

static list_node_t* list_pop_back(list_t* list) {
    list_node_t* node = list->tail;
    list->tail = node->prev;
    if (list->tail == NULL)
        list->head = NULL;
    node->prev = NULL;
    return node;
}

static bool list_node_has_next(list_node_t* node) {
    return (node->next != NULL);
}

static list_node_t* list_node_next(list_node_t* node) {
    return node->next;
}

static bool list_node_has_prev(list_node_t* node) {
    return (node->prev != NULL);
}

static list_node_t* list_node_prev(list_node_t* node) {
    return node->prev;
}
