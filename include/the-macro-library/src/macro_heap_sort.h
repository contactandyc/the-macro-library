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

#define macro_heapify(style, type, cmp,                                        \
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

#define macro_heap_sort(style, type, cmp, arr, num,             \
                        iter, i, largest, left, right, endp)    \
    endp = arr + (num);                                         \
    iter = arr + (num) / 2 - 1;                                 \
    while(iter >= arr) {                                        \
        macro_heapify(style, type, cmp, arr, endp,              \
                      iter, i, largest, left, right )           \
        iter--;                                                 \
    }                                                           \
    endp = endp - 1;                                            \
    while(endp >= arr) {                                        \
        macro_swap(arr, endp);                                  \
        macro_heapify(style, type, cmp, arr, endp,              \
                      arr, i, largest, left, right )            \
        endp--;                                                 \
    }

#endif /* _macro_heap_sort_H */