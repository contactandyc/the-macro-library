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

#ifndef _macro_check_sorted_H
#define _macro_check_sorted_H

#include "the-macro-library/macro_cmp.h"

/*
    The basic idea with this is to quickly exit if a set is not sorted to one 
    of the goto channels (find_mid or mid_found).  In order for the set to be
    sorted in ascending order, lo <= mid <= hi must be satisfied.  If this is
    the case, check a few more points depending upon the array size.  If 
    lo >= mid >= hi, then the same idea can be used to find if the set is 
    reversed.  If it is reversed, this will reverse the array in place.

    This should only be called if there are > 8 elements (16 is better).

    There are more pre-comparisons for larger sets as the larger sets are a more expensive hit if they happen
    to seem sorted.

    style, type, cmp are used for macro_less
    base - array of n elements
    r_lo, r_mid, r_hi are values that are set.  find_mid and mid_found are labels to
    goto.  If r_mid is properly found, this will go to the mid_found, otherwise,
    it will goto find_mid.  If the set is sorted, this macro will fall-through

    temporary variables that are needed are...
    delta, i should be of type size_t
    a, and b should be of type*
    
    note: the major if/else statement reflect the inverse of each other.  It is 
    possible to consolidate the code further, but I found it unreadable in my 
    attempt.
*/

#define macro_check_sorted(style, type, cmp,                                  \
                           base, n,                                           \
                           r_lo, r_mid, r_hi,                                 \
                           delta, xp, yp,                           \
                           find_mid, mid_found)                               \
    r_lo = base;                                                              \
    r_mid = r_lo + (n >> 1);                                                  \
    r_hi = r_lo+(n-1);                                                        \
    if(macro_less(style, type, cmp, r_hi, r_lo )) {                           \
        if(macro_less(style, type, cmp, r_lo, r_mid)) {                       \
            r_mid = r_lo;                                                     \
            goto find_mid;                                                    \
        }                                                                     \
        if(macro_less(style, type, cmp, r_mid, r_hi)) {                       \
            r_mid = r_hi;                                                     \
            goto find_mid;                                                    \
        }                                                                     \
        if(n < 32) { \
            delta = (n >> 2);                                                  \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += delta;                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
        } else if(n < 256) { \
            delta = (n >> 3);                                                  \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += delta;                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += delta;                                                  \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
        } else { \
            delta = (n >> 4);                                                  \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp, xp+delta)) goto find_mid;         \
            if(macro_less(style, type, cmp, yp-delta, yp)) goto find_mid;         \
        } \
        xp = r_hi;                                                            \
        while(xp > r_lo) {                                                    \
            if(macro_less(style, type, cmp, xp-1, xp)) goto mid_found;        \
            xp--;                                                             \
        }                                                                     \
        while(r_lo < r_hi) {                                                  \
            macro_swap(r_lo, r_hi);                                           \
            r_lo++;                                                           \
            r_hi--;                                                           \
        }                                                                     \
    } else {                                                                  \
        if(macro_less(style, type, cmp, r_hi, r_mid)) {                       \
            r_mid = r_hi;                                                     \
            goto find_mid;                                                    \
        }                                                                     \
        if(macro_less(style, type, cmp, r_mid, r_lo)) {                       \
            r_mid = r_lo;                                                     \
            goto find_mid;                                                    \
        }                                                                     \
        if(n < 32) { \
            delta = n >> 2;                                                 \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += delta;                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
        } else if(n < 256) { \
            delta = n >> 3;                                                 \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += delta;                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += delta;                                                  \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= delta; \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
        } else { \
            delta = n >> 4;                                                 \
            xp = r_lo;                                                            \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            yp = r_hi; \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            xp += (delta<<1);                                                  \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
            yp -= (delta<<1); \
            if(macro_less(style, type, cmp, xp+delta, xp)) goto find_mid;         \
            if(macro_less(style, type, cmp, yp, yp-delta)) goto find_mid;         \
        } \
        xp = r_lo;                                                            \
        while(xp < r_hi) {                                                    \
            if(macro_less(style, type, cmp, xp+1, xp)) goto mid_found;        \
            xp++;                                                             \
        }                                                                     \
    }

#endif /* _macro_check_sorted_H */