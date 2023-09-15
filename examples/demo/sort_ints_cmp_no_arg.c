#include "macro_sort.h"

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

_macro_sort(sort_ints, cmp_no_arg, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]));
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
