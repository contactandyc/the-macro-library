#include <stdio.h>
#include <stdlib.h>

#include "the-macro-library/macro_heap.h"

// create the my_heap structure type
macro_heap_t(my_heap, int);

// create push, pop, and replace functions
macro_min_heap_push(push_into_min_heap, less, int, na, my_heap);
macro_min_heap_pop(pop_from_min_heap, less, int, na, my_heap);
macro_min_heap_replace(replace_in_min_heap, less, int, na, my_heap);

int main() {
    // Define and initialize a min heap
    my_heap heap;
    heap.arr = malloc(10 * sizeof(int)); // Initial allocation
    heap.num = 0;
    heap.size = 10;

    // Push elements into the min heap
    int elements[] = {5, 3, 6, 2, 8};
    for (int i = 0; i < 5; i++) {
        heap.arr[heap.num] = elements[i];
        push_into_min_heap(&heap);
    }

    // Pop and print elements from the min heap
    printf("Min Heap Elements (in ascending order):\n");
    while (heap.num > 0) {
        int* top = pop_from_min_heap(&heap);
        if (top) {
            printf("%d ", *top);
        }
    }

    // Clean up
    free(heap.arr);
    return 0;
}