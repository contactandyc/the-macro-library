#include "macro_sort.h"

_macro_sort_compare(sort_ints, arg_cmp, int);

int compare_ints(void *arg, const int *a, const int *b) {
    return *a - *b;
}

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    void *arg = NULL;
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), compare_ints, arg);
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
