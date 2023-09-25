#include <stdio.h>

void reverse(int *arr, int size) {
    int tmp;
    int *p = arr;
    int *ep = arr+size-1;
    while(p < ep) {
        tmp = *p;
        *p = *ep;
        *ep = tmp;
        p++;
        ep--;
    }
}

int main( int argc, char *argv[] ) {
    int arr[] = { 5, 4, 3, 2, 1 };
    int size = sizeof(arr) / sizeof(arr[0]);
    reverse(arr, size);
    for( int i=0; i<size; i++ )
       printf( "%d ", arr[i] );
    printf( "\n" );
    return 0;
}