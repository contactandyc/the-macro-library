#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "the-macro-library/macro_sort.h"
#include "test/macro_test_sort.h"

class item_t {
public:
    int key;
    int key2;
    int payload[3];

    item_t() : key(0), key2(0) {}

    bool operator<(const item_t& other) const {
        if(key != other.key)
            return key < other.key;
        return key2 < other.key2;
    }
};

void set_item(item_t *p, int value) {
    p->key = value >> 10;
    p->key2 = value;
    p->payload[0] = value;
}
/*
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
*/

static inline
bool compare_items_for_macro_sort(const item_t *a, const item_t *b) {
    if(a->key != b->key)
        return a->key < b->key;
    return a->key2 < b->key2;
}

macro_sort(sort_items_with_compare, item_t, compare_items_for_macro_sort);

static inline
bool less_items_for_std_sort(const item_t & a, const item_t & b) {
    if(a.key != b.key)
        return a.key < b.key;
    return a.key2 < b.key2;
}

void std_sort_with_compare(item_t *items, size_t num) {
    std::sort(items, items+num, less_items_for_std_sort);
}


// _macro_sort(sort_items, less, item_t, not_used);

void std_sort(item_t *items, size_t num) {
    std::sort(items, items+num);
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
    argc -= 3;
    argv += 3;

    macro_test_sort_driver(size, rep, set_item,
                           "macro_sort (with compare)", sort_items_with_compare,
                           "std::sort", std_sort,
                           "std::sort (with compare)", std_sort_with_compare,
                           less_no_arg, item_t, compare_items_for_macro_sort, argv, argc, cc);
    return 0;
}
