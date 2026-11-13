#include "test.h"
#define XI_ARENA_IMPLEMENTATION
#define XI_STRING_IMPLEMENTATION
#include "xi_string.h"
#include <string.h>

void test_x_string_append_cstr(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Hello");
    xi_string_append_cstr(&str, " World");

    ASSERT_EQ(11, (int)str.length);
    ASSERT_TRUE(strcmp("Hello World", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_append(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str1 = xi_string_create(&arena, "A");
    Xi_String str2 = xi_string_create(&arena, "B");

    xi_string_append(&str1, &str2);

    ASSERT_EQ(2, (int)str1.length);
    ASSERT_TRUE(strcmp("AB", str1.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_append_char(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "A");
    xi_string_append_char(&str, 'B');
    xi_string_append_char(&str, 'C');

    ASSERT_EQ(3, (int)str.length);
    ASSERT_TRUE(strcmp("ABC", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_insert(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Hlo");
    xi_string_insert(&str, 1, "el");

    ASSERT_EQ(5, (int)str.length);
    ASSERT_TRUE(strcmp("Hello", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_remove(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Hello World");
    xi_string_remove(&str, 5, 6);

    ASSERT_EQ(5, (int)str.length);
    ASSERT_TRUE(strcmp("Hello", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_trim(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "  \tTrim Me  \n");
    xi_string_trim(&str);

    ASSERT_TRUE(strcmp("Trim Me", str.data) == 0);

    Xi_String empty = xi_string_create(&arena, "   ");
    xi_string_trim(&empty);
    ASSERT_EQ(0, (int)empty.length);

    xi_arena_release(&arena);
}

void test_x_string_to_cases(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str1 = xi_string_create(&arena, "AbCdEf");
    xi_string_to_lower(&str1);
    ASSERT_TRUE(strcmp("abcdef", str1.data) == 0);

    Xi_String str2 = xi_string_create(&arena, "AbCdEf");
    xi_string_to_upper(&str2);
    ASSERT_TRUE(strcmp("ABCDEF", str2.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_printf(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create_empty(&arena, 0);
    xi_string_printf(&str, "Hello, %s! You have %d messages.", "Alice", 3);
    ASSERT_TRUE(strcmp("Hello, Alice! You have 3 messages.", str.data) == 0);
    ASSERT_EQ(34, (int)str.length);

    xi_arena_release(&arena);
}

void test_x_string_printf_overwrites(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "old content");
    xi_string_printf(&str, "%d + %d = %d", 2, 3, 5);
    ASSERT_TRUE(strcmp("2 + 3 = 5", str.data) == 0);
    ASSERT_EQ(9, (int)str.length);

    xi_arena_release(&arena);
}

void test_x_string_printf_large(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create_empty(&arena, 0);
    xi_string_printf(&str, "%-10d %010d %+d % d", 42, 42, 42, 42);
    ASSERT_TRUE(strcmp("42         0000000042 +42  42", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_printf_self_reference(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "count: 0");
    xi_string_printf(&str, "%s (done)", xi_string_get_cstr(&str));
    ASSERT_TRUE(strcmp("count: 0 (done)", str.data) == 0);

    xi_arena_release(&arena);
}

void test_x_string_resize_and_shrink(void) {
    Xi_Arena arena = xi_arena_allocate(4096);
    Xi_String str = xi_string_create(&arena, "Hi");
    ASSERT_TRUE(xi_string_resize(&str, 64));
    ASSERT_EQ(64, (int)str.capacity);
    ASSERT_TRUE(strcmp("Hi", str.data) == 0);
    ASSERT_TRUE(xi_string_append_cstr(&str, " there"));
    ASSERT_TRUE(strcmp("Hi there", str.data) == 0);

    ASSERT_TRUE(xi_string_shrink(&str));
    ASSERT_EQ(8, (int)str.capacity);
    ASSERT_TRUE(strcmp("Hi there", str.data) == 0);

    xi_arena_release(&arena);
}

int main(void) {
    RUN_TEST(test_x_string_append_cstr);
    RUN_TEST(test_x_string_append);
    RUN_TEST(test_x_string_append_char);
    RUN_TEST(test_x_string_insert);
    RUN_TEST(test_x_string_remove);
    RUN_TEST(test_x_string_trim);
    RUN_TEST(test_x_string_to_cases);
    RUN_TEST(test_x_string_printf);
    RUN_TEST(test_x_string_printf_overwrites);
    RUN_TEST(test_x_string_printf_large);
    RUN_TEST(test_x_string_printf_self_reference);
    RUN_TEST(test_x_string_resize_and_shrink);

    printf("\nRan %d assertions: %d failed.\n", tests_run, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
