# the-macro-library
The macro library attempts to match the performance of C++ in C.  macro_sort performs significantly better than C++ in some cases and in others is roughly comparable.  In particular, macro_sort significantly outperforms std::sort when the items are presorted or reversed.  There are also some cases where it is able to output std::sort across the board.

examples/speed_test.cc (C/C++ is provided with a compare function)



![Speed Comparison with Provided ]

The macro_sort performs significantly better than qsort and roughly the same as .  This test was run on a MacBook Pro 2020 2.3 GHz Quad-Core Intel Core i7 with 32 GB 3733 MHz LPDDR4X RAM.

![Speed Comparison](images/BarChart.png)

## What's included

`macro_sort.h` - a c approach to sorting

`macro_bsearch.h` - a c approach to searching using various binary search approaches

`macro_map.h` - a c version of the c++ map (or dictionary)

I welcome suggestions and plan to have more soon!

## Installation

### Clone the library and change to the directory

```bash
git clone https://github.com/contactandyc/the-macro-library.git
cd the-macro-library
```

### Setup python environment

Mac OS X
```bash
python3 -m venv venv
source venv/bin/activate
source .env
pip install -r requirements.txt
```

Linux
```bash
python -m venv venv
source venv/bin/activate
source .env
pip install -r requirements.txt
```

Windows
```bash
python -m venv venv
source `.\venv\Scripts\activate`
pip install -r requirements.txt
```

To leave the virtual environment
```bash
deactivate
```

### Make the macro library 

```bash
mkdir -p build
cd build
cmake ..
make
```

Optionally install the library
```bash
make install
```

## Lightweight

This library is entirely header based and requires no special linkage.

## Simple to use

```c
#include "the-macro-library/macro_sort.h"

bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}

macro_sort(sort_ints, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, 5);
    ...    
```

## Debuggable

C macros are difficult to debug with most debuggers.  The included `bin/convert-macros-to-code` will convert macros to their base code and preserve readability.  Additionally, the convert tool can be used to create functions which have no dependency on this library.

## Extendable

This library has started with introsort and a series of binary search methods.  Other algorithms are planned to be added soon.

## A Fast C Sort (works fine with C++)
A comparison of this sort to qsort and std::sort follows.

starting from root directory (assuming package was built - see last section)
```bash
cd build/examples/speed-test
./speed_test 1000000 10
macro-plot.py
```

```
Sorting 1,000,000 items 10 times, each item is 20 bytes

       test name          macro_sort      std::sort          qsort
       ascending              12.895         35.109         46.842
           equal              31.933         35.080         55.717
      descending              30.492         71.564        259.188
          random             801.425       1006.874       1329.232
ascending_random             685.448        876.419       1208.503
        rand_max             289.191        373.177        413.587
       rand_head             365.521        570.607        752.974
       rand_tail             331.612        488.445        688.835
```

```
% ./speed_test 1000000 100 

Sorting 1,000,000 items 100 times, each item is 20 bytes

       test name          macro_sort (with_compare)       std::sort (with_compare)           qsort
       ascending                            124.100                        345.294         452.234
           equal                            266.327                        394.359         550.887
      descending                            267.301                        718.980        2545.969
          random                           8163.480                       9829.273       12926.917
ascending_random                           6929.094                       8659.981       11573.819
        rand_max                           2786.566                       3742.983        4169.095
       rand_head                           3687.647                       5481.055        7297.058
       rand_tail                           3397.744                       4923.769        6599.711
```

```
% ./speed_test2 1000000 100

Sorting 1,000,000 items 100 times, each item is 20 bytes

       test name          macro_sort (with compare)       macro_sort (user compare)       std::sort (with compare)
       ascending                            125.303                         132.150                        350.112
           equal                            281.692                         283.339                        362.696
      descending                            270.878                         279.437                        756.251
          random                           8417.626                        8432.059                      10164.068
ascending_random                           7039.942                        7175.946                       8886.222
        rand_max                           2827.633                        2823.108                       3808.227
       rand_head                           3762.794                        3738.675                       5657.587
       rand_tail                           3374.678                        3412.348                       4921.894
```

```
% ./speed_test_class 1000000 100 

Sorting 1,000,000 items 100 times, each item is 20 bytes

       test name          macro_sort (with compare)       std::sort       std::sort (with compare)
       ascending                            125.574         276.332                        392.728
           equal                            267.515         277.585                        364.329
      descending                            270.696         556.046                        781.695
          random                           8312.908        8071.065                      10161.059
ascending_random                           6985.034        7093.619                       8936.456
        rand_max                           2799.808        3093.811                       3792.217
       rand_head                           3751.256        3964.302                       5711.602
       rand_tail                           3352.918        3425.056                       5045.069
```

```
% ./speed_test_class2 1000000 100

Sorting 1,000,000 items 100 times, each item is 20 bytes

       test name          macro_sort      macro_sort (with compare)       std::sort
       ascending             123.947                        125.024         277.621
           equal             262.961                        274.195         279.412
      descending             269.761                        279.707         537.356
          random            8141.267                       8217.977        7894.193
ascending_random            6911.564                       6948.011        6782.404
        rand_max            2760.134                       2745.247        2960.125
       rand_head            3727.606                       3729.694        3841.979
       rand_tail            3416.946                       3404.477        3390.682
```

```
% ./speed_test_class3 1000000 10 

Sorting 1,000,000 items 10 times, each item is 128 bytes

       test name          macro_sort      std::sort       std::sort (with compare)
       ascending              66.346        129.161                        124.068
           equal             136.901        126.345                        120.919
      descending             134.923        259.615                        250.875
          random            1205.031       1190.597                       1387.029
ascending_random            1126.265       1108.153                       1290.547
        rand_max             576.988        585.242                        653.625
       rand_head             844.193        854.493                        950.742
       rand_tail             814.720        796.030                        866.865
```

Across nearly all measures for the given test, the macro library performs better than std::sort and significantly better than qsort.  I've found that for raw types, the std::sort tends to perform about the same as the macro versions.

See examples/speed-test/speed_test.cc and feel free to modify the test. 

## The macro sort is the introsort + a sort check at the beginning.

The sort check at the beginning considers the first and last element of the array and then expects the middle element to be between them.  If the middle element is not between them, the introsort continues.  Otherwise, two conditions remain.  The first is that the first element is less than or equal to the last element.  In this case a few additional elements are compared to further check that the array is sorted.  If any of those checks fail, the introsort loop is continued.  Otherwise, the array is checked from the beginning to the end and if it is entirely sorted, the sort is finished.  If the first element is greater than the last element, then the same comparisions continue in reverse. Once the array is determined to be completely reversed, all elements are swapped.

## A simple example

examples/demo/sort_ints.c
```c
#include "the-macro-library/macro_sort.h"

bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}

macro_sort(sort_ints, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]));
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
```

```bash
$ gcc examples/demo/sort_ints.c -o sort_ints -O3
$ ./sort_ints
 1 2 3 4 5
```

## `_macro_sort( name, comparison_style, type, compare_function )`

The default macro_sort function is a less function which takes no arguments.  The `_macro_sort` method allows for custom comparisons.  In general, the macro library's approach is to define a customizable underscore prefixed macro and a default macro without the underscores with a common value.

The `macro_cmp.h` file defines a series of comparison_style(s) which are listed below.  If a new style is added to this file, it should allow for the other algorithms to take advantage of it without change!

| Suffix      | Comparison Type                                                            |
|-------------|----------------------------------------------------------------------------|
| cmp_no_arg  | int compare(const type *a, const type *b);                                 |
| cmp_arg     | int compare(const type *a, const type *b, void *arg);                      |
| arg_cmp     | int compare(void *arg, const type *a, const type *b);                      |
| less_no_arg | bool less(const type *a, const type *b);                                   |
| less_arg    | bool less(const type *a, const type *b, void *arg);                        |
| arg_less    | bool less(void *arg, const type *a, const type *b);                        |
| less        | no comparison, expects *(a) < *(b) to function                             |
| cmp         | no comparison, expects *(a) < *(b), *(a) <= *(b), *(a) == *(b) to function |

## Making the functions static and/or static inline

To make the sort function `static` or `static inline`, add it in the line before the macro_sort call.

```c
static
macro_sort(sort_ints, int, compare_int);
```

OR

```c
static inline
macro_sort(sort_ints, int, compare_int);
```

## User specified compare functions

`macro_sort` has a `macro_sort_compare` counterpart which defines a sort function that allows for user specified compare functions.  This xxx_compare function does not require a compare method to be passed to it.

```c
macro_sort_compare(sort_ints, int);

bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}
```

## Creating header file definitions

For each macro, there is a corresponding _h macro, (`macro_sort` => `macro_sort_h`).

some_header.h
```c
macro_sort_compare_h(sort_ints, int);
```

For headers, it is important to add the semicolon.  This produces ...

```c
void sort_ints(int *base, size_t n,
          bool (*cmp)(const int *a, const int *b));
```

Use `static` or other modifiers on the line before if needed.

```c
static inline
macro_sort_compare_h(sort_ints, int);
```

produces

```c
static inline
void sort_ints(int *base, size_t n,
          bool (*cmp)(const int *a, const int *b));
```


## Converting define macros into code

`#define` is a very useful mechanism in C/C++.  However, these statements present challenges during debugging since debuggers won't typically let you step through a macro.  This library helps to address this with the `bin/convert-macros-to-code` script.  The tool will read a source file and replace any macro_...() define statements with their underlying code.

Consider examples/demo/sort_ints.c
```c
#include "the-macro-library/macro_sort.h"

bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}

macro_sort(sort_ints, int, compare_ints);

int main() {
...
```

```bash
$ convert-macros-to-code examples/demo/sort_ints.c | less
...
bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}

void sort_ints(int *base,
          size_t n) {
    int *a, *b, *e;
    int tmp;
    if(n < 17) {
        e = base + n;
        a = base + 1;
        while (a < e) {
            tmp = *a;
            b = a;
...
```

Instead of printing to the terminal, the output can be redirected to a file and compiled.
```bash
$ convert-macros-to-code examples/demo/sort_ints.c > sort_ints_d.c
$ g++ sort_ints_d.cc -o sort_ints_d -g
$ ./sort_ints_d
 1 2 3 4 5
```

Because the macros are expanded, this new program is easy to debug.

# Binary Search

```c
#include "the-macro-library/macro_bsearch.h"

static inline
int compare_int(const int *a, const int *b) {
    return *a - *b;
}

macro_bsearch(bsearch_ints, int, compare_int)
macro_bsearch_first(bsearch_first_ints, int, compare_int)
macro_bsearch_last(bsearch_last_ints, int, compare_int)
macro_bsearch_floor(bsearch_floor_ints, int, compare_int)
macro_bsearch_ceiling(bsearch_ceiling_ints, int, compare_int)
macro_bsearch_lower_bound(bsearch_lower_bound_ints, int, compare_int)
macro_bsearch_upper_bound(bsearch_upper_bound_ints, int, compare_int)
```

This produces the core binary search function `bsearch_ints` and the `first`, `last`, `floor`, `ceiling`, `lower_bound`, and `upper_bound` functions.

```c
macro_bsearch_h(bsearch_ints, int, compare_int);
```

produces

```c
int *bsearch_ints(const int *key, const int *base, size_t n);
```

and it can be used like

```c
    int arr[] = { 1, 3, 3, 5 };
    int key = 3;
    int *r = bsearch(&key, arr, n);
    if(r) // key is found and points to one of the 3s
```

There is also a `macro_bsearch_kv` macro which allows the comparison function to have a different type for the key than that of the array.

# The Set or Map
An implementation of the red black tree using macros and inlined code.

Documentation coming soon.

See `macro_map.h` and `examples/demo/map_ints.c`

## A quick refresher on what each bsearch function does (and roughly the map functions)

Consider the following array

1, 3, 3, 5

### core (the common bsearch method)
Finds any instance of a key or return NULL if none exist.

| key | response | index                    | reason                                  |
|-----| --- |--------------------------|-----------------------------------------|
| 0   | NULL | none | 0 is not in the array                   |
| 1   | 1 | 0 | 1 is found in the first index           |
| 2   | NULL | none | 2 is not in the array                   |
| 3   | 3 | 1 or 2 | it is valid for either 3 to be returned |
| 4   | NULL | none | 4 is not in the array                   |
| 5   | 5 | 3 | 5 is found in the last index            |
| 6   | NULL | none | 6 is not in the array                   |

### first
Finds the first instance of a key or return NULL if none exist.

| key | response | index | reason                                           |
|-----| --- |---|--------------------------------------------------|
| 0   | NULL | none | 0 is not in the array                            |
| 1   | 1 | 0 | 1 is found in the first index                    |
| 2   | NULL | none | 2 is not in the array                            |
| 3   | 3 | 1 | The 3 in the 2nd slot is the first instance of 3 |
| 4   | NULL | none | 4 is not in the array                            |
| 5   | 5 | 3 | 5 is found in the last index                     |
| 6   | NULL | none | 6 is not in the array                            |

### last
Finds the last instance of a key or return NULL if none exist.

| key | response | index | reason                                          |
|-----| --- |-------|-------------------------------------------------|
| 0   | NULL | none  | 0 is not in the array                           |
| 1   | 1 | 0     | 1 is found in the first index                   |
| 2   | NULL | none  | 2 is not in the array                           |
| 3   | 3 | 2     | The 3 in the 3rd slot is the last instance of 3 |
| 4   | NULL | none  | 4 is not in the array                           |
| 5   | 5 | 3     | 5 is found in the last index                    |
| 6   | NULL | none  | 6 is not in the array                           |

### floor
Finds the first instance of a key or the value less than.  NULL is returned if the key is less than any item in the array.

| key | response | index | reason                                         |
|-----|----------|-------|------------------------------------------------|
| 0   | NULL     | none  | 0 is less than any value in the array          |
| 1   | 1        | 0     | 1 is found, see bsearch_first                  |
| 2   | 2        | 0     | 2 is not in the array, 1 is less               |
| 3   | 3        | 1     | 3 is found, see bsearch_first                  |
| 4   | 3        | 2     | 4 is not found, the last 3 is less             |
| 5   | 5        | 3     | 5 is found, see bsearch_first                  |
| 6   | 5        | 3     | 6 is not found, the last 5 is less |

### ceiling
Finds the last instance of a key or the value less than.  NULL is returned if the key is less than any item in the array.

| key | response | index | reason                                        |
|-----| ---|-------|-----------------------------------------------|
| 0   | NULL     | none  | 0 is less than any value in the array         |
| 1   | 1        | 0     | 1 is found, see bsearch_last                  |
| 2   | 2        | 0     | 2 is less than 3 in the 2nd slot, returning 1 |
| 3   | 3        | 2     | 3 is found, see bsearch_last                  |
| 4   | 3        | 2     | 4 is less than 5 in the 4th slot, returning 3 |
| 5   | 5        | 3     | 5 is found, see bsearch_last                  |
| 6   | 5        | 3     | 6 is greater than all items, return last one  |

### lower_bound
Finds the first instance of a key or the value greater.  NULL is returned if the key is greater than any item in the array.

| key | response | index | reason                                                  |
|-----|----------|-------|---------------------------------------------------------|
| 0   | 1        | 0     | 0 is less than any value in the array, return 1st value |
| 1   | 1        | 0     | 1 is found, see bsearch_first                           |
| 2   | 3        | 1     | 3 is the greater value than 2                           |
| 3   | 3        | 1     | 3 is found, see bsearch_first                           |
| 4   | 5        | 3     | 5 is the greater value than 4                           |
| 5   | 5        | 3     | 5 is found, see bsearch_first                           |
| 6   | NULL     | none  | 6 is greater than all items, return NULL                |

### upper_bound
This is different than the others in that the response is always one greater than the key.  It is possible for this to extend beyond the array.  The purpose of upper_bound is to be used in conjunction with lower_bound to form a range.

| key | response  | index | reason                                                  |
|-----|-----------|-------|---------------------------------------------------------|
| 0   | 1         | 0     | 0 is less than any value in the array, return 1st value |
| 1   | 3         | 1     | 1 is found, return next value (3)                       |
| 2   | 3         | 1     | 2 is greater than 1, return next value (3)              |
| 3   | 5         | 3     | 3 is found, return next value (5)                       |
| 4   | 5         | 3     | 4 is greater than 3, return next value (5)              |
| 5   | undefined | 4     | 5 is found, return past array                           |
| 6   | undefined | 4     | 6 is greater than all items, return past array          |

See examples/demo/search_ints.c 

## More to come soon

Contact me at contactandyc@gmail.com