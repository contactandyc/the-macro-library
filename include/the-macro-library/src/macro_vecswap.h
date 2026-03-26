// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

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
