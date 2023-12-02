# Macro Heap Library

The Macro Heap Library provides a comprehensive set of macros for creating and manipulating min heaps and max heaps in C. This library is particularly useful for efficient sorting and priority queue implementations.

## Features

- **Generic Heap Structures**: Define heap data structures for various data types.
- **Min and Max Heap Operations**: Supports both min heap and max heap operations.
- **Flexible Comparison Functions**: Allows for custom comparison functions.
- **Efficient Heap Algorithms**: Includes macros for heapify, sort, push, pop, and replace.

### Example Usage

Here's a simple usage example demonstrating min heap operations:

```c
#include <stdio.h>
#include <stdlib.h>

#include "the-macro-library/macro_heap.h"

// create the my_heap structure type
macro_heap_t(my_heap, int);

// create push, pop, and replace functions
macro_min_heap_push(push_into_min_heap, less, int, my_heap);
macro_min_heap_pop(pop_from_min_heap, less, int, my_heap);
macro_min_heap_replace(replace_in_min_heap, less, int, my_heap);

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
```

## Customization

- **Comparison Function**: Use custom comparison functions to handle complex data types or specific sorting criteria.

## Limitations

- The library requires C preprocessor capabilities.
- Memory management for dynamic heap resizing needs to be handled externally.

## License

The Macro Heap Library is licensed under the Apache License, Version 2.0. See the [LICENSE](http://www.apache.org/licenses/LICENSE-2.0) for more details.

---

This README provides an overview of the Macro Heap Library, including its features, usage, and a simple example. The example demonstrates basic min heap operations with integers. You can modify the example and the library according to your specific needs and data types.
