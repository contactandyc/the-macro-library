/*
Copyright 2023 Andy Curtis

Public domain
*/

#ifndef _macro_test_H
#define _macro_test_H

#include <sys/time.h>

#include <stdio.h>
#include <string.h>

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/src/macro_swap.h"

#define __macro_test_start_sort_vars(type, rep, n)    \
    struct timeval start, end;                        \
    long long sv, ev;                                 \

#define macro_test_fill_reps(type, rep, n)           \
    for( int i=1; i<rep; i++ )  {                    \
        memcpy(arr+(i*n), arr, sizeof(type) * n);    \
    }

#define macro_test_sort_ascending(type, set_value, rep, n, gap, p2, p3)    \
    __macro_test_start_sort_vars(type, rep, n)                             \
    {                                                                      \
        type *p = arr;                                                     \
        type *ep = arr+n;                                                  \
        int i=0;                                                           \
        while(p < ep) {                                                    \
            set_value(p, i);                                               \
            i += gap;                                                      \
            p++;                                                           \
        }                                                                  \
        macro_test_fill_reps(type, rep, n)                                 \
    }

#define macro_test_sort_saw(type, set_value, rep, n,                    \
                            period, max_value, increment_per_period)    \
    __macro_test_start_sort_vars(type, rep, n)                          \
    {                                                                   \
        type *p = arr;                                                  \
        type *ep = arr+n;                                               \
        int i=0, value;                                                 \
        while(p < ep) {                                                 \
            value = ((i % period) * (max_value / period)) +             \
                ((i / period) * increment_per_period);                  \
            set_value(p, value);                                        \
            i++;                                                        \
            p++;                                                        \
        }                                                               \
        macro_test_fill_reps(type, rep, n)                              \
    }


#define macro_test_sort_descending(type, set_value, rep, n, gap, p2, p3)    \
    __macro_test_start_sort_vars(type, rep, n)                              \
    {                                                                       \
        type *p = arr;                                                      \
        type *ep = arr+n;                                                   \
        int i=n*gap;                                                        \
        while(p < ep) {                                                     \
            set_value(p, i);                                                \
            i -= gap;                                                       \
            p++;                                                            \
        }                                                                   \
        macro_test_fill_reps(type, rep, n)                                  \
    }


#define macro_test_sort_ascending_random(type, set_value, rep, n, gap, pct, p3)    \
    __macro_test_start_sort_vars(type, rep, n)                                     \
    {                                                                              \
        for( int r=0; r<rep; r++ ) {                                               \
            type tmp, *p = arr + (r*n), *ep = arr + ((r+1)*n);                     \
            int i=0;                                                               \
            while(p < ep) {                                                        \
                set_value(p, i);                                                   \
                i += gap;                                                          \
                p++;                                                               \
            }                                                                      \
            p = arr + (r*n);                                                       \
            int swaps = n * pct / 100;                                             \
            for( i=0; i<swaps; i++ ) {                                             \
                int a=rand() % n;                                                  \
                int b=rand() % n;                                                  \
                macro_swap(p+a, p+b);                                              \
            }                                                                      \
        }                                                                          \
    }

#define macro_test_sort_descending_random(type, set_value, rep, n, gap, pct, p3)    \
    __macro_test_start_sort_vars(type, rep, n)                                      \
    {                                                                               \
        for( int r=0; r<rep; r++ ) {                                                \
            type tmp, *p = arr + (r*n), *ep = arr + ((r+1)*n);                      \
            int i=n*gap;                                                            \
            while(p < ep) {                                                         \
                set_value(p, i);                                                    \
                i -= gap;                                                           \
                p++;                                                                \
            }                                                                       \
            p = arr + (r*n);                                                       \
            int swaps = n * pct / 100;                                              \
            for( i=0; i<swaps; i++ ) {                                              \
                int a=rand() % n;                                                   \
                int b=rand() % n;                                                   \
                macro_swap(p+a, p+b);                                               \
            }                                                                       \
        }                                                                           \
    }


#define macro_test_sort_random(type, set_value, rep, n, seed, p2, p3)    \
    if(seed != 0) srand(seed);                                           \
    __macro_test_start_sort_vars(type, rep, n)                           \
    {                                                                    \
        type *p = arr;                                                   \
        type *ep = arr+(n*rep);                                          \
        while(p < ep) {                                                  \
            set_value(p, rand());                                        \
            p++;                                                         \
        }                                                                \
    }

#define macro_test_sort_rand_max(type, set_value, rep, n,seed, max, p3)    \
    if(seed != 0) srand(seed);                                             \
    __macro_test_start_sort_vars(type, rep, n)                             \
    {                                                                      \
        type *p = arr;                                                     \
        type *ep = arr+(n*rep);                                            \
        while(p < ep) {                                                    \
            set_value(p, rand() % max);                                    \
            p++;                                                           \
        }                                                                  \
    }

#define macro_test_sort_rand_tail(type, set_value, rep, n, seed, pct, gap)    \
    if(seed != 0) srand(seed);                                                \
    __macro_test_start_sort_vars(type, rep, n)                                \
    {                                                                         \
        for( int r=0; r<rep; r++ ) {                                          \
            type *wp, *p = arr + (r*n), *ep = arr + ((r+1)*n);                \
            wp = p + ((100-(pct))*n/100);                                     \
            int i=0;                                                          \
            while(p < wp) {                                                   \
                set_value(p, i);                                              \
                i += gap;                                                     \
                p++;                                                          \
            }                                                                 \
            while(p < ep) {                                                   \
                set_value(p, rand());                                         \
                p++;                                                          \
            }                                                                 \
        }                                                                     \
    }


#define macro_test_sort_rand_head(type, set_value, rep, n, seed, pct, gap)    \
    if(seed != 0) srand(seed);                                                \
    __macro_test_start_sort_vars(type, rep, n)                                \
    {                                                                         \
        for( int r=0; r<rep; r++ ) {                                          \
            type *wp, *p = arr + (r*n), *ep = arr + ((r+1)*n);                \
            wp = p + ((pct)*n/100);                                           \
            int i=0;                                                          \
            while(p < wp) {                                                   \
                set_value(p, rand());                                         \
                p++;                                                          \
            }                                                                 \
            while(p < ep) {                                                   \
                set_value(p, i);                                              \
                i += gap;                                                     \
                p++;                                                          \
            }                                                                 \
        }                                                                     \
    }

#define __macro_test_sort_loop(name, sort, rep, n, style, type, cmp, report_name, time_spent)    \
    {                                                                                            \
        memcpy(new_arr, arr, sizeof(type) * n * rep);                                            \
        gettimeofday(&start, NULL);                                                              \
        for( int r=0; r<rep; r++ ) {                                                             \
            type *p = new_arr + (r*n);                                                           \
            sort(p, n);                                                                          \
        }                                                                                        \
        gettimeofday(&end, NULL);                                                                \
        int r=0;                                                                                 \
        type *wp = NULL;                                                                         \
        for( r=0; r<rep && !wp; r++ ) {                                                          \
            type *p = new_arr + (r*n), *ep = new_arr + ((r+1)*n);                                \
            p++;                                                                                 \
            while(p < ep) {                                                                      \
                if(macro_less(style, type, cmp, p, p-1)) {                                       \
                    wp = p-1;                                                                    \
                    break;                                                                       \
                }                                                                                \
                p++;                                                                             \
            }                                                                                    \
        }                                                                                        \
        sv = start.tv_sec * 1000000LL + start.tv_usec;                                           \
        ev = end.tv_sec * 1000000LL + end.tv_usec;                                               \
        ev = (ev-sv);                                                                            \
        time_spent = ev / 1000.0;                                                                \
        if(wp != NULL) {                                                                         \
            type *p = new_arr + (r*n), *ep = new_arr + ((r+1)*n);                                \
            printf( "%s: %s out of order at %ld of %lu\n",                                       \
                    name, report_name, wp-p, ep-p );                                             \
        }                                                                                        \
    }

#define macro_test_sort(sort_type, p1, p2, p3, rep,                                          \
                        test_name,  fmt,                                                     \
                        name1, name2, name3, sort1, sort2, sort3,                            \
                        style, type, cmp,                                                    \
                        set_value, n, result)                                                \
    {                                                                                        \
        double speed1 = 0.0, speed2 = 0.0, speed3 = 0.0;                                     \
        macro_test_sort_ ## sort_type(type, set_value, rep, n, p1, p2, p3)                   \
        __macro_test_sort_loop(name1, sort1, rep, n, style, type, cmp, test_name, speed1)    \
        __macro_test_sort_loop(name2, sort2, rep, n, style, type, cmp, test_name, speed2)    \
        __macro_test_sort_loop(name3, sort3, rep, n, style, type, cmp, test_name, speed3)    \
        (result).test_name = (char *)test_name;                                              \
        (result).times[0] = speed1;                                                          \
        (result).times[1] = speed2;                                                          \
        (result).times[2] = speed3;                                                          \
        if(test_name[0] != '*') printf( fmt, test_name, speed1, speed2, speed3);             \
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

static const char *__macro_default_sorts2[] = {
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



static const char *__macro_default_sorts[] = {
    "ascending",
    "equal",
    "descending",
    "random",
    "mixed",
    "rand_max",
    "rand_head",
    "rand_tail"
};

/* from chatgpt */
static inline void format_with_commas(int n, char *out) {
    int i, j = 0, len;
    char buffer[20];
    snprintf(buffer, 20, "%d", n);
    len = strlen(buffer);
    for(i = len - 1; i >= 0; i--) {
        out[j++] = buffer[i];
        if((len - i) % 3 == 0 && i != 0) {
            out[j++] = ',';
        }
    }
    out[j] = '\0';
    char *p = out;
    char *ep = out+j-1;
    while(p < ep) {
        char tmp = *p;
        *p = *ep;
        *ep = tmp;
        p++;
        ep--;
    }
}

static inline char * print_header_and_get_format( char **sorts, size_t num_sorts,
                                                  int size, int rep, int elem_size,
                                                  const char *sort1,
                                                  const char *sort2,
                                                  const char *sort3 ) {
    char size_s[20], rep_s[20];
    format_with_commas(size, size_s);
    format_with_commas(rep, rep_s);
    if(rep > 1)
        printf( "\nSorting %s items %s times, each item is %d bytes\n\n", size_s, rep_s, elem_size );
    else
        printf( "\nSorting %s items, each item is %d bytes\n\n", size_s, elem_size );
    size_t max_len = 10;
    for( size_t i=0; i<num_sorts; i++ ) {
        if(sorts[i][0] != '-' && sorts[i][0] != '*' && strlen(sorts[i]) > max_len)
            max_len = strlen(sorts[i]);
    }
    char *fmt = (char *)malloc(100);
    size_t sort1_len = strlen(sort1)+2, sort2_len = strlen(sort2)+2, sort3_len = strlen(sort3)+2;
    if(sort1_len < 10)
        sort1_len = 10;
    if(sort2_len < 10)
        sort2_len = 10;
    if(sort3_len < 10)
        sort3_len = 10;
    snprintf(fmt, 100, "%%%lus\t%%%lus\t%%%lus\t%%%lus\n",
             max_len, sort1_len, sort2_len, sort3_len );
    printf( fmt, "test name", sort1, sort2, sort3 );
    snprintf(fmt, 100, "%%%lus\t%%%lu.3f\t%%%lu.3f\t%%%lu.3f\n", max_len, sort1_len, sort2_len, sort3_len );
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

typedef struct {
    char *test_name;
    double times[3];
} __macro_test_sort_result_t;

static inline void __macro_sort_write_plot_csv( size_t n, size_t elem_size, int rep,
                                                const char *sort1, const char *sort2, const char *sort3,
                                                __macro_test_sort_result_t *results, int num_results) {

    char size_s[20], rep_s[20];
    format_with_commas(n, size_s);
    format_with_commas(rep, rep_s);

    FILE *out = fopen("plot.dat", "wb");
    if(rep > 1)
        fprintf( out, "\"Sorting %s items %s times, each item is %lu bytes\"\n", size_s, rep_s, elem_size );
    else
        fprintf( out, "\"Sorting %s items, each item is %lu bytes\"\n", size_s, elem_size );
    fprintf( out, "test_names" );
    for( int i=0; i<num_results; i++ )
        fprintf(out, ",\"%s\"", results[i].test_name );
    fprintf( out, "\n" );
    fprintf( out, "%s", sort1 );
    for( int i=0; i<num_results; i++ )
        fprintf(out, ",%0.3f", results[i].times[0] );
    fprintf( out, "\n" );
    fprintf( out, "%s", sort2 );
    for( int i=0; i<num_results; i++ )
        fprintf(out, ",%0.3f", results[i].times[1] );
    fprintf( out, "\n" );
    fprintf( out, "%s", sort3 );
    for( int i=0; i<num_results; i++ )
        fprintf(out, ",%0.3f", results[i].times[2] );
    fprintf( out, "\n" );
    fclose(out);
}

#define macro_test_alloc_c(type, rep, size) \
    type *arr = (type*)malloc(sizeof(type) * size * rep);                                      \
    type *new_arr = (type*)malloc(sizeof(type) * size * rep);                                  \

#define macro_test_alloc_cc(type, rep, size) \
    type *arr = new type[size * rep];                                      \
    type *new_arr = new type[size * rep];                                  \

#define macro_test_free_c() \
    free(arr); \
    free(new_arr);                                  \

#define macro_test_free_cc() \
    delete[] arr;                                      \
    delete[] new_arr;                                  \


#define macro_test_sort_driver(size, rep, set_value,                                               \
                               sort1_name, sort1, sort2_name, sort2, sort3_name, sort3,            \
                               style, type, cmp, sorts, num_sorts, language)                       \
    {                                                                                              \
        char **tests = sorts;                                                                      \
        int num_tests = num_sorts;                                                                 \
        if(num_tests <= 0) {                                                                       \
            tests = (char **)__macro_default_sorts;                                                \
            num_tests = sizeof(__macro_default_sorts) / sizeof(__macro_default_sorts[0]);          \
        }                                                                                          \
        int elem_size = sizeof(type);                                                              \
        char *fmt = print_header_and_get_format(tests, num_tests, size,                            \
            rep, elem_size, sort1_name, sort2_name, sort3_name );                                  \
        __macro_test_sort_result_t *results =                                                      \
            (__macro_test_sort_result_t*)malloc(sizeof(__macro_test_sort_result_t)*num_tests);     \
        int num_results = 0;                                                                       \
        macro_test_alloc_ ## language(type, rep, size) \
        for( int t=-2; t<num_tests; t++ ) {                                                        \
            const char *test_name;                                                                 \
            if(t < 0) test_name = "*descending";                                                   \
            else test_name = tests[t];                                                             \
            if(test_name[0] == '-') { printf( "%s\n", test_name+1); continue; }                    \
            const char *match_name = test_name;                                                    \
            if(match_name[0] == '*') match_name++;                                                 \
            if(!strncmp(match_name, "mixed", sizeof("mixed")-1) ||   \
               !strncmp(match_name, "ascending_random", sizeof("ascending_random")-1)) {      \
                int seed = 12345;                                                                  \
                int pct = 25;                                                                      \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                get_param_from_test_name(test_name, "pct", &pct);                                  \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(ascending_random, seed, pct, gap, rep,                             \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "descending_random", sizeof("descending_random")-1)) {    \
                int seed = 12345;                                                                  \
                int pct = 25;                                                                      \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                get_param_from_test_name(test_name, "pct", &pct);                                  \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(descending_random, seed, pct, gap, rep,                            \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "ascending", sizeof("ascending")-1)) {                         \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(ascending, gap, 0, 0, rep,                                         \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "equal", sizeof("equal")-1)) {                            \
                int gap = 0;                                                                       \
                macro_test_sort(ascending, gap, 0, 0, rep,                                         \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "descending", sizeof("descending")-1)) {                  \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(descending, gap, 0, 0, rep,                                        \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "rand_max", sizeof("rand_max")-1)) {                      \
                int seed = 12345;                                                                  \
                int rmax = 100;                                                                    \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                get_param_from_test_name(test_name, "max", &rmax);                                 \
                macro_test_sort(rand_max, seed, rmax, 0, rep,                                      \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "random", sizeof("random")-1)) {                          \
                int seed = 12345;                                                                  \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                macro_test_sort(random, seed, 0, 0, rep,                                           \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "rand_head", sizeof("rand_head")-1)) {                    \
                int seed = 12345;                                                                  \
                int pct = 25;                                                                      \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                get_param_from_test_name(test_name, "pct", &pct);                                  \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(rand_head, seed, pct, gap, rep,                                    \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "rand_tail", sizeof("rand_tail")-1)) {                    \
                int seed = 12345;                                                                  \
                int pct = 25;                                                                      \
                int gap = 5;                                                                       \
                get_param_from_test_name(test_name, "seed", &seed);                                \
                get_param_from_test_name(test_name, "pct", &pct);                                  \
                get_param_from_test_name(test_name, "gap", &gap);                                  \
                macro_test_sort(rand_tail, seed, pct, gap, rep,                                    \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            else if(!strncmp(match_name, "saw", sizeof("saw")-1)) {                                \
                int period = 100;                                                                  \
                int max_value = 500;                                                               \
                int increment_per_period = 0;                                                      \
                get_param_from_test_name(test_name, "period", &period);                            \
                get_param_from_test_name(test_name, "max", &max_value);                            \
                get_param_from_test_name(test_name, "increment", &increment_per_period);           \
                macro_test_sort(saw, period, max_value, increment_per_period, rep,                 \
                                test_name, fmt,                                                    \
                                sort1_name, sort2_name, sort3_name, sort1, sort2, sort3,           \
                                style, type, cmp,                                                  \
                                set_value, size, results[num_results]);                            \
            }                                                                                      \
            if(test_name[0] != '*') num_results++;                                                 \
        }                                                                                          \
        macro_test_free_ ## language() \
        free(fmt);                                                                                 \
        __macro_sort_write_plot_csv( size, elem_size, rep, sort1_name, sort2_name, sort3_name,     \
                                     results, num_results );                                       \
        free(results);                                                                             \
    }

#endif /* _macro_test_H */