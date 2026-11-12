#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT_TRUE(expr)                                                      \
  do {                                                                         \
    tests_run++;                                                               \
    if (!(expr)) {                                                             \
      fprintf(stderr, "FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__);        \
      tests_failed++;                                                          \
    }                                                                          \
  } while (0)

#define ASSERT_EQ(expected, actual)                                            \
  do {                                                                         \
    tests_run++;                                                               \
    if ((expected) != (actual)) {                                              \
      fprintf(stderr, "FAIL: expected %d, got %d (%s:%d)\n", (int)(expected),  \
              (int)(actual), __FILE__, __LINE__);                              \
      tests_failed++;                                                          \
    }                                                                          \
  } while (0)

#define RUN_TEST(test_fn)                                                      \
  do {                                                                         \
    printf("Running %s...\n", #test_fn);                                       \
    test_fn();                                                                 \
  } while (0)

#endif
