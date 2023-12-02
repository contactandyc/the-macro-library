/*
Copyright (c) 2023 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _macro_heap_sort_H
#define _macro_heap_sort_H

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/src/macro_swap.h"

#define macro_max_heapify(style, type, cmp,                                    \
                      p, endp,                                                 \
                      iter, i, largest, left, right )                          \
    i = iter;                                                                  \
    while (i < endp) {                                                         \
        left = p + (2 * (i - p) + 1);                                          \
        right = left+1;                                                        \
        largest = i;                                                           \
        if (left < endp && (macro_less(style, type, cmp, largest, left)))      \
            largest = left;                                                    \
        if (right < endp && (macro_less(style, type, cmp, largest, right)))    \
            largest = right;                                                   \
        if (largest != i) {                                                    \
            macro_swap(largest, i);                                            \
            i = largest;                                                       \
        } else                                                                 \
            break;                                                             \
    }

#define macro_min_heapify(style, type, cmp,                                   \
                          p, endp,                                            \
                          iter, i, smallest, left, right )                    \
    i = iter;                                                                 \
    while (i < endp) {                                                        \
        left = p + (2 * (i - p) + 1);                                         \
        right = left + 1;                                                     \
        smallest = i;                                                         \
        if (left < endp && macro_less(style, type, cmp, left, smallest))      \
            smallest = left;                                                  \
        if (right < endp && macro_less(style, type, cmp, right, smallest))    \
            smallest = right;                                                 \
        if (smallest != i) {                                                  \
            macro_swap(smallest, i);                                          \
            i = smallest;                                                     \
        } else                                                                \
            break;                                                            \
    }

#define macro_heap_sort(style, type, cmp, arr, num,             \
                        iter, i, largest, left, right, endp)    \
    endp = arr + (num);                                         \
    iter = arr + (num) / 2 - 1;                                 \
    while(iter >= arr) {                                        \
        macro_max_heapify(style, type, cmp, arr, endp,          \
                      iter, i, largest, left, right )           \
        iter--;                                                 \
    }                                                           \
    endp = endp - 1;                                            \
    while(endp >= arr) {                                        \
        macro_swap(arr, endp);                                  \
        macro_max_heapify(style, type, cmp, arr, endp,          \
                      arr, i, largest, left, right )            \
        endp--;                                                 \
    }

#define __macro_min_heap_push_code(style, type, cmp, arr, num)                \
    ssize_t i = num;                                                          \
    num++;                                                                    \
    ssize_t parent = (i - 1) / 2;                                             \
    while (i > 0 && macro_less(style, type, cmp, arr + i, arr + parent)) {    \
        macro_swap(arr + i, arr + parent);                                    \
        i = parent;                                                           \
        parent = (i - 1) / 2;                                                 \
    }

#define __macro_max_heap_push_code(style, type, cmp, arr, num)                \
    ssize_t i = num;                                                          \
    num++;                                                                    \
    ssize_t parent = (i - 1) / 2;                                             \
    while (i > 0 && macro_less(style, type, cmp, arr + parent, arr + i)) {    \
        macro_swap(arr + i, arr + parent);                                    \
        i = parent;                                                           \
        parent = (i - 1) / 2;                                                 \
    }

#define __macro_min_heap_pop_code(style, type, cmp, arr, num)            \
    type *iter, *smallest, *left, *right;                                \
    type tmp;                                                            \
    num--;                                                               \
    macro_swap(arr, arr + num);                                          \
    macro_min_heapify(style, type, cmp, arr, arr + num, arr,           \
                        iter, smallest, left, right)

#define __macro_max_heap_pop_code(style, type, cmp, arr, num)            \
    type *iter, *largest, *left, *right;                                 \
    type tmp;                                                            \
    num--;                                                               \
    macro_swap(arr, arr + num);                                        \
    macro_max_heapify(style, type, cmp, arr, arr + num, arr,           \
                        iter, largest, left, right)

#define macro_check_min_heap_zero(style, type, cmp, arr, num) \
    if(num <= 1 || macro_less(style, type, cmp, arr, arr + 1)) return; \
    if(num <= 2 || macro_less(style, type, cmp, arr, arr + 2)) return

#define macro_check_max_heap_zero(style, type, cmp, arr, num) \
    if(num <= 1 || macro_less(style, type, cmp, arr + 1, arr)) return; \
    if(num <= 2 || macro_less(style, type, cmp, arr + 2, arr)) return

#endif /* _macro_heap_sort_H */