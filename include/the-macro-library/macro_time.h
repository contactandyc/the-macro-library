// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#ifndef _MACRO_TIME_H
#define _MACRO_TIME_H

#include <time.h>
#include <stdint.h>

static inline
uint64_t macro_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static inline
uint64_t macro_now_add_seconds(int32_t seconds) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int64_t now_ns = (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
    int64_t adjusted = now_ns + (int64_t)seconds * 1000000000LL;
    return (uint64_t)adjusted;
}

static inline
double macro_time_diff(uint64_t end, uint64_t start) {
    return (double)(end - start) / 1000000000;
}

#endif
