#include "macro_bsearch.h"

static inline
int compare_int(const int *a, const int *b) {
    return *a - *b;
}

macro_bsearch(bsearch_ints, int, compare_int);
macro_bsearch_first(bsearch_first_ints, int, compare_int);
macro_bsearch_last(bsearch_last_ints, int, compare_int);
macro_bsearch_floor(bsearch_floor_ints, int, compare_int);
macro_bsearch_ceiling(bsearch_ceiling_ints, int, compare_int);
macro_bsearch_lower_bound(bsearch_lower_bound_ints, int, compare_int);
macro_bsearch_upper_bound(bsearch_upper_bound_ints, int, compare_int);

void test_bsearch(const char *test_name, int key, int *arr, int n, int *expected, bool allow_same_value, int * (*bsearch)(const int *k, const int *a, size_t n)) {
    int *r = bsearch(&key, arr, n);
    if(r) {
        if(r != expected) {
            if(allow_same_value && expected && *r == *expected) {
                printf( "success(%s): key(%d) %d at position %lu returned\n", test_name, key, *r, r-arr );
            }
            else {
                if(expected)
                    printf( "fail(%s): key(%d) %d at position %lu returned, expected %d at position %lu\n", test_name, key, *r, r-arr, *expected, expected-arr );
                else
                    printf( "fail(%s): key(%d) %d at position %lu returned, expected NULL\n", test_name, key, *r, r-arr );
            }
        }
        else
            printf( "success(%s): key(%d) %d at position %lu returned\n", test_name, key, *r, r-arr );
    }
    else {
        if(expected)
            printf( "fail(%s): key(%d) NULL returned, %d at position %lu expected\n", test_name, key, *expected, expected-arr );
        else
            printf( "success(%s): key(%d) NULL returned\n", test_name, key );
    }
}


int main() {
    // the 9999 is the value after and is meant for the upper_bound test
    // n is actually one less than the array size
    int arr[] = { 1, 3, 3, 5, 9999 };
    int n = (sizeof(arr) / sizeof(arr[0])) - 1;

    // bsearch - Finds any instance of a key or return NULL if none exist.
    test_bsearch("bsearch", 0, arr, n, NULL, true, bsearch_ints );
    test_bsearch("bsearch", 1, arr, n, arr+0, true, bsearch_ints );
    test_bsearch("bsearch", 2, arr, n, NULL, true, bsearch_ints );
    test_bsearch("bsearch", 3, arr, n, arr+1, true, bsearch_ints );
    test_bsearch("bsearch", 4, arr, n, NULL, true, bsearch_ints );
    test_bsearch("bsearch", 5, arr, n, arr+3, true, bsearch_ints );
    test_bsearch("bsearch", 6, arr, n, NULL, true, bsearch_ints );

    // bsearch_first - Finds the first instance of a key or return NULL if none exist.
    test_bsearch("bsearch_first", 0, arr, n, NULL, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 1, arr, n, arr+0, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 2, arr, n, NULL, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 3, arr, n, arr+1, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 4, arr, n, NULL, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 5, arr, n, arr+3, false, bsearch_first_ints );
    test_bsearch("bsearch_first", 6, arr, n, NULL, false, bsearch_first_ints );

    // bsearch_last - Finds the last instance of a key or return NULL if none exist.
    test_bsearch("bsearch_last", 0, arr, n, NULL, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 1, arr, n, arr+0, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 2, arr, n, NULL, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 3, arr, n, arr+2, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 4, arr, n, NULL, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 5, arr, n, arr+3, false, bsearch_last_ints );
    test_bsearch("bsearch_last", 6, arr, n, NULL, false, bsearch_last_ints );

    // bsearch_floor - Finds the first instance of a key or the value less than that.
    //    NULL is returned if the key is less than any item in the array.
    test_bsearch("bsearch_floor", 0, arr, n, NULL, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 1, arr, n, arr+0, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 2, arr, n, arr+0, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 3, arr, n, arr+1, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 4, arr, n, arr+2, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 5, arr, n, arr+3, false, bsearch_floor_ints );
    test_bsearch("bsearch_floor", 6, arr, n, arr+3, false, bsearch_floor_ints );

    // bsearch_ceiling - Finds the last instance of a key or the value less than.
    //    NULL is returned if the key is less than any item in the array.
    test_bsearch("bsearch_ceiling", 0, arr, n, NULL, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 1, arr, n, arr+0, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 2, arr, n, arr+0, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 3, arr, n, arr+2, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 4, arr, n, arr+2, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 5, arr, n, arr+3, false, bsearch_ceiling_ints );
    test_bsearch("bsearch_ceiling", 6, arr, n, arr+3, false, bsearch_ceiling_ints );

    // bsearch_lower_bound - Finds the first instance of a key or the value greater.
    //    NULL is returned if the key is greater than any item in the array.
    test_bsearch("bsearch_lower_bound", 0, arr, n, arr+0, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 1, arr, n, arr+0, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 2, arr, n, arr+1, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 3, arr, n, arr+1, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 4, arr, n, arr+3, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 5, arr, n, arr+3, false, bsearch_lower_bound_ints );
    test_bsearch("bsearch_lower_bound", 6, arr, n, NULL, false, bsearch_lower_bound_ints );

    // bsearch_upper_bound - This is different than the others in that the response is always
    //   one greater than the key. It is possible for this to extend beyond the array. The
    //   purpose of upper_bound is to be used in conjunction with lower_bound to form a range.
    test_bsearch("bsearch_upper_bound", 0, arr, n, arr+0, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 1, arr, n, arr+1, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 2, arr, n, arr+1, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 3, arr, n, arr+3, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 4, arr, n, arr+3, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 5, arr, n, arr+4, false, bsearch_upper_bound_ints );
    test_bsearch("bsearch_upper_bound", 6, arr, n, arr+4, false, bsearch_upper_bound_ints );

    return 0;
}
