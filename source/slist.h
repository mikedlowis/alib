/**
  @brief Intrusive singly-linked list implementation.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/

typedef struct slist_node_t {
    struct slist_node_t* next;
} slist_node_t;

typedef struct {
    slist_node_t* head;
} slist_t;

static void slist_init(slist_t* list) {
    list->head = NULL;
}

static bool slist_empty(slist_t* list) {
    return (list->head == NULL);
}

static size_t slist_size(slist_t* list) {
    size_t sz = 0;
    slist_node_t* node = list->head;
    while (node != NULL) {
        sz++;
        node = node->next;
    }
    return sz;
}

static slist_node_t* slist_front(slist_t* list) {
    return list->head;
}

static void slist_push_front(slist_t* list, slist_node_t* node) {
    node->next = list->head;
    list->head = node;
}

static slist_node_t* slist_pop_front(slist_t* list) {
    slist_node_t* node = list->head;
    list->head = node->next;
    node->next = NULL;
    return node;
}

static slist_node_t* slist_back(slist_t* list) {
    slist_node_t* node = list->head;
    while (node && node->next != NULL)
        node = node->next;
    return (node) ? node : NULL;
}

static void slist_push_back(slist_t* list, slist_node_t* node) {
    slist_node_t* back = slist_back(list);
    if (NULL != back)
        back->next = node;
    else
        list->head = node;
    node->next = NULL;
}

static slist_node_t* slist_pop_back(slist_t* list) {
    slist_node_t* prev = NULL;
    slist_node_t* curr = list->head;
    while (curr && curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }
    if (prev != NULL)
        prev->next = NULL;
    if (list->head == curr)
        list->head = NULL;
    return curr;
}

static bool slist_node_has_next(slist_node_t* node) {
    return (node->next != NULL);
}

static slist_node_t* slist_node_next(slist_node_t* node) {
    return node->next;
}

#define slist_foreach(elem, list) \
    for(slist_node_t* elem = slist_front(list); elem != NULL; elem = elem->next)
