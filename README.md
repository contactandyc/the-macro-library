# the-macro-library

## Installation

```bash
mkdir -p build
cd build
cmake ..
make
make install
```

## A Fast C Sort (with more to come)
This library is entirely header based and requires no special linkage.  A comparison of this sort to qsort and std::sort follows.

starting from root directory (assuming package was built - see last section)
```bash
cd build/examples/speed-test
./speed_test 1000 10000 
```

```
Sorting 1000 elements 10000 times which are of size 20

                            test name	  sort_items	  std_sort	         gain %
Ascending Values
                            ascending	    0.006563	  0.029219	         345.21
Equal Values
                     ascending[gap=0]	    0.013708	  0.029565	         115.68
Descending Values
                           descending	    0.013612	  0.065414	         380.56
Saw Pattern
 saw[period=50,max=1000,increment=10]	    0.082563	  0.187881	         127.56
saw[period=50,max=-1000,increment=20]	    0.074940	  0.146521	          95.52

Random Values
                       random[seed=1]	    0.118494	  0.231757	          95.59
                       random[seed=0]	    0.123645	  0.208858	          68.92
                       random[seed=0]	    0.139703	  0.208990	          49.60
Random between 0-99
             rand_max[seed=1,max=100]	    0.088512	  0.167277	          88.99
             rand_max[seed=0,max=100]	    0.087671	  0.169538	          93.38
             rand_max[seed=0,max=100]	    0.088723	  0.164302	          85.19
Random between 0-9999
           rand_max[seed=1,max=10000]	    0.136579	  0.202220	          48.06
           rand_max[seed=0,max=10000]	    0.128565	  0.214389	          66.76
           rand_max[seed=0,max=10000]	    0.174204	  0.223062	          28.05
Random for first 25%
             rand_head[seed=1,pct=25]	    0.099045	  0.151807	          53.27
             rand_head[seed=0,pct=25]	    0.099430	  0.178332	          79.35
             rand_head[seed=0,pct=25]	    0.098464	  0.183933	          86.80
Random for last 25%
             rand_tail[seed=1,pct=25]	    0.067868	  0.079599	          17.29
             rand_tail[seed=0,pct=25]	    0.067383	  0.083243	          23.54
             rand_tail[seed=0,pct=25]	    0.068252	  0.088028	          28.97

Sorting 1000 elements 10000 times which are of size 20

                            test name	  sort_items	     qsort	         gain %
Ascending Values
                            ascending	    0.006701	  0.041219	         515.12
Equal Values
                     ascending[gap=0]	    0.012256	  0.059348	         384.24
Descending Values
                           descending	    0.013634	  0.259071	        1800.18
Saw Pattern
 saw[period=50,max=1000,increment=10]	    0.081647	  0.229434	         181.01
saw[period=50,max=-1000,increment=20]	    0.074231	  0.203322	         173.90

Random Values
                       random[seed=1]	    0.115585	  0.458976	         297.09
                       random[seed=0]	    0.114679	  0.464667	         305.19
                       random[seed=0]	    0.152963	  0.468421	         206.23
Random between 0-99
             rand_max[seed=1,max=100]	    0.084674	  0.228831	         170.25
             rand_max[seed=0,max=100]	    0.087867	  0.230848	         162.72
             rand_max[seed=0,max=100]	    0.089134	  0.235022	         163.67
Random between 0-9999
           rand_max[seed=1,max=10000]	    0.135200	  0.488818	         261.55
           rand_max[seed=0,max=10000]	    0.132019	  0.485112	         267.46
           rand_max[seed=0,max=10000]	    0.176569	  0.488761	         176.81
Random for first 25%
             rand_head[seed=1,pct=25]	    0.097860	  0.272600	         178.56
             rand_head[seed=0,pct=25]	    0.103751	  0.289281	         178.82
             rand_head[seed=0,pct=25]	    0.097597	  0.274428	         181.18
Random for last 25%
             rand_tail[seed=1,pct=25]	    0.068167	  0.180194	         164.34
             rand_tail[seed=0,pct=25]	    0.064350	  0.168062	         161.17
             rand_tail[seed=0,pct=25]	    0.067563	  0.181413	         168.51
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

`#define` is a very useful mechanism in C/C++.  However, these statements present challenges during debugging since debuggers won't typically let you step through a macro.  This library helps to address this with the `bin/convert_macros_to_code.py` script.  The tool will read a source file and replace any macro_...() define statements with their underlying code.

Consider examples/demo/sort_ints.c
```c
#include "macro_sort.h"

macro_sort_cmp(sort_ints, int);

int main() {
...
```

```bash
$ python3 bin/convert_macros_to_code.py examples/demo/sort_ints.c | less
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
$ python3 bin/convert_macros_to_code.py examples/demo/sort_ints.c > sort_ints_d.c
$ g++ sort_ints_d.cc -o sort_ints_d -g
$ ./sort_ints_d
 1 2 3 4 5
```

Because the macros are expanded, this new program is easy to debug.






```bash
python3 bin/convert_macros_to_code.py examples/cpp/test_sort.cc include/ > test_sort_expanded.cc
g++ test_sort_expanded.cc -o test_sort_expanded -g
lldb ./test_sort_expanded random
```
