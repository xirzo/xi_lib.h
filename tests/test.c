#include "test.h"
#define X_ARENA_IMPLEMENTATION
#include "x_arena.h"
#include <stddef.h>

void test_x_arena_push_exact_struct_size_returns_pointer(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(sizeof(Test));

  // Act
  Test *test = x_arena_push(&arena, sizeof(Test));
  test->x = 123;

  // Assert
  ASSERT_EQ(123, test->x);
  x_arena_release(&arena);
}

void test_x_arena_push_returns_zero_initialized(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(sizeof(Test));

  // Act
  Test *test = x_arena_push(&arena, sizeof(Test));

  // Assert
  ASSERT_TRUE(test->x == 0);
  x_arena_release(&arena);
}

void test_x_arena_push_not_enough_memory_returns_null(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(2);

  // Act
  Test *test = x_arena_push(&arena, sizeof(Test));

  // Assert
  ASSERT_TRUE(test == NULL);
  x_arena_release(&arena);
}

void test_x_arena_pop_bigger_size_does_not_overflow_stack_pointer(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(sizeof(Test));

  // Act
  x_arena_pop(&arena, sizeof(Test) * 2);

  // Assert
  ASSERT_TRUE(arena.stack_pointer == 0);
  x_arena_release(&arena);
}

void test_x_arena_release_zeros_data_pointer(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(sizeof(Test));

  // Act
  x_arena_release(&arena);

  // Assert
  ASSERT_TRUE(arena.data == NULL);
}

void test_x_arena_clear_does_not_free(void) {
  // Arrange
  typedef struct {
    int x;
  } Test;
  X_Arena arena = x_arena_allocate(sizeof(Test));

  // Act
  x_arena_clear(&arena);

  // Assert
  ASSERT_TRUE(arena.data != NULL);
  x_arena_release(&arena);
}

int main(void) {
  RUN_TEST(test_x_arena_push_returns_zero_initialized);
  RUN_TEST(test_x_arena_push_exact_struct_size_returns_pointer);
  RUN_TEST(test_x_arena_push_not_enough_memory_returns_null);
  RUN_TEST(test_x_arena_pop_bigger_size_does_not_overflow_stack_pointer);
  RUN_TEST(test_x_arena_release_zeros_data_pointer);
  RUN_TEST(test_x_arena_clear_does_not_free);

  printf("\nRan %d assertions: %d failed.\n", tests_run, tests_failed);
  return tests_failed > 0 ? 1 : 0;
}
