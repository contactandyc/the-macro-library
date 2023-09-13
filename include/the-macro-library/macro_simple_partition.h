/*
Copyright 2023 Andy Curtis

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