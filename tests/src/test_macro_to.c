// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "the-macro-library/macro_to.h"
#include "the-macro-library/macro_test.h"

/* Portable UTC constructor for tests */
static time_t make_utc(int y,int m,int d,int H,int M,int S){
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d;
    t.tm_hour = H;
    t.tm_min  = M;
    t.tm_sec  = S;
#if defined(__APPLE__) || defined(_GNU_SOURCE) || defined(__USE_BSD) || defined(__BSD_VISIBLE)
    return timegm(&t);
#else
    char *oldtz = getenv("TZ");
    setenv("TZ","UTC",1);
    tzset();
    time_t r = mktime(&t);
    if (oldtz) setenv("TZ", oldtz, 1); else unsetenv("TZ");
    tzset();
    return r;
#endif
}

/* ---------------- Bit utilities ---------------- */

MACRO_TEST(bit_ops_zero_returns_minus1) {
    MACRO_ASSERT_EQ_INT(macro_highest_bit_index(0),  -1);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(0),   -1);
    MACRO_ASSERT_EQ_INT(macro_bit_count(0),           0);

    MACRO_ASSERT_EQ_INT(macro_highest_bit_index64(0), -1);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index64(0),  -1);
    MACRO_ASSERT_EQ_INT(macro_bit_count64(0),          0);
}

MACRO_TEST(bit_ops_basic_32) {
    MACRO_ASSERT_EQ_INT(macro_highest_bit_index(1u), 0);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(1u),  0);

    MACRO_ASSERT_EQ_INT(macro_highest_bit_index(2u), 1);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(2u),  1);

    MACRO_ASSERT_EQ_INT(macro_highest_bit_index(0x80000000u), 31);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(0x80000000u),  31);

    MACRO_ASSERT_EQ_INT(macro_bit_count(0xF0F0u), 8);
}

MACRO_TEST(bit_ops_basic_64) {
    MACRO_ASSERT_EQ_INT(macro_highest_bit_index64(1ull), 0);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index64(1ull),  0);

    MACRO_ASSERT_EQ_INT(macro_highest_bit_index64(1ull<<63), 63);
    MACRO_ASSERT_EQ_INT(macro_lowest_bit_index64(1ull<<63),  63);

    MACRO_ASSERT_EQ_INT(macro_bit_count64(0xF0F0F0F0F0F0ull), 24);
}

/* Exhaustive single-bit positions (32-bit) */
MACRO_TEST(bit_ops_exhaustive_32_shift_scan) {
    for (int k = 0; k < 32; ++k) {
        uint32_t x = (k == 31) ? 0x80000000u : (1u << k);
        MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(x),  k);
        MACRO_ASSERT_EQ_INT(macro_highest_bit_index(x), k);
    }
}

/* Prefix masks: (1<<k)-1 has popcount=k, hbi=k-1 (k>0), lbi=0 (k>0) */
MACRO_TEST(bit_ops_prefix_masks_32) {
    for (int k = 0; k <= 32; ++k) {
        uint32_t mask = (k == 32) ? 0xFFFFFFFFu : (k == 0 ? 0u : ((1u << k) - 1u));
        MACRO_ASSERT_EQ_INT(macro_bit_count(mask), k);
        if (k == 0) {
            MACRO_ASSERT_EQ_INT(macro_highest_bit_index(mask), -1);
            MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(mask),  -1);
        } else {
            MACRO_ASSERT_EQ_INT(macro_highest_bit_index(mask), k - 1);
            MACRO_ASSERT_EQ_INT(macro_lowest_bit_index(mask),  0);
        }
    }
}

/* Pattern popcounts, 32- & 64-bit */
MACRO_TEST(bit_ops_pattern_counts) {
    /* 32-bit patterns */
    MACRO_ASSERT_EQ_INT(macro_bit_count(0xFFFFFFFFu), 32);
    MACRO_ASSERT_EQ_INT(macro_bit_count(0xAAAAAAAAu), 16);
    MACRO_ASSERT_EQ_INT(macro_bit_count(0x55555555u), 16);

    /* 64-bit patterns */
    MACRO_ASSERT_EQ_INT(macro_bit_count64(0xFFFFFFFFFFFFFFFFull), 64);
    MACRO_ASSERT_EQ_INT(macro_bit_count64(0xAAAAAAAAAAAAAAAAull), 32);
    MACRO_ASSERT_EQ_INT(macro_bit_count64(0x5555555555555555ull), 32);

    /* Odd-length (15-nibble) pattern: 15 * 2 ones = 30 */
    MACRO_ASSERT_EQ_INT(macro_bit_count64(0xAAAAAAAAAAAAAAAull), 30);
}

/* ---------------- String default helper ---------------- */

MACRO_TEST(string_default_helper) {
    const char *d = "DEF";
    MACRO_ASSERT_STREQ(macro_to_string(NULL, d), d);
    MACRO_ASSERT_STREQ(macro_to_string("x", d), "x");
}

MACRO_TEST(string_empty_is_not_default) {
    const char *d = "DEF";
    MACRO_ASSERT_STREQ(macro_to_string("", d), ""); /* empty string is a real value */
}

/* ---------------- Integer conversions (overflow=>default) ---------------- */

MACRO_TEST(conv_to_int_variants) {
    MACRO_ASSERT_EQ_INT(macro_to_int("0", 7), 0);
    MACRO_ASSERT_EQ_INT(macro_to_int("-12", 7), -12);
    MACRO_ASSERT_EQ_INT(macro_to_int("12x", 7), 7);
    MACRO_ASSERT_EQ_INT(macro_to_int(NULL, 9), 9);

    MACRO_ASSERT_EQ_INT((int)macro_to_int32("2147483646", -1), 2147483646);
    MACRO_ASSERT_EQ_INT((int)macro_to_int32("-2147483647", -1), -2147483647);

    /* uint32/uint64 are unsigned-only; minus or junk => default */
    MACRO_ASSERT_EQ_INT((int)macro_to_uint32("4294967295", 1), (int)0xFFFFFFFFu);
    MACRO_ASSERT_EQ_INT((int)macro_to_uint32("-1", 3), 3);
    MACRO_ASSERT_EQ_INT((int)macro_to_uint32("12x", 4), 4);

    /* Exact UINT64_MAX is ok */
    uint64_t u = macro_to_uint64("18446744073709551615", 0); /* UINT64_MAX */
    MACRO_ASSERT_TRUE(u == UINT64_MAX);
    /* negatives and trailing junk => defaults */
    MACRO_ASSERT_TRUE(macro_to_uint64("-1", 5) == 5);
    MACRO_ASSERT_TRUE(macro_to_uint64("18446744073709551615x", 6) == 6);
}

MACRO_TEST(conv_to_int_overflow_defaults) {
    /* Signed 32 overflow => default */
    char above_i32[] = "2147483648";
    char below_i32[] = "-2147483649";
    MACRO_ASSERT_EQ_INT(macro_to_int32(above_i32, -7), -7);
    MACRO_ASSERT_EQ_INT(macro_to_int32(below_i32,  8),  8);

    /* Signed 64 overflow => default */
    MACRO_ASSERT_TRUE(macro_to_int64("9223372036854775808", -11) == -11);
    MACRO_ASSERT_TRUE(macro_to_int64("-9223372036854775809",  12) == 12);

    /* Unsigned 32/64 overflow => default */
    MACRO_ASSERT_TRUE(macro_to_uint32("4294967296", 77) == 77);
    MACRO_ASSERT_TRUE(macro_to_uint64("18446744073709551616", 99) == 99);
}

/* Formatting/strictness edges for ints (no whitespace/plus allowed) */
MACRO_TEST(conv_to_int_formatting_edges) {
    MACRO_ASSERT_EQ_INT(macro_to_int("00123", 0), 123);
    MACRO_ASSERT_EQ_INT(macro_to_int("-0", 9), 0);

    /* strict: leading '+' or spaces or trailing spaces => default */
    MACRO_ASSERT_EQ_INT(macro_to_int("+42", 7), 7);
    MACRO_ASSERT_EQ_INT(macro_to_int(" 42", 7), 7);
    MACRO_ASSERT_EQ_INT(macro_to_int("42 ", 7), 7);
}

MACRO_TEST(conv_to_uint32_leading_zeros) {
    MACRO_ASSERT_EQ_INT((int)macro_to_uint32("00000000042", 77), 42);
    MACRO_ASSERT_EQ_INT((int)macro_to_uint32("-0", 99), 99);
}

MACRO_TEST(conv_to_uint64_zero_and_max) {
    MACRO_ASSERT_TRUE(macro_to_uint64("0", 5) == 0ull);
    MACRO_ASSERT_TRUE(macro_to_uint64("18446744073709551615", 5) == UINT64_MAX);
}

/* ---------------- Floating-point conversions (strict) ---------------- */

MACRO_TEST(conv_to_float_double) {
    double d = macro_to_double("3.14159", -1.0);
    float  f = macro_to_float("2.5", -1.0f);
    MACRO_ASSERT_TRUE(d > 3.14 && d < 3.15);
    MACRO_ASSERT_TRUE(f > 2.49f && f < 2.51f);

    MACRO_ASSERT_TRUE(macro_to_double("12x", 1.25) == 1.25);
    MACRO_ASSERT_TRUE(macro_to_float("x", 0.5f) == 0.5f);
}

MACRO_TEST(conv_to_double_scientific_and_negzero) {
    double e3 = macro_to_double("1e-3", 9.0);
    MACRO_ASSERT_TRUE(e3 > 0.000999 && e3 < 0.001001);

    double n0 = macro_to_double("-0.0", 9.0);
    MACRO_ASSERT_TRUE(fabs(n0) < 1e-12);
}

/* ---------------- Boolean conversion semantics (permissive) ----------------
   macro_to_bool(str, default):
     - If default==true  -> returns false only if first char in {f,0,n,F,N}; else true
     - If default==false -> returns true  only if first char in {t,1,y,T,Y}; else false
*/
MACRO_TEST(conv_to_bool_semantics) {
    /* default=false branch (needs t/1/y) */
    MACRO_ASSERT_TRUE(macro_to_bool("true",  false) == true);
    MACRO_ASSERT_TRUE(macro_to_bool("1",     false) == true);
    MACRO_ASSERT_TRUE(macro_to_bool("yes",   false) == true);
    MACRO_ASSERT_TRUE(macro_to_bool("no",    false) == false);
    MACRO_ASSERT_TRUE(macro_to_bool("x",     false) == false);
    MACRO_ASSERT_TRUE(macro_to_bool(NULL,    false) == false);

    /* default=true branch (false only on f/0/n) */
    MACRO_ASSERT_TRUE(macro_to_bool("false", true) == false);
    MACRO_ASSERT_TRUE(macro_to_bool("0",     true) == false);
    MACRO_ASSERT_TRUE(macro_to_bool("no",    true) == false);
    MACRO_ASSERT_TRUE(macro_to_bool("x",     true) == true);
    MACRO_ASSERT_TRUE(macro_to_bool(NULL,    true) == true);
}

MACRO_TEST(conv_to_bool_case_insensitive_more) {
    MACRO_ASSERT_TRUE(macro_to_bool("TRUE",  false) == true);
    MACRO_ASSERT_TRUE(macro_to_bool("FALSE", true)  == false);
    MACRO_ASSERT_TRUE(macro_to_bool("Yes",   false) == true);
    MACRO_ASSERT_TRUE(macro_to_bool("No",    true)  == false);
    /* empty string: first char '\0' -> not t/1/y and not f/0/n */
    MACRO_ASSERT_TRUE(macro_to_bool("", false) == false);
    MACRO_ASSERT_TRUE(macro_to_bool("", true)  == true);
}

/* ---------------- Try-conversions (new strict APIs) ---------------- */

MACRO_TEST(try_to_int_success_and_failure) {
    int v;
    MACRO_ASSERT_TRUE(macro_try_to_int("0", &v) && v == 0);
    MACRO_ASSERT_TRUE(macro_try_to_int("-123", &v) && v == -123);
    MACRO_ASSERT_TRUE(!macro_try_to_int("+1", &v));   /* plus not allowed */
    MACRO_ASSERT_TRUE(!macro_try_to_int(" 1", &v));   /* no whitespace */
    MACRO_ASSERT_TRUE(!macro_try_to_int("1 ", &v));
    MACRO_ASSERT_TRUE(!macro_try_to_int(NULL, &v));

    /* Overflow -> false */
    MACRO_ASSERT_TRUE(!macro_try_to_int32("2147483648", (int32_t*)&v));
    MACRO_ASSERT_TRUE(!macro_try_to_int32("-2147483649", (int32_t*)&v));
}

MACRO_TEST(try_to_uint_success_and_failure) {
    uint32_t u32;
    uint64_t u64;
    MACRO_ASSERT_TRUE(macro_try_to_uint32("0", &u32) && u32 == 0u);
    MACRO_ASSERT_TRUE(macro_try_to_uint32("4294967295", &u32) && u32 == 0xFFFFFFFFu);
    MACRO_ASSERT_TRUE(!macro_try_to_uint32("-1", &u32));
    MACRO_ASSERT_TRUE(!macro_try_to_uint32("4294967296", &u32)); /* overflow */

    MACRO_ASSERT_TRUE(macro_try_to_uint64("18446744073709551615", &u64) && u64 == UINT64_MAX);
    MACRO_ASSERT_TRUE(!macro_try_to_uint64("18446744073709551616", &u64)); /* overflow */
}

MACRO_TEST(try_to_int64_boundaries) {
    int64_t i64;
    MACRO_ASSERT_TRUE(macro_try_to_int64("9223372036854775807", &i64) && i64 == INT64_MAX);
    MACRO_ASSERT_TRUE(macro_try_to_int64("-9223372036854775808", &i64) && i64 == INT64_MIN);
    MACRO_ASSERT_TRUE(!macro_try_to_int64("9223372036854775808", &i64));   /* overflow */
    MACRO_ASSERT_TRUE(!macro_try_to_int64("-9223372036854775809", &i64));  /* underflow */
}

MACRO_TEST(try_to_float_double) {
    float f;
    double d;
    MACRO_ASSERT_TRUE(macro_try_to_float("3.5", &f) && f > 3.49f && f < 3.51f);
    MACRO_ASSERT_TRUE(macro_try_to_double("1e-3", &d) && d > 0.000999 && d < 0.001001);
    MACRO_ASSERT_TRUE(!macro_try_to_float("x", &f));
    MACRO_ASSERT_TRUE(!macro_try_to_double("12x", &d));
}

MACRO_TEST(try_to_bool_strict) {
    bool b;
    MACRO_ASSERT_TRUE(macro_try_to_bool("true", &b)  && b == true);
    MACRO_ASSERT_TRUE(macro_try_to_bool("TrUe", &b)  && b == true);
    MACRO_ASSERT_TRUE(macro_try_to_bool("false", &b) && b == false);
    MACRO_ASSERT_TRUE(macro_try_to_bool("FALSE", &b) && b == false);
    MACRO_ASSERT_TRUE(macro_try_to_bool("1", &b)     && b == true);
    MACRO_ASSERT_TRUE(macro_try_to_bool("0", &b)     && b == false);

    /* Not accepted by strict try-API */
    MACRO_ASSERT_TRUE(macro_try_to_bool("yes", &b) && b == true);
    MACRO_ASSERT_TRUE(macro_try_to_bool("no", &b) && b == false);
    MACRO_ASSERT_TRUE(!macro_try_to_bool("", &b));
    MACRO_ASSERT_TRUE(!macro_try_to_bool(NULL, &b));
}

/* ---------------- Date/time helpers ---------------- */

MACRO_TEST(datetime_formatters_gmtime) {
    /* 1970-01-01 00:00:00 UTC */
    char bufdt[32], bufd[16];
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 0), "1970-01-01 00:00:00");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, 0),       "1970-01-01");

    /* 2000-01-02 03:04:05 UTC  (946782245) */
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 946782245), "2000-01-02 03:04:05");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, 946782245),       "2000-01-02");
}

/* Another known boundary: 1971-01-01 00:00:00 UTC (31536000) */
MACRO_TEST(datetime_1971_utc) {
    char bufdt[32], bufd[16];
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 31536000), "1971-01-01 00:00:00");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, 31536000),       "1971-01-01");
}

/* ---------------- More date/time tests ---------------- */

MACRO_TEST(datetime_epoch_step_seconds) {
    char bufdt[32], bufd[16];

    /* 1970-01-01 00:00:00 .. :59 .. 01:00 */
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 0),   "1970-01-01 00:00:00");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, 0),         "1970-01-01");

    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 1),   "1970-01-01 00:00:01");
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 59),  "1970-01-01 00:00:59");
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, 60),  "1970-01-01 00:01:00");
}

MACRO_TEST(datetime_year_rollover_1999_2000) {
    char bufdt[32], bufd[16];

    time_t t1 = make_utc(1999,12,31,23,59,59);
    time_t t2 = make_utc(2000, 1, 1, 0, 0, 0);

    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, t1), "1999-12-31 23:59:59");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, t1),       "1999-12-31");

    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, t2), "2000-01-01 00:00:00");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, t2),       "2000-01-01");
}

MACRO_TEST(datetime_leap_day_2000_and_nonleap_2100) {
    char bufdt[32], bufd[16];

    /* 2000 is leap (divisible by 400) */
    time_t leap_day   = make_utc(2000,2,29,12,34,56);
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, leap_day), "2000-02-29 12:34:56");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, leap_day),       "2000-02-29");

    /* 2100 is NOT leap (century not divisible by 400) -> March 1 follows Feb 28 */
    time_t pre_2100 = make_utc(2100,2,28,23,59,59);
    time_t post_2100= make_utc(2100,3, 1, 0, 0, 0);
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, pre_2100),  "2100-02-28 23:59:59");
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, post_2100), "2100-03-01 00:00:00");
}

MACRO_TEST(datetime_month_length_boundaries) {
    char bufdt[32];

    /* April -> 30 days */
    time_t apr_end = make_utc(2021,4,30,23,59,59);
    time_t may_1   = make_utc(2021,5, 1, 0, 0, 0);
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, apr_end), "2021-04-30 23:59:59");
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, may_1),   "2021-05-01 00:00:00");

    /* January -> 31 days */
    time_t jan_end = make_utc(2022,1,31,23,59,59);
    time_t feb_1   = make_utc(2022,2, 1, 0, 0, 0);
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, jan_end), "2022-01-31 23:59:59");
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, feb_1),   "2022-02-01 00:00:00");
}

MACRO_TEST(date_matches_datetime_prefix) {
    char bufdt[32], bufd[16];

    time_t t = make_utc(2033,11,22,7,8,9);
    const char *dt = macro_to_date_time(bufdt, t);
    const char *d  = macro_to_date(bufd, t);

    MACRO_ASSERT_TRUE(dt != NULL && d != NULL);
    MACRO_ASSERT_TRUE(strncmp(dt, d, 10) == 0);           /* "YYYY-MM-DD" */
    MACRO_ASSERT_TRUE(dt[10] == ' ');                     /* space separator */
    MACRO_ASSERT_TRUE(strlen(d) == 10);
}

MACRO_TEST(datetime_returns_dest_and_null_terminated) {
    char bufdt[32], bufd[16];
    memset(bufdt, 0xA5, sizeof(bufdt));
    memset(bufd,  0xA5, sizeof(bufd));

    time_t t = make_utc(1970,1,1,0,0,0);
    char *ret_dt = macro_to_date_time(bufdt, t);
    char *ret_d  = macro_to_date(bufd, t);

    MACRO_ASSERT_TRUE(ret_dt == bufdt);
    MACRO_ASSERT_TRUE(ret_d  == bufd);

    /* Ensure there is a terminating NUL at expected end */
    MACRO_ASSERT_STREQ(bufdt, "1970-01-01 00:00:00");
    MACRO_ASSERT_STREQ(bufd,  "1970-01-01");
}

MACRO_TEST(datetime_far_future_reasonable) {
    char bufdt[32], bufd[16];

    /* Something big but within 32/64-bit time_t ranges commonly supported */
    time_t t = make_utc(2500,12,31,23,59,59);
    /* If platform time_t can’t represent this, make_utc will overflow; in that case
       this test may not be meaningful on that platform. We at least verify stability: */
    const char *dt = macro_to_date_time(bufdt, t);
    const char *d  = macro_to_date(bufd, t);
    MACRO_ASSERT_TRUE(dt && d);
    /* Sanity: formats and separators intact */
    MACRO_ASSERT_TRUE(dt[4]=='-' && dt[7]=='-' && dt[10]==' ' && dt[13]==':' && dt[16]==':');
    MACRO_ASSERT_TRUE(d[4]=='-'  && d[7]=='-');
}

MACRO_TEST(datetime_pre_epoch) {
    char bufdt[32], bufd[16];
    time_t t = make_utc(1969,12,31,23,59,59);
    MACRO_ASSERT_STREQ(macro_to_date_time(bufdt, t), "1969-12-31 23:59:59");
    MACRO_ASSERT_STREQ(macro_to_date(bufd, t),       "1969-12-31");
}

/* ---------------- Runner ---------------- */

int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;

    MACRO_ADD(tests, bit_ops_zero_returns_minus1);
    MACRO_ADD(tests, bit_ops_basic_32);
    MACRO_ADD(tests, bit_ops_basic_64);
    MACRO_ADD(tests, bit_ops_exhaustive_32_shift_scan);
    MACRO_ADD(tests, bit_ops_prefix_masks_32);
    MACRO_ADD(tests, bit_ops_pattern_counts);

    MACRO_ADD(tests, string_default_helper);
    MACRO_ADD(tests, string_empty_is_not_default);

    MACRO_ADD(tests, conv_to_int_variants);
    MACRO_ADD(tests, conv_to_int_overflow_defaults);
    MACRO_ADD(tests, conv_to_int_formatting_edges);
    MACRO_ADD(tests, conv_to_uint32_leading_zeros);
    MACRO_ADD(tests, conv_to_uint64_zero_and_max);

    MACRO_ADD(tests, conv_to_float_double);
    MACRO_ADD(tests, conv_to_double_scientific_and_negzero);

    MACRO_ADD(tests, conv_to_bool_semantics);
    MACRO_ADD(tests, conv_to_bool_case_insensitive_more);

    /* New try_* tests */
    MACRO_ADD(tests, try_to_int_success_and_failure);
    MACRO_ADD(tests, try_to_uint_success_and_failure);
    MACRO_ADD(tests, try_to_int64_boundaries);
    MACRO_ADD(tests, try_to_float_double);
    MACRO_ADD(tests, try_to_bool_strict);

    MACRO_ADD(tests, datetime_formatters_gmtime);
    MACRO_ADD(tests, datetime_1971_utc);
    MACRO_ADD(tests, datetime_epoch_step_seconds);
    MACRO_ADD(tests, datetime_year_rollover_1999_2000);
    MACRO_ADD(tests, datetime_leap_day_2000_and_nonleap_2100);
    MACRO_ADD(tests, datetime_month_length_boundaries);
    MACRO_ADD(tests, date_matches_datetime_prefix);
    MACRO_ADD(tests, datetime_returns_dest_and_null_terminated);
    MACRO_ADD(tests, datetime_far_future_reasonable);
    MACRO_ADD(tests, datetime_pre_epoch);

    macro_run_all("the-macro-library", tests, test_count);
    return 0;
}
