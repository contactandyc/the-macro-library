// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <time.h>
#include <stdint.h>
#include <string.h>

#include "the-macro-library/macro_time.h"
#include "the-macro-library/macro_test.h"

/* --- tiny sleep helper (nanosleep), tolerant to EINTR --- */
static void sleep_ns(long ns) {
    struct timespec req, rem;
    req.tv_sec  = ns / 1000000000L;
    req.tv_nsec = ns % 1000000000L;
    while (nanosleep(&req, &rem) == -1) {
        req = rem;
    }
}

/* We keep tolerances loose to avoid flakes on busy runners. */
#define NEAR(x, target, tol) ((x) >= (target)-(tol) && (x) <= (target)+(tol))

/* ------------- Tests ------------- */

/* macro_now() should be monotonic non-decreasing */
MACRO_TEST(time_monotonic_non_decreasing) {
    uint64_t t0 = macro_now();
    uint64_t t1 = macro_now();     /* could be equal if same tick */
    MACRO_ASSERT_TRUE(t1 >= t0);

    /* after a short sleep it should strictly advance */
    sleep_ns(2 * 1000 * 1000);     /* ~2ms */
    uint64_t t2 = macro_now();
    MACRO_ASSERT_TRUE(t2 > t1);
}

/* Sleeping should advance time by roughly the requested duration */
MACRO_TEST(time_sleep_progresses_about_5ms) {
    uint64_t t0 = macro_now();
    sleep_ns(5 * 1000 * 1000);               /* 5ms */
    uint64_t t1 = macro_now();

    double dt = macro_time_diff(t1, t0);
    /* Accept between ~2ms and ~50ms to avoid flakes on slow CI */
    MACRO_ASSERT_TRUE(dt >= 0.002 && dt <= 0.050);
}

/* Adding seconds should offset by ~exact seconds when compared to same 'now' */
MACRO_TEST(time_add_seconds_forward_backward) {
    uint64_t base = macro_now();
    uint64_t plus2  = macro_now_add_seconds(2);
    uint64_t minus3 = macro_now_add_seconds(-3);

    /* end >= start for macro_time_diff; check forward directly */
    double d_plus = macro_time_diff(plus2, base);
    MACRO_ASSERT_TRUE(NEAR(d_plus, 2.0, 0.01));      /* within 10ms */

    /* for backward, flip the order (now - (now-3s)) ~ 3s */
    double d_minus = macro_time_diff(base, minus3);
    MACRO_ASSERT_TRUE(NEAR(d_minus, 3.0, 0.01));
}

/* Diff is consistent between two shifted timestamps */
MACRO_TEST(time_diff_between_shifted) {
    uint64_t a = macro_now_add_seconds(5);
    uint64_t b = macro_now_add_seconds(8);
    double d = macro_time_diff(b, a);
    MACRO_ASSERT_TRUE(NEAR(d, 3.0, 0.002));          /* same underlying read + arithmetic */
}

/* Larger offsets: add ~1 day and ensure it’s about one day */
MACRO_TEST(time_add_one_day) {
    const int DAY = 24 * 60 * 60;
    uint64_t base = macro_now();
    uint64_t future = macro_now_add_seconds(DAY);
    double d = macro_time_diff(future, base);
    MACRO_ASSERT_TRUE(NEAR(d, (double)DAY, 0.05));   /* 50ms tolerance */
}

/* Composition: dt(t2,t0) ~= dt(t2,t1)+dt(t1,t0) */
MACRO_TEST(time_diff_composition) {
    uint64_t t0 = macro_now();
    sleep_ns(1 * 1000 * 1000);   /* 1ms */
    uint64_t t1 = macro_now();
    sleep_ns(2 * 1000 * 1000);   /* 2ms */
    uint64_t t2 = macro_now();

    double a = macro_time_diff(t2, t1);
    double b = macro_time_diff(t1, t0);
    double c = macro_time_diff(t2, t0);

    MACRO_ASSERT_TRUE(NEAR(a + b, c, 0.003)); /* allow 3ms jitter */
}

/* Defensive: macro_time_diff must be non-negative when end>=start */
MACRO_TEST(time_diff_non_negative) {
    uint64_t t0 = macro_now();
    uint64_t t1 = macro_now();
    MACRO_ASSERT_TRUE(macro_time_diff(t1, t0) >= 0.0);
}

/* Basic sanity on return types: values are in nanoseconds and not zeroed */
MACRO_TEST(time_units_are_nanoseconds) {
    uint64_t t0 = macro_now();
    sleep_ns(1 * 1000 * 1000); /* 1ms */
    uint64_t t1 = macro_now();
    uint64_t delta_ns = t1 - t0;

    /* Expect roughly 1,000,000 ns (± a lot) */
    MACRO_ASSERT_TRUE(delta_ns > 1000); /* definitely more than 1µs */
    /* Not asserting an upper bound strictly; platforms vary under load */
}

int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;

    MACRO_ADD(tests, time_monotonic_non_decreasing);
    MACRO_ADD(tests, time_sleep_progresses_about_5ms);
    MACRO_ADD(tests, time_add_seconds_forward_backward);
    MACRO_ADD(tests, time_diff_between_shifted);
    MACRO_ADD(tests, time_add_one_day);
    MACRO_ADD(tests, time_diff_composition);
    MACRO_ADD(tests, time_diff_non_negative);
    MACRO_ADD(tests, time_units_are_nanoseconds);

    macro_run_all("the-macro-library", tests, test_count);
    return 0;
}
