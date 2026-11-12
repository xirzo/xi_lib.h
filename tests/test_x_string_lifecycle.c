#include "test.h"
#define XI_STRING_IMPLEMENTATION
#include "x_string.h"
#include <stdlib.h>
#include <string.h>

void test_x_string_getters(void) {
    Xi_String str = xi_string_create("Data");
    ASSERT_TRUE(strcmp("Data", xi_string_get_cstr(&str)) == 0);
    ASSERT_TRUE(xi_string_get_char(&str, 1) == 'a');
    ASSERT_TRUE(!xi_string_is_empty(&str));
    xi_string_free(&str);
    
    Xi_String empty = xi_string_create_empty(5);
    ASSERT_TRUE(xi_string_is_empty(&empty));
    xi_string_free(&empty);
}

void test_x_string_comparisons(void) {
    Xi_String s1 = xi_string_create("Apple");
    Xi_String s2 = xi_string_create("Apple");
    Xi_String s3 = xi_string_create("App");
    
    ASSERT_TRUE(xi_string_equals(&s1, &s2));
    ASSERT_TRUE(!xi_string_equals(&s1, &s3));
    
    ASSERT_TRUE(xi_string_starts_with(&s1, "App"));
    ASSERT_TRUE(!xi_string_starts_with(&s1, "Bpp"));
    
    ASSERT_TRUE(xi_string_ends_with(&s1, "ple"));
    ASSERT_TRUE(!xi_string_ends_with(&s1, "pl"));
    
    xi_string_free(&s1);
    xi_string_free(&s2);
    xi_string_free(&s3);
}

void test_x_string_finds(void) {
    Xi_String str = xi_string_create("The cat and the cat");
    
    ASSERT_EQ(4, xi_string_find(&str, "cat"));
    ASSERT_EQ(-1, xi_string_find(&str, "dog"));
    
    ASSERT_EQ(16, xi_string_find_last(&str, "cat"));
    
    xi_string_free(&str);
}

void test_x_string_substring(void) {
    Xi_String str = xi_string_create("Hello World");
    Xi_String sub = xi_string_substring(&str, 6, 5);
    
    ASSERT_EQ(5, (int)sub.length);
    ASSERT_TRUE(strcmp("World", sub.data) == 0);
    
    xi_string_free(&str);
    xi_string_free(&sub);
}

void test_x_string_split(void) {
    Xi_String str = xi_string_create("a,b,c");
    size_t count = 0;
    Xi_String *parts = xi_string_split(&str, ",", &count);
    
    ASSERT_TRUE(parts != NULL);
    ASSERT_EQ(3, (int)count);
    ASSERT_TRUE(strcmp("a", parts[0].data) == 0);
    ASSERT_TRUE(strcmp("b", parts[1].data) == 0);
    ASSERT_TRUE(strcmp("c", parts[2].data) == 0);
    
    for (size_t i = 0; i < count; i++) {
        xi_string_free(&parts[i]);
    }
    free(parts);
    xi_string_free(&str);
}

int main(void) {
    RUN_TEST(test_x_string_getters);
    RUN_TEST(test_x_string_comparisons);
    RUN_TEST(test_x_string_finds);
    RUN_TEST(test_x_string_substring);
    RUN_TEST(test_x_string_split);

    printf("\nRan %d assertions: %d failed.\n", tests_run, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
