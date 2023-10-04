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

#ifndef _macro_introsort_H
#define _macro_introsort_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>


#include "the-macro-library/src/macro_check_sorted.h"
#include "the-macro-library/src/macro_dutch_flag_partition.h"
#include "the-macro-library/src/macro_isort.h"
#include "the-macro-library/src/macro_heap_sort.h"
#include "the-macro-library/src/macro_swap.h"
#include "the-macro-library/src/macro_vecswap.h"

#include "the-macro-library/macro_cmp.h"

#ifdef __linux__
#ifdef __LP64__
#define __mcro_introsort_max_depth(n) depth_limit = (flsl(n) - 1) << 1
#else
#define __mcro_introsort_max_depth(n) depth_limit = (fls(n) - 1) << 1
#endif
#elif __FreeBSD__
#ifdef __LP64__
#define __mcro_introsort_max_depth(n) depth_limit = (flsl(n) - 1) << 1
#else
#define __mcro_introsort_max_depth(n) depth_limit = (fls(n) - 1) << 1
#endif
#else
#define __mcro_introsort_max_depth(n)    \
    tmp_n = n;                           \
    depth_limit = 0;                     \
    while (tmp_n >>= 1)                  \
        depth_limit++;                   \
    depth_limit <<= 1
#endif

/*
    __macro_introsort_stack_t is used to avoid recursion.  Because this ends up being a
    macro, it is important that no recursion occurs.
*/
typedef struct {
  void *base;
  ssize_t n;
} __macro_introsort_stack_t;

#define __macro_push_and_loop(top, arr, num,                   \
                              left, left_n, right, right_n,    \
                              loop, pop)                       \
    if(left_n > 1) {                                           \
        top->base = left;                                      \
        top->n = left_n;                                       \
        top++;                                                 \
        if(right_n > 1) {                                      \
            arr = right;                                       \
            num = right_n;                                     \
            goto loop;                                         \
        }                                                      \
        else goto pop;                                         \
    }                                                          \
    else if(right_n > 1) {                                     \
        arr = right;                                           \
        num = right_n;                                         \
        goto loop;                                             \
    }                                                          \
    else {                                                     \
        goto pop;                                              \
    }

#define __macro_mid3(style, type, cmp, a, b, c)       \
    if (macro_less(style, type, cmp, a, b)) {         \
        if(macro_less(style, type, cmp, c, b)) {      \
            if(macro_less(style, type, cmp, a, c))    \
                b = c;                                \
            else                                      \
                b = a;                                \
        }                                             \
    }                                                 \
    else if(macro_less(style, type, cmp, a, c))       \
        b = a;                                        \
    else if(macro_less(style, type, cmp, b, c))       \
        b = c;

#define __macro_pivot_ninther(style, type, cmp)    \
    delta = n/8;                                   \
    a = lo+delta;                                  \
    b = a+delta;                                   \
    c = b+delta;                                   \
    mid = c+delta;                                 \
    d = mid+delta;                                 \
    e = d+delta;                                   \
    f = e+delta;                                   \
    __macro_mid3(style, type, cmp, lo, a, b)       \
    __macro_mid3(style, type, cmp, c, mid, d)      \
    __macro_mid3(style, type, cmp, e, f, hi)       \
    __macro_mid3(style, type, cmp, a, mid, f)

#define __macro_pivot_5ther(style, type, cmp)    \
    delta = n>>2;                                \
    a = lo+delta;                                \
    b = mid+delta;                               \
    __macro_mid3(style, type, cmp, lo, a, b)     \
    __macro_mid3(style, type, cmp, a, mid, hi)

#define __macro_lo_mid_hi()    \
    lo = base;                 \
    mid = lo + (n >> 1);       \
    hi = lo+(n-1);

/*
    common variables needed for the macro_introsorts.  The stack depth of 64 should be
    okay on 64 bit systems because only the smaller segment is pushed onto the
    stack.  Once a segment is less than 6 elements, a predefined sort is used.

    add
    type tmp_items[5]; to support macro_introsort_experimental
*/
#define __macro_introsort_vars(type)                            \
    type* c; type* d; type* f; type* lo; type* mid; type* hi;   \
    ssize_t left_n, right_n, tmp_n, delta, elem_size;    \
    int depth_limit;                                            \
    __macro_introsort_stack_t stack[64];                        \
    __macro_introsort_stack_t *top = stack;                     \
    top->base = NULL;                                           \
    top++;                                                      \
    int cur_depth = 0

#define __macro_introsort_ivars(type)    \
    type* a; type* b; type* e;                     \
    type tmp

#define __macro_introsort_code(style, type, cmp)                  \
    __macro_introsort_ivars(type);                                \
    if(n < 17) {                                                  \
        macro_micro_check_reverse_on(style, type, cmp, base, n, a, b)  \
        macro_isort(style, type, cmp, base, n, e, a, b, tmp );    \
        return;                                                   \
    }                                                             \
    __macro_introsort_vars(type);                                 \
    __mcro_introsort_max_depth(n);                                \
    macro_check_sorted(style, type, cmp,                          \
                       base, n,                                   \
                       lo, mid, hi,                               \
                       delta, a, b,                 \
                       find_pivot, partition)                     \
    return;                                                       \
hi_mid_low:;                                                      \
    __macro_lo_mid_hi();                                          \
find_pivot:;                                                      \
    if(n > 40) {                                                  \
        __macro_pivot_ninther(style, type, cmp);                  \
    } else {                                                      \
        __macro_pivot_5ther(style, type, cmp);                    \
    }                                                             \
partition:;                                                       \
    if(cur_depth < depth_limit) {                                 \
        macro_dutch_flag_partition(qs, style, type, cmp,          \
                                   lo, mid, hi,                   \
                                   a, b, c, d,                    \
                                   left_n, right_n, tmp_n)        \
        __macro_push_and_loop(top, base, n,                       \
                              b, left_n, c, right_n,              \
                              loop, pop_stack);                   \
    } else {                                                      \
        a = base;                                                 \
        macro_heap_sort(style, type, cmp, base, n,                \
                        a, b, c, d, e, f)                         \
    }                                                             \
pop_stack:;                                                       \
    top--;                                                        \
    base = (type *)top->base;                                     \
    if (!base) return;                                            \
    n = top->n;                                                   \
    cur_depth = top-stack;                                        \
loop:;                                                            \
    cur_depth++;                                                  \
small_sort:;                                                      \
    if(n < 17) {                                                  \
        macro_isort(style, type, cmp, base, n, e, a, b, tmp );    \
        goto pop_stack;                                           \
    }                                                             \
    goto hi_mid_low;


#define macro_introsort_h(name, style, type)    \
void name(type *base,                           \
          macro_cmp_signature(size_t n, style, type))

#define macro_introsort(name, style, type, cmp)    \
macro_introsort_h(name, style, type) {             \
    __macro_introsort_code(style, type, cmp);      \
}

#define macro_introsort_compare_h(name, style, type)    \
void name(type *base,                                   \
          macro_cmp_signature(size_t n, compare_ ## style, type))

#define macro_introsort_compare(name, style, type)    \
macro_introsort_compare_h(name, style, type) {        \
    __macro_introsort_code(style, type, cmp);         \
}

#endif

