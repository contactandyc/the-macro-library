#include "the-macro-library/macro_sort.h"

int compare_ints(void *arg, const int *a, const int *b) {
    return *a - *b;
}

// the default macro_sort expects a less function, use _macro_sort to customize!
_macro_sort(sort_ints, arg_cmp, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    void *arg = NULL;
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), arg);
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
