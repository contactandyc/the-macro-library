#include "the-macro-library/macro_sort.h"

// the default macro_sort_compare expects a less function, use _macro_sort_compare to customize!
_macro_sort_compare(sort_ints, cmp_no_arg, int);

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), compare_ints);
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
