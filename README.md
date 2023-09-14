# the-macro-library
The macro_sort performs significantly better than qsort and better than std::sort.  This test was run on a MacBook Pro 2020 2.3 GHz Quad-Core Intel Core i7 with 32 GB 3733 MHz LPDDR4X RAM.

![Speed Comparison](images/BarChart.png)

## Installation

```bash
mkdir -p build
cd build
cmake ..
make
make install
```

To use the python utilities

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

## Lightweight

This library is entirely header based and requires no special linkage.

## Simple to use

```c
#include "macro_sort.h"

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

macro_sort_cmp_no_arg(sort_ints, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, 5);
    ...    
```

## Debuggable

C macros are difficult to debug with most debuggers.  The included `bin/convert-macros-to-code.py` will convert macros to their base code and preserve readability.  Additionally, the convert tool can be used to create functions which have no dependency on this library.

## Extendable

This library has started with introsort and a series of binary search methods.  Other algorithms are planned to be added soon.

## A Fast C Sort (with more to come)
A comparison of this sort to qsort and std::sort follows.

starting from root directory (assuming package was built - see last section)
```bash
cd build/examples/speed-test
./speed_test 1000000 100 
macro-plot.py
```

```
Sorting 100000 elements 1000 times which are of size 20

              test name	  macro_sort	  std_sort	     qsort
              ascending	    0.136391	  0.346498	  0.490240
       ascending[gap=0]	    0.227794	  0.359617	  0.660179
             descending	    0.246166	  0.683351	  2.450071
 saw[period=10,max=100]	    0.695768	  1.219611	  1.582721
saw[period=10,max=-100]	    0.691319	  1.274635	  1.572666
                 random	    6.480880	  8.028105	 10.640059
      rand_max[max=100]	    2.505797	  3.630266	  4.067321
    rand_max[max=10000]	    5.956528	  7.415671	  8.924805
      rand_head[pct=25]	    2.789506	  3.954147	  6.123680
      rand_tail[pct=25]	    3.011739	  4.204019	  5.975161
```

Across nearly all measures for the given test, the macro library performs better than std::sort and significantly better than qsort.  I've found that for raw types, the std::sort tends to perform about the same as the macro versions.

See examples/speed-test/speed_test.cc and feel free to modify the test. 

## The macro sort is the introsort + a sort check at the beginning.

The sort check at the beginning considers the first and last element of the array and then expects the middle element to be between them.  If the middle element is not between them, the introsort continues.  Otherwise, two conditions remain.  The first is that the first element is less than or equal to the last element.  In this case a few additional elements are compared to further check that the array is sorted.  If any of those checks fail, the introsort loop is continued.  Otherwise, the array is checked from the beginning to the end and if it is entirely sorted, the sort is finished.  If the first element is greater than the last element, then the same comparisions continue in reverse. Once the array is determined to be completely reversed, all elements are swapped.

## A simple example

examples/demo/sort_ints.c
```c
#include "macro_sort.h"

macro_sort_cmp(sort_ints, int);

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

## macro_sort_xxx

There are a series of macro_sort macros to allow for different compare methods.  `macro_sort_cmp` is used when the `type` (in this case `int`), can respond to `<`, `<=`, and `==`.

    
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


In the simple example above, macro_sort_cmp was used (defined in `include/macro_sort.h`).

examples/demo/sort_ints_cmp_no_arg.c
```c
...
int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

macro_sort_cmp_no_arg(sort_ints, int, compare_ints);
...
```

The program outputs the same result as sort_ints above.
```bash
$ gcc examples/demo/sort_ints_cmp_no_arg.c -o sort_ints_cmp_no_arg -O3
$ ./sort_ints_cmp_no_arg
 1 2 3 4 5
```

Using less_no_arg

examples/demo/sort_ints_less_no_arg.c
```c
...
bool compare_ints(const int *a, const int *b) {
    return *a < *b;
}

macro_sort_less_no_arg(sort_ints, int, compare_ints);
...
```

All of these macros so far are producing a function named `sort_ints` with the following signature.

```c
void sort_ints(int *base, size_t n); 
```

The functions have the same signature, but implement the compare method differently.  In general, the goal of this project is to allow the end user to use the comparison approach that best fits their project.

## macro_sort_compare_xxx

These macros are similar to the `macro_sort_xxx` except that they create a function which allows the end user to pass in a compare function.

The second example,

examples/demo/sort_ints_cmp_no_arg.c
```c
#include "macro_sort.h"

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

macro_sort_cmp_no_arg(sort_ints, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]));
...
```

=> 

examples/demo/sort_ints_compare_cmp_no_arg.c
```c
#include "macro_sort.h"

macro_sort_compare_cmp_no_arg(sort_ints, int);

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), compare_ints);
...
```

## arg

Sometimes it is nice to be able to pass an extra argument to the compare function. The following suffixes can be used.

 Suffix      | Comparison Type                                                            |
|-------------|----------------------------------------------------------------------------|
| cmp_arg     | int compare(const type *a, const type *b, void *arg);                      |
| arg_cmp     | int compare(void *arg, const type *a, const type *b);                      |
| less_arg    | bool less(const type *a, const type *b, void *arg);                        |
| arg_less    | bool less(void *arg, const type *a, const type *b);                        |

examples/demo/sort_ints_cmp_arg.c
```c
#include "macro_sort.h"

int compare_ints(const int *a, const int *b, void *arg) {
    return *a - *b;
}

macro_sort_cmp_arg(sort_ints, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    void *arg = NULL;
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), arg);
```

arg_cmp only changes the following lines

examples/demo/sort_ints_arg_cmp.c
```c
int compare_ints(void *arg, const int *a, const int *b) {
    return *a - *b;
}

macro_sort_arg_cmp(sort_ints, int, compare_ints);
```

Allowing the compare function to be passed into sort function as an argument

examples/demo/sort_ints_compare_arg_cmp.c
```c
#include "macro_sort.h"

macro_sort_compare_arg_cmp(sort_ints, int);

int compare_ints(void *arg, const int *a, const int *b) {
    return *a - *b;
}

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    void *arg = NULL;
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]), compare_ints, arg);
```

## Making the functions static and/or static inline

To make the sort function `static` or `static inline`, add it in the line before the macro_sort call.

```c
static
macro_sort_cmp(sort_ints, int);
```

OR

```c
static inline
macro_sort_cmp(sort_ints, int);
```

## Creating header file definitions

Use the `macro_sort_h_xxx` or `macro_sort_compare_h_xxx` macros.

```c
macro_sort_compare_h_arg_less(sort_ints, int);
```

produces

```c
void sort_ints(int *base, size_t n,
          bool (*cmp)(void *arg, const int *a, const int *b),
          void *arg);
```

Use `static` or other modifiers on the line before if needed.

```c
static inline
macro_sort_h_less(sort_ints, int);
```

produces

```c
static inline
void sort_ints(int *base, size_t n);
```


## Converting define macros into code

`#define` is a very useful mechanism in C/C++.  However, these statements present challenges during debugging since debuggers won't typically let you step through a macro.  This library helps to address this with the `bin/convert-macros-to-code.py` script.  The tool will read a source file and replace any macro_...() define statements with their underlying code.

Consider examples/demo/sort_ints.c
```c
#include "macro_sort.h"

macro_sort_cmp(sort_ints, int);

int main() {
...
```

```bash
$ convert-macros-to-code.py examples/demo/sort_ints.c | less
...
void sort_ints(int *base, size_t n) {
    int *a, *b, *e;
    int tmp;
    if(n < 17) {
        e = base + n;
        a = base + 1;
        while (a < e) {
            tmp = *a;
...
```

Instead of printing to the terminal, the output can be redirected to a file and compiled.
```bash
$ convert-macros-to-code.py examples/demo/sort_ints.c > sort_ints_d.c
$ g++ sort_ints_d.cc -o sort_ints_d -g
$ ./sort_ints_d
 1 2 3 4 5
```

Because the macros are expanded, this new program is easy to debug.

# Binary Search

## A quick refresher on what each function does

Consider the following array

1, 3, 3, 5

### bsearch
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

### bsearch_first
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

### bsearch_last
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

### bsearch_floor
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

### bsearch_ceiling
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

### bsearch_lower_bound
Finds the first instance of a key or the value greater.  NULL is returned if the key is greater than any item in the array.

success(bsearch_lower_bound): key(1) 1 at position 0 returned
success(bsearch_lower_bound): key(2) 3 at position 1 returned
success(bsearch_lower_bound): key(3) 3 at position 1 returned
success(bsearch_lower_bound): key(4) 5 at position 3 returned
success(bsearch_lower_bound): key(5) 5 at position 3 returned
success(bsearch_lower_bound): key(6) NULL returned

| key | response | index | reason                                                  |
|-----|----------|-------|---------------------------------------------------------|
| 0   | 1        | 0     | 0 is less than any value in the array, return 1st value |
| 1   | 1        | 0     | 1 is found, see bsearch_first                           |
| 2   | 3        | 1     | 3 is the greater value than 2                           |
| 3   | 3        | 1     | 3 is found, see bsearch_first                           |
| 4   | 5        | 3     | 5 is the greater value than 4                           |
| 5   | 5        | 3     | 5 is found, see bsearch_first                           |
| 6   | NULL     | none  | 6 is greater than all items, return NULL                |

### bsearch_upper_bound
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