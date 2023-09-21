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

#ifndef _macro_vecswap_H
#define _macro_vecswap_H

/*
    swaps x/tmp_n with y/tmp_n

    tmp_x, tmp_y are temporary variables which are the same type as x and y.
    tmp_num becomes zero after this macro, but is expected to be the number of
    elements to swap.
*/

#define macro_vecswap(x, y, tmp_x, tmp_y, tmp_n)    \
    tmp_x = x;                                      \
    tmp_y = y;                                      \
    while(tmp_n > 0) {                              \
        macro_swap(tmp_x, tmp_y);                   \
        tmp_x++;                                    \
        tmp_y++;                                    \
        tmp_n--;                                    \
    }

#endif /* _macro_vecswap_H */