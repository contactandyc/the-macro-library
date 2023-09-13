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

#ifndef _macro_test_H
#define _macro_test_H

#include <sys/time.h>

#include <stdio.h>
#include <string.h>

#include "macro_cmp.h"

#define __macro_test_start_sort_vars(type, rep, arr, new_arr, n)    \
    type *wp, *sp = arr, *sp2 = new_arr, *p = arr;                  \
    type *ep = arr+n, *ep2 = sp2+n;                                 \
    struct timeval start, end;                                      \
    long long sv, ev;                                               \
    int repeat=(rep);

#define __macro_test_start_sort_loop(sort, n)    \
    gettimeofday(&start, NULL);                  \
    for( int r=0; r<repeat; r++ ) {              \
        p = sp;                                  \
        wp = sp2;                                \
        while(p < ep) {                          \
            *wp++ = *p++;                        \
        }                                        \
        sort(arr2, n);                           \
    }

#define macro_test_sort_ascending(type, set_value, rep, arr, new_arr, n, gap, p2, p3, sort)    \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                   \
    int i=0;                                                                                   \
    while(p < ep) {                                                                            \
        set_value(p, i);                                                                       \
        i += gap;                                                                              \
        p++;                                                                                   \
    }                                                                                          \
    __macro_test_start_sort_loop(sort, n)

#define macro_test_sort_saw(type, set_value, rep, arr, new_arr, n,            \
                            period, max_value, increment_per_period, sort)    \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                  \
    int i=0, value;                                                           \
    while(p < ep) {                                                           \
        value = ((i % period) * (max_value / period)) +                       \
            ((i / period) * increment_per_period);                            \
        set_value(p, value);                                                  \
        i++;                                                                  \
        p++;                                                                  \
    }                                                                         \
    __macro_test_start_sort_loop(sort, n)


#define macro_test_sort_descending(type, set_value, rep, arr, new_arr, n, gap, p2, p3, sort)    \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                    \
    int i=n*gap;                                                                                \
    while(p < ep) {                                                                             \
        set_value(p, i);                                                                        \
        i -= gap;                                                                               \
        p++;                                                                                    \
    }                                                                                           \
    __macro_test_start_sort_loop(sort, n)


#define macro_test_sort_random(type, set_value, rep, arr, new_arr, n, seed, p2, p3, sort)    \
    if(seed != 0) srand(seed);                                                               \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                 \
    while(p < ep) {                                                                          \
        set_value(p, rand());                                                                \
        p++;                                                                                 \
    }                                                                                        \
    __macro_test_start_sort_loop(sort, n)


#define macro_test_sort_rand_tail(type, set_value, rep, arr, new_arr, n, seed, pct, gap, sort)    \
    if(seed != 0) srand(seed);                                                                    \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                      \
    wp = p + ((100-(pct))*n/100);                                                                 \
    int i=0;                                                                                      \
    while(p < wp) {                                                                               \
        set_value(p, i);                                                                          \
        i += gap;                                                                                 \
        p++;                                                                                      \
    }                                                                                             \
    while(p < ep) {                                                                               \
        set_value(p, rand());                                                                     \
        p++;                                                                                      \
    }                                                                                             \
    __macro_test_start_sort_loop(sort, n)

#define macro_test_sort_rand_head(type, set_value, rep, arr, new_arr, n, seed, pct, gap, sort)    \
    if(seed != 0) srand(seed);                                                                    \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                      \
    wp = p + ((pct)*n/100);                                                                       \
    int i=0;                                                                                      \
    while(p < wp) {                                                                               \
        set_value(p, rand());                                                                     \
        p++;                                                                                      \
    }                                                                                             \
    while(p < ep) {                                                                               \
        set_value(p, i);                                                                          \
        i += gap;                                                                                 \
        p++;                                                                                      \
    }                                                                                             \
    __macro_test_start_sort_loop(sort, n)


#define macro_test_sort_rand_max(type, set_value, rep, arr, new_arr, n,seed, max, p3, sort)    \
    if(seed != 0) srand(seed);                                                                 \
    __macro_test_start_sort_vars(type, rep, arr, new_arr, n)                                   \
    while(p < ep) {                                                                            \
        set_value(p, rand() % max);                                                            \
        p++;                                                                                   \
    }                                                                                          \
    __macro_test_start_sort_loop(sort, n)

#define macro_test_sort_end(name, style, type, cmp, report_name, time_spent)    \
    gettimeofday(&end, NULL);                                                   \
                                                                                \
    wp = NULL;                                                                  \
    p = sp2+1;                                                                  \
    while(p < ep2) {                                                            \
        if(macro_less(style, type, cmp, p, p-1)) {                              \
            wp = p-1;                                                           \
            break;                                                              \
        }                                                                       \
        p++;                                                                    \
    }                                                                           \
                                                                                \
    sv = start.tv_sec * 1000000LL + start.tv_usec;                              \
    ev = end.tv_sec * 1000000LL + end.tv_usec;                                  \
    ev = (ev-sv);                                                               \
    time_spent = ev / 1000000.0;                                                \
    if(wp != NULL)                                                              \
        printf( "%s: %s out of order at %ld of %lu\n",                          \
                name, report_name, wp-sp2, ep2-sp2 );

#define macro_test_sort(sort_type, p1, p2, p3, rep,                                                    \
                        test_name,  fmt,                                                               \
                        name1, name2, sort1, sort2,                                                    \
                        style, type, cmp,                                                              \
                        set_value, arr, new_arr, n)                                                    \
    {                                                                                                  \
        double speed1 = 0.0, speed2 = 0.0;                                                             \
        {                                                                                              \
            macro_test_sort_ ## sort_type(type, set_value, rep, arr, new_arr, n, p1, p2, p3, sort1)    \
            macro_test_sort_end(name1, style, type, cmp, test_name, speed1)                            \
        }                                                                                              \
        {                                                                                              \
            macro_test_sort_ ## sort_type(type, set_value, rep, arr, new_arr, n, p1, p2, p3, sort2)    \
            macro_test_sort_end(name2, style, type, cmp, test_name, speed2)                            \
        }                                                                                              \
        double percentageGain = speed1 > 0.0 ? ((speed2 - speed1) / speed1) * 100 : 0.0;               \
        if(test_name[0] != '*') printf( fmt, test_name, speed1, speed2, percentageGain);               \
    }

#define __macro_get_int_param(argc, argv, arg, v)    \
    if(argc > arg)                                   \
        sscanf(argv[arg], "%d", v);                  \

#define macro_test_sort_driver_usage()                       \
    "<test_name> [size=1024*1024] [rep=10] [test_args]\n"    \
    "\n"                                                     \
    "ascending: [gap=5]\n"                                   \
    "descending: [gap=5]\n"                                  \
    "random: [seed=1234]\n"                                  \
    "rand_max: [seed=1234] [max_value=100]\n"                \
    "rand_head: [seed=1234] [pct=25] [gap=5]\n"              \
    "rand_tail: [seed=1234] [pct=25] [gap=5]\n"              \
    "saw: [period=100] [max_value=500] [increment_per_period=0]\n"

static const char *__macro_default_sorts[] = {
    "-Ascending Values",
    "ascending",
    "-Equal Values",
    "ascending[gap=0]",
    "-Descending Values",
    "descending",
    "-Saw Pattern",
    "saw[period=50,max=1000,increment=10]",
    "saw[period=50,max=-1000,increment=20]",
    "-\nRandom Values",
    "random[seed=1]",
    "random[seed=0]",
    "random[seed=0]",
    "-Random between 0-99",
    "rand_max[seed=1,max=100]",
    "rand_max[seed=0,max=100]",
    "rand_max[seed=0,max=100]",
    "-Random between 0-9999",
    "rand_max[seed=1,max=10000]",
    "rand_max[seed=0,max=10000]",
    "rand_max[seed=0,max=10000]",
    "-Random for first 25%",
    "rand_head[seed=1,pct=25]",
    "rand_head[seed=0,pct=25]",
    "rand_head[seed=0,pct=25]",
    "-Random for last 25%",
    "rand_tail[seed=1,pct=25]",
    "rand_tail[seed=0,pct=25]",
    "rand_tail[seed=0,pct=25]",
};


static inline char * print_header_and_get_format( char **sorts, size_t num_sorts,
                                                  int size, int rep, int elem_size,
                                                  const char *sort1, const char *sort2 ) {
    printf( "\nSorting %d elements %d times which are of size %d\n\n", size, rep, elem_size );
    size_t max_len = 10;
    for( size_t i=0; i<num_sorts; i++ ) {
        if(sorts[i][0] != '-' && sorts[i][0] != '*' && strlen(sorts[i]) > max_len)
            max_len = strlen(sorts[i]);
    }
    char *fmt = (char *)malloc(100);
    size_t sort1_len = strlen(sort1)+2, sort2_len = strlen(sort2)+2;
    if(sort1_len < 10)
        sort1_len = 10;
    if(sort2_len < 10)
        sort2_len = 10;
    snprintf(fmt, 100, "%%%lus\t%%%lus\t%%%lus\t%%15s\n", max_len, sort1_len, sort2_len);
    printf( fmt, "test name", sort1, sort2, "gain %");
    snprintf(fmt, 100, "%%%lus\t%%%lu.6f\t%%%lu.6f\t%%15.2f\n", max_len, sort1_len, sort2_len );
    return fmt;
}

static inline void get_param_from_test_name(const char *input, const char *key, int *res) {
    char *start_bracket = (char *)strchr(input, '[');
    if (start_bracket == NULL) {
        return;
    }

    char *start = strstr(start_bracket, key);
    while (start != NULL) {
        char preceding = *(start - 1);
        if (preceding == ',' || preceding == '[') {
            if (*(start + strlen(key)) == '=') {
                // Found a valid key
                break;
            }
        }
        start = strstr(start + 1, key);
    }

    if (start == NULL) {
        return;
    }

    start += strlen(key);
    start++;

    int value = 0;
    bool negative = false;
    if(*start == '-') {
        negative = true;
        start++;
    }
    while(*start >= '0' && *start <= '9') {
        int digit = *start - '0';
        value = (value*10) + digit;
        start++;
    }
    if(negative)
        value = -value;

    *res = value;
}

#define macro_test_sort_driver(arr, arr2, size, rep, set_value,                              \
                               sort1_name, sort1, sort2_name, sort2,                         \
                               style, type, cmp, sorts, num_sorts)                           \
    {                                                                                        \
        char **tests = sorts;                                                                \
        int num_tests = num_sorts;                                                           \
        if(num_tests <= 0) {                                                                 \
            tests = (char **)__macro_default_sorts;                                          \
            num_tests = sizeof(__macro_default_sorts) / sizeof(__macro_default_sorts[0]);    \
        }                                                                                    \
        int elem_size = sizeof(type);                                                        \
        char *fmt = print_header_and_get_format(tests, num_tests, size,                      \
            rep, elem_size, sort1_name, sort2_name );                                        \
        for( int t=-2; t<num_tests; t++ ) {                                                  \
            const char *test_name;                                                           \
            if(t < 0) test_name = "*descending";                                             \
            else test_name = tests[t];                                                       \
            if(test_name[0] == '-') { printf( "%s\n", test_name+1); continue; }              \
            const char *match_name = test_name;                                              \
            if(match_name[0] == '*') match_name++;                                           \
            if(!strncmp(match_name, "ascending", sizeof("ascending")-1)) {                   \
                int gap = 5;                                                                 \
                get_param_from_test_name(test_name, "gap", &gap);                            \
                macro_test_sort(ascending, gap, 0, 0, rep,                                   \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "descending", sizeof("descending")-1)) {            \
                int gap = 5;                                                                 \
                get_param_from_test_name(test_name, "gap", &gap);                            \
                macro_test_sort(descending, gap, 0, 0, rep,                                  \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "rand_max", sizeof("rand_max")-1)) {                \
                int seed = 12345;                                                            \
                int rmax = 100;                                                              \
                get_param_from_test_name(test_name, "seed", &seed);                          \
                get_param_from_test_name(test_name, "max", &rmax);                           \
                macro_test_sort(rand_max, seed, rmax, 0, rep,                                \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "random", sizeof("random")-1)) {                    \
                int seed = 12345;                                                            \
                get_param_from_test_name(test_name, "seed", &seed);                          \
                macro_test_sort(random, seed, 0, 0, rep,                                     \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "rand_head", sizeof("rand_head")-1)) {              \
                int seed = 12345;                                                            \
                int pct = 25;                                                                \
                int gap = 5;                                                                 \
                get_param_from_test_name(test_name, "seed", &seed);                          \
                get_param_from_test_name(test_name, "pct", &pct);                            \
                get_param_from_test_name(test_name, "gap", &gap);                            \
                macro_test_sort(rand_head, seed, pct, gap, rep,                              \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "rand_tail", sizeof("rand_tail")-1)) {              \
                int seed = 12345;                                                            \
                int pct = 25;                                                                \
                int gap = 5;                                                                 \
                get_param_from_test_name(test_name, "seed", &seed);                          \
                get_param_from_test_name(test_name, "pct", &pct);                            \
                get_param_from_test_name(test_name, "gap", &gap);                            \
                macro_test_sort(rand_tail, seed, pct, gap, rep,                              \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
            else if(!strncmp(match_name, "saw", sizeof("saw")-1)) {                          \
                int period = 100;                                                            \
                int max_value = 500;                                                         \
                int increment_per_period = 0;                                                \
                get_param_from_test_name(test_name, "period", &period);                      \
                get_param_from_test_name(test_name, "max", &max_value);                      \
                get_param_from_test_name(test_name, "increment", &increment_per_period);     \
                macro_test_sort(saw, period, max_value, increment_per_period, rep,           \
                                test_name, fmt,                                              \
                                sort1_name, sort2_name, sort1, sort2,                        \
                                style, type, cmp,                                            \
                                set_value, arr, arr2, size);                                 \
            }                                                                                \
        }                                                                                    \
        free(fmt);                                                                           \
    }

#endif /* _macro_test_H */