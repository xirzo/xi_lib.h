#include "test.h"
#define XI_ARENA_IMPLEMENTATION
#include "xi_arena.h"
#define X_DYNAMIC_ARRAY_IMPLEMENTATION
#include "xi_dynamic_array.h"
#include <string.h>

static int compare_int(const void *a, const void *b) {
  int left  = *(const int *)a;
  int right = *(const int *)b;
  return (left > right) - (left < right);
}

void test_xi_da_init_push_and_get(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 2, int);

  // Act
  XI_DA_PUSH(&array, 10);
  XI_DA_PUSH(&array, 20);
  XI_DA_PUSH(&array, 30);  // triggers resize

  // Assert
  ASSERT_EQ(3, (int)array.size);
  ASSERT_EQ(10, XI_DA_GET(&array, int, 0));
  ASSERT_EQ(20, XI_DA_GET(&array, int, 1));
  ASSERT_EQ(30, XI_DA_GET(&array, int, 2));
  ASSERT_TRUE(xi_da_get(&array, 99) == NULL);

  xi_arena_release(&arena);
}

void test_xi_da_pop(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 4, int);
  XI_DA_PUSH(&array, 1);
  XI_DA_PUSH(&array, 2);

  // Act
  int popped = 0;
  enum Xi_DA_Result result = XI_DA_POP(&array, &popped);

  // Assert
  ASSERT_EQ(XI_DA_SUCCESS, (int)result);
  ASSERT_EQ(2, popped);
  ASSERT_EQ(1, (int)array.size);

  // Popping an empty array fails
  XI_DA_POP(&array, &popped);
  result = XI_DA_POP(&array, &popped);
  ASSERT_EQ(XI_DA_EMPTY, (int)result);

  xi_arena_release(&arena);
}

void test_xi_da_insert_replace_remove(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 4, int);
  XI_DA_PUSH(&array, 1);
  XI_DA_PUSH(&array, 3);

  // Act + Assert (insert)
  XI_DA_INSERT(&array, 1, 2);
  ASSERT_EQ(3, (int)array.size);
  ASSERT_EQ(2, XI_DA_GET(&array, int, 1));

  // Act + Assert (replace)
  XI_DA_REPLACE(&array, 2, 99);
  ASSERT_EQ(99, XI_DA_GET(&array, int, 2));

  // Act + Assert (remove)
  XI_DA_REMOVE(&array, 0);
  ASSERT_EQ(2, (int)array.size);
  ASSERT_EQ(2, XI_DA_GET(&array, int, 0));
  ASSERT_EQ(99, XI_DA_GET(&array, int, 1));

  // Out of bounds insert fails
  ASSERT_EQ(XI_DA_OUT_OF_BOUNDS, (int)xi_da_insert(&array, 99, &(int){0}));

  xi_arena_release(&arena);
}

void test_xi_da_clear_and_free(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 4, int);
  XI_DA_PUSH(&array, 1);

  // Act
  xi_da_clear(&array);
  xi_da_free(&array);

  // Assert
  ASSERT_EQ(0, (int)array.size);
  ASSERT_TRUE(array.data == NULL);
  ASSERT_TRUE(array.arena == NULL);

  xi_arena_release(&arena);
}

void test_xi_da_swap_sort_shrink(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 1, int);
  XI_DA_PUSH(&array, 3);
  XI_DA_PUSH(&array, 1);
  XI_DA_PUSH(&array, 2);

  // Act + Assert (swap)
  XI_DA_SWAP(&array, 0, 1);
  ASSERT_EQ(1, XI_DA_GET(&array, int, 0));
  ASSERT_EQ(3, XI_DA_GET(&array, int, 1));

  // Act + Assert (sort)
  XI_DA_SORT(&array, compare_int);
  ASSERT_EQ(1, XI_DA_GET(&array, int, 0));
  ASSERT_EQ(2, XI_DA_GET(&array, int, 1));
  ASSERT_EQ(3, XI_DA_GET(&array, int, 2));

  // Act + Assert (shrink to fit)
  XI_DA_SHRINK(&array);
  ASSERT_EQ(array.size, array.capacity);
  ASSERT_EQ(1, XI_DA_GET(&array, int, 0));

  // Out of bounds swap fails
  ASSERT_EQ(XI_DA_OUT_OF_BOUNDS, (int)xi_da_swap(&array, 0, 99));

  xi_arena_release(&arena);
}

void test_xi_da_foreach(void) {
  // Arrange
  Xi_Arena arena = xi_arena_allocate(1024);
  Xi_Dynamic_Array array;
  XI_DA_INIT(&array, &arena, 4, int);
  XI_DA_PUSH(&array, 1);
  XI_DA_PUSH(&array, 2);
  XI_DA_PUSH(&array, 3);

  // Act
  int sum = 0;
  XI_DA_FOREACH(&array, int, it) { sum += *it; }

  // Assert
  ASSERT_EQ(6, sum);

  xi_arena_release(&arena);
}

int main(void) {
  RUN_TEST(test_xi_da_init_push_and_get);
  RUN_TEST(test_xi_da_pop);
  RUN_TEST(test_xi_da_insert_replace_remove);
  RUN_TEST(test_xi_da_clear_and_free);
  RUN_TEST(test_xi_da_swap_sort_shrink);
  RUN_TEST(test_xi_da_foreach);

  printf("\nRan %d assertions: %d failed.\n", tests_run, tests_failed);
  return tests_failed > 0 ? 1 : 0;
}
