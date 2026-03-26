// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>
//
// Based on the Dutch national flag problem (https://en.wikipedia.org/wiki/Dutch_national_flag_problem)
// Consulted: Bentley & McIlroy, “Engineering a sort function,”
// Software – Practice and Experience 23(11): 1249–1265 (1993)

#ifndef _macro_dutch_flag_partition_H
#define _macro_dutch_flag_partition_H

#include "the-macro-library/src/macro_vecswap.h"

/* Towards the end in
    macro_vecswap(lo, left_p-tmp_n, left_eq, right_p, tmp_n);
      AND
    macro_vecswap(left_p, hi+1-tmp_n, left_eq, right_p, tmp_n);

   the left_eq, right_p are used as temp variables.  This is just to avoid
   unneeded variables in the function.  Even though right_p is used again,
   it is only set to hi+1-right_n at the end, so it is safe to use these
   two variables as temp variables in the vecswap call.
*/

#define macro_dutch_flag_partition(label_prefix, style, type, cmp,        \
                                   lo, mid, hi,                           \
                                   left_eq, left_p, right_p, right_eq,    \
                                   left_n, right_n, tmp_n)                \
    macro_swap(lo, mid);                                                  \
    left_eq = left_p = lo + 1;                                            \
    right_p = right_eq = hi;                                              \
label_prefix ## _cmp_first:;                                              \
    if (left_p <= right_p) {                                              \
        if (macro_less(style, type, cmp, left_p, lo)) {                   \
            left_p++;                                                     \
            goto label_prefix ## _cmp_first;                              \
        }                                                                 \
        if (macro_less(style, type, cmp, lo, left_p))                     \
            goto label_prefix ## _cmp_second2;                            \
        macro_swap(left_eq, left_p);                                      \
        left_eq++;                                                        \
        left_p++;                                                         \
        goto label_prefix ## _cmp_first;                                  \
    } else                                                                \
        goto label_prefix ## _after_swapping;                             \
label_prefix ## _cmp_second:;                                             \
    if (left_p > right_p)                                                 \
        goto label_prefix ## _after_swapping;                             \
label_prefix ## _cmp_second2:;                                            \
    if (macro_less(style, type, cmp, lo, right_p)) {                      \
        right_p--;                                                        \
        goto label_prefix ## _cmp_second;                                 \
    }                                                                     \
    if (macro_less(style, type, cmp, right_p, lo))                        \
        goto label_prefix ## _swap_and_loop;                              \
    macro_swap(right_p, right_eq);                                        \
    right_eq--;                                                           \
    right_p--;                                                            \
    goto label_prefix ## _cmp_second;                                     \
label_prefix ## _swap_and_loop:;                                          \
    macro_swap(left_p, right_p);                                          \
    left_p++;                                                             \
    right_p--;                                                            \
    goto label_prefix ## _cmp_first;                                      \
label_prefix ## _after_swapping:;                                         \
    left_n  = (size_t)(left_p  - left_eq);                                \
    right_n = (size_t)(right_eq - right_p);                               \
    tmp_n   = (size_t)(left_eq - lo);                                     \
    if (tmp_n > left_n)                                                   \
        tmp_n = left_n;                                                   \
    macro_vecswap(lo, left_p - tmp_n, left_eq, right_p, tmp_n);           \
    tmp_n = (size_t)(hi - right_eq);                                      \
    if (tmp_n > right_n)                                                  \
        tmp_n = right_n;                                                  \
    macro_vecswap(left_p, hi + 1 - tmp_n, left_eq, right_p, tmp_n);       \
    left_p = lo;                                                          \
    right_p = hi + 1 - right_n;

#endif /* _macro_dutch_flag_partition_H */
