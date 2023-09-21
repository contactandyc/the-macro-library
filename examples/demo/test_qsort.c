#include <stdio.h>

int compare_ints(const void *p1, const void *p2) {
    int *a = (int *)p1;
    int *b = (int *)p2;
    return *a - *b;
}

int main( int argc, char *argv[] ) {
    int arr[] = { 5, 4, 3, 2, 1 };
    /* The array size is the size of each int * the number of elements.
       The following code gets the number of elements. */
    int n = sizeof(arr) / sizeof(arr[0]);

    /* sort the array */
    qsort(arr, n, sizeof(int), compare_ints);

    /* print the array */
    for( int i=0; i<n; i++ ) {
        printf( "%d ", arr[i] );
    }
    /* add a new line after all of the elements */
    printf( "\n" );
    return 0;
}
