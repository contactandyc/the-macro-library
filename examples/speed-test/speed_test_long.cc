#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "the-macro-library/macro_sort.h"
#include "test/macro_test_sort.h"

typedef long item_t;

void set_item(item_t *p, int value) {
    *p = value;
}

int compare_items_for_qsort(const void *p1, const void *p2) {
    item_t *a = (item_t *)p1;
    item_t *b = (item_t *)p2;
    if(*a != *b)
        return (*a < *b) ? -1 : 1;
    return 0;
}

void quick_sort(item_t *items, size_t num) {
    qsort(items, num, sizeof(item_t), compare_items_for_qsort);
}

_macro_sort(sort_items, less, item_t, not_used);

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
                           "macro_sort", sort_items,
                           "std::sort", std_sort,
                           "qsort", quick_sort,
                           less, item_t, not_used, argv, argc, c);
    return 0;
}
