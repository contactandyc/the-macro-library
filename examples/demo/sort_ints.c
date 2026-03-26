// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

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
