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

#ifndef _macro_isort_H
#define _macro_isort_H

#include "the-macro-library/macro_cmp.h"

/*
    This macro defines an insertion sort algorithm for an array of elements

    * style, type, cmp are used by the macro_less for comparing elements
    * base, n is the array to be sorted and the number of elements
    * ep, curp, p are used internally and are pointers to type
    * tmp is used internally and is an instance of type
*/

#define macro_isort_reverse_check(style, type, cmp, base, n,                   \
                                   ep, curp, p, tmp)    \
    if(macro_less(style, type, cmp, ep-1, base)) { \
        p = ep-1;   \
        curp = base + 1; \
        macro_swap(base, p); \
        p--; \
        while(curp < p && macro_less(style, type, cmp, p, curp)) { \
            macro_swap(curp, p); \
            p--; \
            curp++; \
        }   \
    }

#define macro_isort_reverse_check2(style, type, cmp, base, n,                   \
                                   ep, curp, p, tmp)    \
    if(macro_less(style, type, cmp, ep-1, base)) { \
        macro_swap(base, ep-1); \
    }

#define macro_isort_reverse_check3(style, type, cmp, base, n,                   \
                                   ep, curp, p, tmp)

#define macro_isort(style, type, cmp, base, n,                   \
                    ep, curp, p, tmp)                            \
    ep = base + n;                                               \
    macro_isort_reverse_check3(style, type, cmp, base, n, ep, curp, p, tmp)    \
    curp = base + 1;                                             \
    while (curp < ep) {                                          \
        tmp = *curp;                                             \
        p = curp;                                                \
        while (p > base &&                                       \
               (macro_less(style, type, cmp, &tmp, p - 1))) {    \
            *p = *(p - 1);                                       \
            --p;                                                 \
        }                                                        \
        *p = tmp;                                                \
        ++curp;                                                  \
    }

#endif /* _macro_isort_H */
