// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#ifndef _macro_simple_partition_H
#define _macro_simple_partition_H

/*
    Similar to dutch_flag_partition, except the items equal to the midpoint
    are not grouped together.  Not useful in practice
*/

#define macro_simple_partition(label_prefix, style, type, cmp,            \
                                   lo, mid, hi,                           \
                                   left_eq, left_p, right_p, right_eq,    \
                                   left_n, right_n, tmp_n)                \
    macro_swap(lo, mid);                                                  \
    left_p = lo+1;                                                        \
    right_p = hi;                                                         \
label_prefix ## _cmp_first:;                                              \
    if(left_p <= right_p) {                                               \
        if(macro_less(style, type, cmp, lo, left_p)) {                    \
            goto label_prefix ## _cmp_second2;                            \
        }                                                                 \
        left_p++;                                                         \
        goto label_prefix ## _cmp_first;                                  \
    }                                                                     \
    else                                                                  \
        goto label_prefix ## _after_swapping;                             \
label_prefix ## _cmp_second:;                                             \
    if(left_p > right_p)                                                  \
        goto label_prefix ## _after_swapping;                             \
label_prefix ## _cmp_second2:;                                            \
    if(macro_less(style, type, cmp, lo, right_p)) {                       \
        right_p--;                                                        \
        goto label_prefix ## _cmp_second;                                 \
    }                                                                     \
label_prefix ## _swap_and_loop:;                                          \
    macro_swap(left_p, right_p);                                          \
    left_p++;                                                             \
    right_p--;                                                            \
    goto label_prefix ## _cmp_first;                                      \
label_prefix ## _after_swapping:;                                         \
	left_n = left_p-lo+1;                                                    \
	right_n = hi-right_p;                                                    \
    macro_swap(lo, left_p-1);                                             \
    left_p = lo;                                                          \
    right_p = hi+1-right_n;

#endif /* _macro_simple_partition_H */
