#include "test.h"
#define XI_ARENA_IMPLEMENTATION
#define XI_STRING_IMPLEMENTATION
#include "xi_string.h"
#include <string.h>

void test_x_string_getters(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Data");
    ASSERT_TRUE(strcmp("Data", xi_string_get_cstr(&str)) == 0);
    ASSERT_TRUE(xi_string_get_char(&str, 1) == 'a');
    ASSERT_TRUE(!xi_string_is_empty(&str));

    Xi_String empty = xi_string_create_empty(&arena, 5);
    ASSERT_TRUE(xi_string_is_empty(&empty));

    xi_arena_release(&arena);
}

void test_x_string_comparisons(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String s1 = xi_string_create(&arena, "Apple");
    Xi_String s2 = xi_string_create(&arena, "Apple");
    Xi_String s3 = xi_string_create(&arena, "App");

    ASSERT_TRUE(xi_string_equals(&s1, &s2));
    ASSERT_TRUE(!xi_string_equals(&s1, &s3));

    ASSERT_TRUE(xi_string_starts_with(&s1, "App"));
    ASSERT_TRUE(!xi_string_starts_with(&s1, "Bpp"));

    ASSERT_TRUE(xi_string_ends_with(&s1, "ple"));
    ASSERT_TRUE(!xi_string_ends_with(&s1, "pl"));

    xi_arena_release(&arena);
}

void test_x_string_finds(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "The cat and the cat");

    ASSERT_EQ(4, xi_string_find(&str, "cat"));
    ASSERT_EQ(-1, xi_string_find(&str, "dog"));

    ASSERT_EQ(16, xi_string_find_last(&str, "cat"));

    xi_arena_release(&arena);
}

void test_x_string_substring(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Hello World");
    Xi_String sub = xi_string_substring(&str, 6, 5);

    ASSERT_EQ(5, (int)sub.length);
    ASSERT_TRUE(strcmp("World", sub.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_split(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "a,b,c");
    size_t count = 0;
    Xi_String *parts = xi_string_split(&str, ",", &count);

    ASSERT_TRUE(parts != NULL);
    ASSERT_EQ(3, (int)count);
    ASSERT_TRUE(strcmp("a", parts[0].data) == 0);
    ASSERT_TRUE(strcmp("b", parts[1].data) == 0);
    ASSERT_TRUE(strcmp("c", parts[2].data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_split_many(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15");
    size_t count = 0;
    Xi_String *parts = xi_string_split(&str, ",", &count);

    ASSERT_TRUE(parts != NULL);
    ASSERT_EQ(15, (int)count);
    ASSERT_TRUE(strcmp("1", parts[0].data) == 0);
    ASSERT_TRUE(strcmp("15", parts[14].data) == 0);

    xi_arena_release(&arena);
}

int main(void) {
    RUN_TEST(test_x_string_getters);
    RUN_TEST(test_x_string_comparisons);
    RUN_TEST(test_x_string_finds);
    RUN_TEST(test_x_string_substring);
    RUN_TEST(test_x_string_split);
    RUN_TEST(test_x_string_split_many);

    printf("\nRan %d assertions: %d failed.\n", tests_run, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
