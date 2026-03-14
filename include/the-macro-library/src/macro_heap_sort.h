// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#ifndef _macro_heap_sort_H
#define _macro_heap_sort_H

#include <stddef.h> /* size_t */
#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/src/macro_swap.h"

#define macro_max_heapify(style, type, cmp,                                        \
                          p, endp,                                                 \
                          iter, i, largest, left, right )                          \
    i = iter;                                                                      \
    while ((i) < (endp)) {                                                         \
        left   = (p) + (2 * ((i) - (p)) + 1);                                      \
        right  = (left) + 1;                                                       \
        largest = (i);                                                             \
        if ((left)  < (endp) && (macro_less(style, type, cmp, largest, left)))     \
            largest = (left);                                                      \
        if ((right) < (endp) && (macro_less(style, type, cmp, largest, right)))    \
            largest = (right);                                                     \
        if (largest != (i)) {                                                      \
            macro_swap(largest, i);                                                \
            (i) = largest;                                                         \
        } else                                                                     \
            break;                                                                 \
    }

#define macro_min_heapify(style, type, cmp,                                       \
                          p, endp,                                                \
                          iter, i, smallest, left, right )                        \
    i = iter;                                                                     \
    while ((i) < (endp)) {                                                        \
        left     = (p) + (2 * ((i) - (p)) + 1);                                   \
        right    = (left) + 1;                                                    \
        smallest = (i);                                                           \
        if ((left)  < (endp) && macro_less(style, type, cmp, left, smallest))     \
            smallest = (left);                                                    \
        if ((right) < (endp) && macro_less(style, type, cmp, right, smallest))    \
            smallest = (right);                                                   \
        if (smallest != (i)) {                                                    \
            macro_swap(smallest, i);                                              \
            (i) = smallest;                                                       \
        } else                                                                    \
            break;                                                                \
    }

#define macro_heap_sort(style, type, cmp, arr, num,             \
                        iter, i, largest, left, right, endp)    \
    endp = (arr) + (num);                                       \
    iter = (arr) + (num) / 2 - 1;                               \
    while ((iter) >= (arr)) {                                   \
        macro_max_heapify(style, type, cmp, (arr), (endp),      \
                          iter, i, largest, left, right)        \
        (iter)--;                                               \
    }                                                           \
    (endp) = (endp) - 1;                                        \
    while ((endp) >= (arr)) {                                   \
        macro_swap((arr), (endp));                              \
        macro_max_heapify(style, type, cmp, (arr), (endp),      \
                          (arr), i, largest, left, right)       \
        (endp)--;                                               \
    }

/* ---------- Push/Pop primitives (public heap API builds on these) ---------- */

#define __macro_min_heap_push_code(style, type, cmp, arr, num)               \
    size_t i__ = (num);                                                      \
    (num)++;                                                                 \
    while (i__ > 0) {                                                        \
        size_t parent__ = (i__ - 1) / 2;                                     \
        if (!macro_less(style, type, cmp, (arr) + i__, (arr) + parent__))    \
            break;                                                           \
        macro_swap((arr) + i__, (arr) + parent__);                           \
        i__ = parent__;                                                      \
    }                                                                        \

#define __macro_max_heap_push_code(style, type, cmp, arr, num)               \
    size_t i__ = (num);                                                      \
    (num)++;                                                                 \
    while (i__ > 0) {                                                        \
        size_t parent__ = (i__ - 1) / 2;                                     \
        if (!macro_less(style, type, cmp, (arr) + parent__, (arr) + i__))    \
            break;                                                           \
        macro_swap((arr) + i__, (arr) + parent__);                           \
        i__ = parent__;                                                      \
    }                                                                        \

#define __macro_min_heap_pop_code(style, type, cmp, arr, num)           \
    type *iter, *smallest, *left, *right;                               \
    type tmp;                                                           \
    (num)--;                                                            \
    macro_swap((arr), (arr) + (num));                                   \
    macro_min_heapify(style, type, cmp, (arr), (arr) + (num), (arr),    \
                      iter, smallest, left, right)                      \

#define __macro_max_heap_pop_code(style, type, cmp, arr, num)           \
    type *iter, *largest, *left, *right;                                \
    type tmp;                                                           \
    (num)--;                                                            \
    macro_swap((arr), (arr) + (num));                                   \
    macro_max_heapify(style, type, cmp, (arr), (arr) + (num), (arr),    \
                      iter, largest, left, right)                       \

/* Early-exit guards for replace: return if root already dominates both children.
   These now check BOTH children and allow equality (stable w.r.t. duplicates). */
#define macro_check_min_heap_zero(style, type, cmp, arr, num)    \
    if ((num) <= 1) return;                                      \
    if (!macro_less(style, type, cmp, (arr) + 1, (arr)) &&       \
        ((num) <= 2 || !macro_less(style, type, cmp, (arr) + 2, (arr)))) return

#define macro_check_max_heap_zero(style, type, cmp, arr, num)    \
    if ((num) <= 1) return;                                      \
    if (!macro_less(style, type, cmp, (arr), (arr) + 1) &&       \
        ((num) <= 2 || !macro_less(style, type, cmp, (arr), (arr) + 2))) return

#endif /* _macro_heap_sort_H */
