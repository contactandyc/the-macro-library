#include "macro_bsearch.h"

static inline
int compare_int(const int *a, const int *b) {
    return *a - *b;
}

macro_bsearch_cmp_no_arg(search_ints, int, int, compare_int);
macro_bsearch_floor_cmp_no_arg(search_floor_ints, int, int, compare_int);
macro_bsearch_lower_bound_cmp_no_arg(search_lower_bound_ints, int, int, compare_int);
macro_bsearch_first_cmp_no_arg(search_first_ints, int, int, compare_int);
macro_bsearch_upper_bound_cmp_no_arg(search_upper_bound_ints, int, int, compare_int);
macro_bsearch_ceiling_cmp_no_arg(search_ceiling_ints, int, int, compare_int);
macro_bsearch_last_cmp_no_arg(search_last_ints, int, int, compare_int);

int main() {
    int arr[] = { 1, 1, 3, 3, 5, 5 };
    int n = sizeof(arr) / sizeof(arr[0]);


    int arr[] = { 1, 3, 5, 5, 5, 5, 5, 5, 7, 9, 9, 11, 13 };
    int key = 5;
    int *r = search_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "search: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_lower_match_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_match: %d found at position %lu\n", r[0], r-arr);
    key = 0;
    r = search_lower_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_bound: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_lower_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_bound: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_lower_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_bound: %d found at position %lu\n", r[0], r-arr);
    key = 15;
    r = search_lower_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "lower_bound: %d found at position %lu\n", r[0], r-arr);
    key = 10;
    r = search_upper_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_bound: %d found at position %lu\n", r[0], r-arr);
    key = 13;
    r = search_upper_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_bound: %d found at position %lu\n", r[0], r-arr);
    key = 14;
    r = search_upper_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_bound: %d found at position %lu\n", r[0], r-arr);
    key = 5;
    r = search_upper_match_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_match: %d found at position %lu\n", r[0], r-arr);
    key = 0;
    r = search_upper_bound_ints(&key, arr, sizeof(arr)/sizeof(arr[0]));
    if(r)
        printf( "upper_bound: %d found at position %lu\n", r[0], r-arr);

    return 0;
}
