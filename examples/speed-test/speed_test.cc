#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "the-macro-library/macro_sort.h"
#include "test/macro_test_sort.h"

typedef struct {
    int key;
    int key2;
    int payload[3];
} item_t;

void set_item(item_t *p, int value) {
    p->key = value >> 10;
    p->key2 = value;
    p->payload[0] = value;
}

int compare_items_for_qsort(const void *p1, const void *p2) {
    item_t *a = (item_t *)p1;
    item_t *b = (item_t *)p2;
    if(a->key != b->key)
        return (a->key < b->key) ? -1 : 1;
    if(a->key2 != b->key2)
        return (a->key2 < b->key2) ? -1 : 1;
    return 0;
}

void quick_sort(item_t *items, size_t num) {
    qsort(items, num, sizeof(item_t), compare_items_for_qsort);
}

bool compare_items_for_macro_sort(const item_t *a, const item_t *b) {
    if(a->key != b->key)
        return a->key < b->key;
    return a->key2 < b->key2;
}

macro_sort(sort_items, item_t, compare_items_for_macro_sort);

static inline
bool less_items_for_std_sort(const item_t & a, const item_t & b) {
    if(a.key != b.key)
        return a.key < b.key;
    return a.key2 < b.key2;
}

void std_sort(item_t *items, size_t num) {
    std::sort(items, items+num, less_items_for_std_sort);
}

int main( int argc, char *argv[]) {
    if(argc < 2) {
        printf(macro_test_sort_driver_usage());
        return -1;
    }
    int size = 100000;
    int rep = 100;
    __macro_get_int_param(argc, argv, 1, &size);
    __macro_get_int_param(argc, argv, 2, &rep);
    item_t *arr = (item_t *)malloc(sizeof(item_t) * size);
    item_t *arr2 = (item_t *)malloc(sizeof(item_t) * size);
    argc -= 3;
    argv += 3;

    macro_test_sort_driver(arr, arr2, size, rep, set_item,
                           "macro_sort", sort_items,
                           "std_sort", std_sort,
                           "qsort", quick_sort,
                           less_no_arg, item_t, compare_items_for_macro_sort, argv, argc);

    free(arr);
    free(arr2);
    return 0;
}
