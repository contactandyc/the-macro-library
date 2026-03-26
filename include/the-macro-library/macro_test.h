// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef MACRO_TEST_H
#define MACRO_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Config toggles (can be overridden by compiler -D flags) ---- */
#ifndef MACRO_TEST_ENABLE_COLOR
#define MACRO_TEST_ENABLE_COLOR 1
#endif

#ifndef MACRO_TEST_ENABLE_TIMING_US
#define MACRO_TEST_ENABLE_TIMING_US 0
#endif

/* ---- Includes ---- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if MACRO_TEST_ENABLE_TIMING_US
  #include <sys/time.h>
  static inline long macro_test_now_us(void) {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      return (tv.tv_sec * 1000000L) + tv.tv_usec;
  }
#endif

/* ---- Color helpers ---- */
#if MACRO_TEST_ENABLE_COLOR
  #define MACRO__CLR_GREEN "\x1b[32m"
  #define MACRO__CLR_RED   "\x1b[31m"
  #define MACRO__CLR_CYAN  "\x1b[36m"
  #define MACRO__CLR_RESET "\x1b[0m"
#else
  #define MACRO__CLR_GREEN ""
  #define MACRO__CLR_RED   ""
  #define MACRO__CLR_CYAN  ""
  #define MACRO__CLR_RESET ""
#endif

/* ---- Types ---- */
typedef void (*macro_test_fn)(void);
typedef struct { const char *name; macro_test_fn fn; } macro_test_case;

/* ---- Decls ---- */
static inline int  macro_streq(const char *a, const char *b);
static inline void macro_run_all(const char *suite, macro_test_case *t, size_t n);

/* ---- API macros ---- */
#define MACRO_TEST(name) static void name(void)
#define MACRO_ADD(list, fn) do { list[test_count++] = (macro_test_case){#fn, fn}; } while (0)

/* ---- Runner ---- */
static inline void macro_run_all(const char *suite, macro_test_case *t, size_t n) {
    size_t passed = 0;
    printf("[====] Running %zu tests in %s\n", n, suite);
    for (size_t i = 0; i < n; ++i) {
    #if MACRO_TEST_ENABLE_TIMING_US
        long t0 = macro_test_now_us();
    #endif
        t[i].fn();
        passed++;
    #if MACRO_TEST_ENABLE_TIMING_US
        long dt = macro_test_now_us() - t0;
        printf("%s[PASS]%s %s %s(+%ld us)%s\n",
               MACRO__CLR_GREEN, MACRO__CLR_RESET, t[i].name,
               MACRO__CLR_CYAN, dt, MACRO__CLR_RESET);
    #else
        printf("%s[PASS]%s %s\n", MACRO__CLR_GREEN, MACRO__CLR_RESET, t[i].name);
    #endif
    }
    printf("[====] %zu/%zu passed\n", passed, n);
}

/* ---- Assertions ---- */
#define MACRO_ASSERT_TRUE(expr) do { if (!(expr)) {                                                           \
    fprintf(stderr,"%s[FAIL]%s %s:%d: %s\n", MACRO__CLR_RED, MACRO__CLR_RESET, __FILE__, __LINE__, #expr);    \
    assert(expr);} } while(0)

#define MACRO_ASSERT_FALSE(expr) do { if ((expr)) {                                                                           \
    fprintf(stderr,"%s[FAIL]%s %s:%d: expected false: %s\n", MACRO__CLR_RED, MACRO__CLR_RESET, __FILE__, __LINE__, #expr);    \
    assert(!(expr));} } while(0)

#define MACRO_ASSERT_EQ_INT(a,b)  do { int _A=(a),_B=(b); if (_A!=_B){                                                               \
    fprintf(stderr,"%s[FAIL]%s %s:%d: %s==%s  (%d!=%d)\n", MACRO__CLR_RED, MACRO__CLR_RESET, __FILE__, __LINE__, #a, #b, _A, _B);    \
    assert(_A==_B);} } while(0)

#define MACRO_ASSERT_EQ_SZ(a,b)  do { size_t _A=(a),_B=(b); if (_A!=_B){                                                               \
    fprintf(stderr,"%s[FAIL]%s %s:%d: %s==%s  (%zu!=%zu)\n", MACRO__CLR_RED, MACRO__CLR_RESET, __FILE__, __LINE__, #a, #b, _A, _B);    \
    assert(_A==_B);} } while(0)

/* Null-safe strcmp */
static inline int macro_streq(const char *a, const char *b) {
    if (!a || !b) return a==b;
    return strcmp(a,b)==0;
}

#define MACRO_ASSERT_STREQ(a,b) do { const char *_A=(a), *_B=(b); if (!macro_streq(_A,_B)) {          \
    fprintf(stderr,"%s[FAIL]%s %s:%d: strings differ\n  A: [%s]\n  B: [%s]\n",                        \
            MACRO__CLR_RED, MACRO__CLR_RESET, __FILE__, __LINE__, _A?_A:"(null)", _B?_B:"(null)");    \
    assert(0);} } while(0)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MACRO_TEST_H */
