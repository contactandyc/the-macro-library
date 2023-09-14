#include "macro_bsearch.h"

static inline
int compare_int(const int *a, const int *b) {
    return *a - *b;
}

macro_bsearch_cmp_no_arg(search_ints, int, int, compare_int);
macro_bsearch_lower_bound_cmp_no_arg(search_lower_bound_ints, int, int, compare_int);
macro_bsearch_lower_match_cmp_no_arg(search_lower_match_ints, int, int, compare_int);
macro_bsearch_upper_bound_cmp_no_arg(search_upper_bound_ints, int, int, compare_int);
macro_bsearch_upper_match_cmp_no_arg(search_upper_match_ints, int, int, compare_int);

int main() {
    int arr[] = { 1, 3, 5, 5, 5, 5, 5, 5, 7, 9, 9, 11, 13 };
    int key = 5;
    int *r = search_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "search: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_lower_match_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_match: %d found at position %lu\n", r[0], r-arr);
    key = 4;
    r = search_lower_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_bound: %d found at position %lu\n", r[0], r-arr);
    key = 10;
    r = search_upper_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_bound: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_upper_match_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_match: %d found at position %lu\n", r[0], r-arr);
    return 0;
}
