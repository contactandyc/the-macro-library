#include "macro_sort.h"

bool compare_ints(const int *a, const int *b, void *arg) {
    return *a < *b;
}

// the default macro_sort expects a less function without an arg, use _macro_sort to customize!
_macro_sort(sort_ints, less_arg, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), NULL);
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
