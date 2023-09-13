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

#ifndef _macro_isort_H
#define _macro_isort_H

#include "macro_cmp.h"

#define macro_isort(style, type, cmp, base, n,                   \
                    ep, curp, p, tmp)                            \
    ep = base + n;                                               \
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
