#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

/*
    Assumptions about your vector API.

    Change these if your vector functions have different names.

    Expected:
        size_t vector_size(vector_t *v);
        void *vector_get(vector_t *v, size_t index);
        void vector_free(vector_t *v);
*/

#define VEC_SIZE(v) vector_size(&(v))
#define VEC_GET(v, i) vector_get(&(v), (i))
#define VEC_GET_CSTR(v, i) (*(char **)vector_get(&(v), (i)))
#define VEC_GET_HSTR(v, i) ((string_t *)vector_get(&(v), (i)))

static int tests_run = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)

#define RUN_TEST(name) do {                         \
    printf("[ RUN      ] %s\n", #name);             \
    name();                                         \
    printf("[     DONE ] %s\n", #name);             \
} while (0)

#define ASSERT_TRUE(expr) do {                                      \
    tests_run++;                                                    \
    if (!(expr)) {                                                  \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: ASSERT_TRUE(%s)\n",             \
               __FILE__, __LINE__, #expr);                         \
    }                                                               \
} while (0)

#define ASSERT_INT_EQ(expected, actual) do {                        \
    tests_run++;                                                    \
    int e__ = (expected);                                           \
    int a__ = (actual);                                             \
    if (e__ != a__) {                                               \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected %d, got %d\n",         \
               __FILE__, __LINE__, e__, a__);                      \
    }                                                               \
} while (0)

#define ASSERT_SIZE_EQ(expected, actual) do {                       \
    tests_run++;                                                    \
    size_t e__ = (size_t)(expected);                                \
    size_t a__ = (size_t)(actual);                                  \
    if (e__ != a__) {                                               \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected %u, got %u\n",       \
               __FILE__, __LINE__, e__, a__);                      \
    }                                                               \
} while (0)

#define ASSERT_CMP_ZERO(actual) do {                                \
    tests_run++;                                                    \
    int a__ = (actual);                                             \
    if (a__ != 0) {                                                 \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected cmp == 0, got %d\n",   \
               __FILE__, __LINE__, a__);                           \
    }                                                               \
} while (0)

#define ASSERT_CMP_NEG(actual) do {                                 \
    tests_run++;                                                    \
    int a__ = (actual);                                             \
    if (!(a__ < 0)) {                                               \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected cmp < 0, got %d\n",    \
               __FILE__, __LINE__, a__);                           \
    }                                                               \
} while (0)

#define ASSERT_CMP_POS(actual) do {                                 \
    tests_run++;                                                    \
    int a__ = (actual);                                             \
    if (!(a__ > 0)) {                                               \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected cmp > 0, got %d\n",    \
               __FILE__, __LINE__, a__);                           \
    }                                                               \
} while (0)

static size_t test_cstr_len(const char *s) {
    size_t len = 0;

    if (s == NULL) {
        return 0;
    }

    while (s[len] != '\0') {
        len++;
    }

    return len;
}

static void print_cstr(const char *s) {
    if (s == NULL) {
        printf("(null)");
        return;
    }

    while (*s != '\0') {
        putchar(*s);
        s++;
    }
}

static void print_hstr(const string_t *s) {
    if (s == NULL) {
        printf("(null string_t)");
        return;
    }

    if (s->str == NULL && s->len > 0) {
        printf("(null buffer with len %u)", s->len);
        return;
    }

    for (size_t i = 0; i < s->len; i++) {
        putchar(s->str[i]);
    }
}

static bool test_cstr_equals(const char *a, const char *b) {
    size_t i = 0;

    if (a == NULL || b == NULL) {
        return a == b;
    }

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }

        i++;
    }

    return a[i] == b[i];
}

static bool test_hstr_equals_cstr(const string_t *s, const char *expected) {
    if (s == NULL || expected == NULL) {
        return false;
    }

    size_t expected_len = test_cstr_len(expected);

    if (s->len != expected_len) {
        return false;
    }

    if (s->len == 0) {
        return true;
    }

    if (s->str == NULL) {
        return false;
    }

    for (size_t i = 0; i < s->len; i++) {
        if (s->str[i] != expected[i]) {
            return false;
        }
    }

    return true;
}

#define ASSERT_CSTR_EQ(expected, actual) do {                       \
    tests_run++;                                                    \
    const char *e__ = (expected);                                   \
    const char *a__ = (actual);                                     \
    if (!test_cstr_equals(e__, a__)) {                              \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected C string \"",          \
               __FILE__, __LINE__);                                \
        print_cstr(e__);                                            \
        printf("\", got \"");                                      \
        print_cstr(a__);                                            \
        printf("\"\n");                                            \
    }                                                               \
} while (0)

#define ASSERT_HSTR_EQ(expected, actual_ptr) do {                   \
    tests_run++;                                                    \
    const char *e__ = (expected);                                   \
    const string_t *s__ = (actual_ptr);                             \
    if (!test_hstr_equals_cstr(s__, e__)) {                         \
        tests_failed++;                                             \
        printf("[  FAILED  ] %s:%d: expected heap string \"",       \
               __FILE__, __LINE__);                                \
        print_cstr(e__);                                            \
        printf("\" len=%u, got \"", test_cstr_len(e__));           \
        print_hstr(s__);                                            \
        printf("\" len=%u\n", s__ ? s__->len : 0);                \
    }                                                               \
} while (0)

static void free_cstr_token_vector(vector_t *tokens) {
    size_t n = vector_size(tokens);

    for (size_t i = 0; i < n; i++) {
        char *tok = *(char **)vector_get(tokens, i);
        free(tok);
    }

    vector_free(tokens);
}

static void free_hstr_token_vector(vector_t *tokens) {
    size_t n = vector_size(tokens);

    for (size_t i = 0; i < n; i++) {
        string_t *tok = (string_t *)vector_get(tokens, i);
        string_free(tok);
    }

    vector_free(tokens);
}

/* ------------------------------------------------------------
   string_t helper tests
------------------------------------------------------------ */

TEST(test_string_create) {
    string_t s = string_create();

    ASSERT_SIZE_EQ(0, s.len);
    ASSERT_SIZE_EQ(10, s.capacity);
    ASSERT_TRUE(s.str != NULL);

    string_free(&s);
}

TEST(test_string_literal_normal) {
    string_t s = string_literal("hello");

    ASSERT_HSTR_EQ("hello", &s);
    ASSERT_SIZE_EQ(5, s.len);
    ASSERT_TRUE(s.capacity >= 5);

    string_free(&s);
}

TEST(test_string_literal_empty) {
    string_t s = string_literal("");

    ASSERT_HSTR_EQ("", &s);
    ASSERT_SIZE_EQ(0, s.len);

    string_free(&s);
}

TEST(test_string_clone_independent_copy) {
    string_t a = string_literal("hello");
    string_t b = string_clone(&a);

    ASSERT_HSTR_EQ("hello", &a);
    ASSERT_HSTR_EQ("hello", &b);

    h_strcat_c(&b, '!');

    ASSERT_HSTR_EQ("hello", &a);
    ASSERT_HSTR_EQ("hello!", &b);

    string_free(&a);
    string_free(&b);
}

TEST(test_string_clear) {
    string_t s = string_literal("hello");

    string_clear(&s);

    ASSERT_HSTR_EQ("", &s);
    ASSERT_SIZE_EQ(0, s.len);

    string_free(&s);
}

TEST(test_string_to_literal) {
    string_t s = string_literal("hello");

    char *lit = string_to_literal(&s);

    ASSERT_CSTR_EQ("hello", lit);

    free(lit);
    string_free(&s);
}

/*
    Do not enable this until string_resize is fixed.

    Your current string_resize does:

        realloc(str, sizeof(char) * capacity)

    but it should realloc str->str, not str.

    TEST(test_string_resize_grow) {
        string_t s = string_literal("hello");
        int ret = string_resize(&s, 100);

        ASSERT_TRUE(ret >= 0);
        ASSERT_TRUE(s.capacity >= 100);
        ASSERT_HSTR_EQ("hello", &s);

        string_free(&s);
    }
*/

/* ------------------------------------------------------------
   strlen tests
------------------------------------------------------------ */

TEST(test_c_strlen_empty) {
    ASSERT_INT_EQ(0, c_strlen(""));
}

TEST(test_c_strlen_normal) {
    ASSERT_INT_EQ(5, c_strlen("hello"));
    ASSERT_INT_EQ(11, c_strlen("hello world"));
}

TEST(test_h_strlen_empty) {
    string_t s = string_literal("");

    ASSERT_INT_EQ(0, h_strlen(&s));

    string_free(&s);
}

TEST(test_h_strlen_normal) {
    string_t s = string_literal("hello");

    ASSERT_INT_EQ(5, h_strlen(&s));

    string_free(&s);
}

/* ------------------------------------------------------------
   strcat tests
------------------------------------------------------------ */

TEST(test_c_strcat_cstr_normal) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");
    c_strcat_cstr(buf, " world");

    ASSERT_CSTR_EQ("hello world", buf);
}

TEST(test_c_strcat_cstr_empty_src) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");
    c_strcat_cstr(buf, "");

    ASSERT_CSTR_EQ("hello", buf);
}

TEST(test_c_strcat_c_normal) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");
    c_strcat_c(buf, '!');

    ASSERT_CSTR_EQ("hello!", buf);
}

TEST(test_c_strcat_hstr_normal) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");

    string_t src = string_literal(" world");

    c_strcat_hstr(buf, &src);

    ASSERT_CSTR_EQ("hello world", buf);

    string_free(&src);
}

TEST(test_h_strcat_c_normal) {
    string_t s = string_literal("hello");

    h_strcat_c(&s, '!');

    ASSERT_HSTR_EQ("hello!", &s);
    ASSERT_SIZE_EQ(6, s.len);

    string_free(&s);
}

TEST(test_h_strcat_cstr_normal) {
    string_t s = string_literal("hello");

    h_strcat_cstr(&s, " world");

    ASSERT_HSTR_EQ("hello world", &s);
    ASSERT_SIZE_EQ(11, s.len);

    string_free(&s);
}

TEST(test_h_strcat_hstr_normal) {
    string_t a = string_literal("hello");
    string_t b = string_literal(" world");

    h_strcat_hstr(&a, &b);

    ASSERT_HSTR_EQ("hello world", &a);
    ASSERT_SIZE_EQ(11, a.len);

    string_free(&a);
    string_free(&b);
}

TEST(test_h_strcat_empty_src) {
    string_t a = string_literal("hello");

    h_strcat_cstr(&a, "");

    ASSERT_HSTR_EQ("hello", &a);
    ASSERT_SIZE_EQ(5, a.len);

    string_free(&a);
}

/* ------------------------------------------------------------
   strcmp tests

   Some of these intentionally catch current bugs:
   your current strcmp-style functions return 0 when one string
   is a prefix of the other.
------------------------------------------------------------ */

TEST(test_c_strcmp_equal) {
    ASSERT_CMP_ZERO(c_strcmp_cstr("abc", "abc"));
}

TEST(test_c_strcmp_less_diff_char) {
    ASSERT_CMP_NEG(c_strcmp_cstr("abc", "abd"));
}

TEST(test_c_strcmp_greater_diff_char) {
    ASSERT_CMP_POS(c_strcmp_cstr("abd", "abc"));
}

TEST(test_c_strcmp_prefix_shorter_should_be_negative) {
    ASSERT_CMP_NEG(c_strcmp_cstr("abc", "abcd"));
}

TEST(test_c_strcmp_prefix_longer_should_be_positive) {
    ASSERT_CMP_POS(c_strcmp_cstr("abcd", "abc"));
}

TEST(test_c_strcmp_empty) {
    ASSERT_CMP_ZERO(c_strcmp_cstr("", ""));
    ASSERT_CMP_NEG(c_strcmp_cstr("", "a"));
    ASSERT_CMP_POS(c_strcmp_cstr("a", ""));
}

TEST(test_c_strcmp_hstr_equal) {
    string_t s = string_literal("abc");

    ASSERT_CMP_ZERO(c_strcmp_hstr("abc", &s));

    string_free(&s);
}

TEST(test_c_strcmp_hstr_prefix_shorter_should_be_negative) {
    string_t s = string_literal("abcd");

    ASSERT_CMP_NEG(c_strcmp_hstr("abc", &s));

    string_free(&s);
}

TEST(test_h_strcmp_hstr_equal) {
    string_t a = string_literal("abc");
    string_t b = string_literal("abc");

    ASSERT_CMP_ZERO(h_strcmp_hstr(&a, &b));

    string_free(&a);
    string_free(&b);
}

TEST(test_h_strcmp_hstr_less_diff_char) {
    string_t a = string_literal("abc");
    string_t b = string_literal("abd");

    ASSERT_CMP_NEG(h_strcmp_hstr(&a, &b));

    string_free(&a);
    string_free(&b);
}

TEST(test_h_strcmp_hstr_prefix_shorter_should_be_negative) {
    string_t a = string_literal("abc");
    string_t b = string_literal("abcd");

    ASSERT_CMP_NEG(h_strcmp_hstr(&a, &b));

    string_free(&a);
    string_free(&b);
}

TEST(test_h_strcmp_cstr_equal) {
    string_t a = string_literal("abc");

    ASSERT_CMP_ZERO(h_strcmp_cstr(&a, "abc"));

    string_free(&a);
}

TEST(test_h_strcmp_cstr_prefix_longer_should_be_positive) {
    string_t a = string_literal("abcd");

    ASSERT_CMP_POS(h_strcmp_cstr(&a, "abc"));

    string_free(&a);
}

/* ------------------------------------------------------------
   strcpy tests
------------------------------------------------------------ */

TEST(test_c_strcpy_cstr_normal) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");

    ASSERT_CSTR_EQ("hello", buf);
}

TEST(test_c_strcpy_hstr_normal) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    string_t src = string_literal("hello");

    c_strcpy_hstr(buf, &src);

    ASSERT_CSTR_EQ("hello", buf);

    string_free(&src);
}

TEST(test_h_strcpy_cstr_normal) {
    string_t dst = string_literal("old");

    h_strcpy_cstr(&dst, "new value");

    ASSERT_HSTR_EQ("new value", &dst);
    ASSERT_SIZE_EQ(9, dst.len);

    string_free(&dst);
}

TEST(test_h_strcpy_hstr_normal) {
    string_t dst = string_literal("old");
    string_t src = string_literal("new value");

    h_strcpy_hstr(&dst, &src);

    ASSERT_HSTR_EQ("new value", &dst);
    ASSERT_SIZE_EQ(9, dst.len);

    string_free(&dst);
    string_free(&src);
}

TEST(test_h_strcpy_cstr_empty) {
    string_t dst = string_literal("old");

    h_strcpy_cstr(&dst, "");

    ASSERT_HSTR_EQ("", &dst);
    ASSERT_SIZE_EQ(0, dst.len);

    string_free(&dst);
}

/* ------------------------------------------------------------
   strncmp tests

   Prefix-length cases catch current bugs.
------------------------------------------------------------ */

TEST(test_c_strncmp_equal_prefix) {
    ASSERT_CMP_ZERO(c_strncmp_cstr("abcdef", "abcxyz", 3));
}

TEST(test_c_strncmp_diff_before_n) {
    ASSERT_CMP_NEG(c_strncmp_cstr("abc", "abd", 3));
    ASSERT_CMP_POS(c_strncmp_cstr("abd", "abc", 3));
}

TEST(test_c_strncmp_zero) {
    ASSERT_CMP_ZERO(c_strncmp_cstr("abc", "xyz", 0));
}

TEST(test_c_strncmp_prefix_shorter_large_n_should_be_negative) {
    ASSERT_CMP_NEG(c_strncmp_cstr("abc", "abcd", 100));
}

TEST(test_c_strncmp_prefix_longer_large_n_should_be_positive) {
    ASSERT_CMP_POS(c_strncmp_cstr("abcd", "abc", 100));
}

TEST(test_c_strncmp_hstr_equal_prefix) {
    string_t b = string_literal("abcxyz");

    ASSERT_CMP_ZERO(c_strncmp_hstr("abcdef", &b, 3));

    string_free(&b);
}

TEST(test_h_strncmp_hstr_equal_prefix) {
    string_t a = string_literal("abcdef");
    string_t b = string_literal("abcxyz");

    ASSERT_CMP_ZERO(h_strncmp_hstr(&a, &b, 3));
    ASSERT_CMP_NEG(h_strncmp_hstr(&a, &b, 4));

    string_free(&a);
    string_free(&b);
}

TEST(test_h_strncmp_cstr_equal_prefix) {
    string_t a = string_literal("abcdef");

    ASSERT_CMP_ZERO(h_strncmp_cstr(&a, "abcxyz", 3));
    ASSERT_CMP_NEG(h_strncmp_cstr(&a, "abcxyz", 4));

    string_free(&a);
}

/* ------------------------------------------------------------
   strncpy tests

   Some h_strncpy tests intentionally expose current bugs:
   h_strncpy_cstr loops using dst->len instead of source length,
   and h_strncpy_hstr adds to dst->len instead of setting it.
------------------------------------------------------------ */

TEST(test_c_strncpy_cstr_partial) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strncpy_cstr(buf, "hello", 3);

    ASSERT_CSTR_EQ("hel", buf);
}

TEST(test_c_strncpy_hstr_partial) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    string_t src = string_literal("hello");

    c_strncpy_hstr(buf, &src, 4);

    ASSERT_CSTR_EQ("hell", buf);

    string_free(&src);
}

TEST(test_h_strncpy_hstr_should_replace_with_prefix) {
    string_t dst = string_literal("old");
    string_t src = string_literal("hello world");

    h_strncpy_hstr(&dst, &src, 5);

    ASSERT_HSTR_EQ("hello", &dst);
    ASSERT_SIZE_EQ(5, dst.len);

    string_free(&dst);
    string_free(&src);
}

TEST(test_h_strncpy_cstr_should_replace_with_prefix) {
    string_t dst = string_literal("old");

    h_strncpy_cstr(&dst, "hello world", 5);

    ASSERT_HSTR_EQ("hello", &dst);
    ASSERT_SIZE_EQ(5, dst.len);

    string_free(&dst);
}

/* ------------------------------------------------------------
   strncat tests
------------------------------------------------------------ */

TEST(test_c_strncat_cstr_partial) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");
    c_strncat_cstr(buf, " world", 3);

    ASSERT_CSTR_EQ("hello wo", buf);
}

TEST(test_c_strncat_cstr_zero) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");
    c_strncat_cstr(buf, " world", 0);

    ASSERT_CSTR_EQ("hello", buf);
}

TEST(test_c_strncat_hstr_partial) {
    char buf[64];

    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
    }

    c_strcpy_cstr(buf, "hello");

    string_t src = string_literal(" world");

    c_strncat_hstr(buf, &src, 3);

    ASSERT_CSTR_EQ("hello wo", buf);

    string_free(&src);
}

TEST(test_h_strncat_cstr_partial) {
    string_t dst = string_literal("hello");

    h_strncat_cstr(&dst, " world", 3);

    ASSERT_HSTR_EQ("hello wo", &dst);
    ASSERT_SIZE_EQ(8, dst.len);

    string_free(&dst);
}

TEST(test_h_strncat_hstr_partial) {
    string_t dst = string_literal("hello");
    string_t src = string_literal(" world");

    h_strncat_hstr(&dst, &src, 3);

    ASSERT_HSTR_EQ("hello wo", &dst);
    ASSERT_SIZE_EQ(8, dst.len);

    string_free(&dst);
    string_free(&src);
}

TEST(test_h_strncat_zero) {
    string_t dst = string_literal("hello");

    h_strncat_cstr(&dst, " world", 0);

    ASSERT_HSTR_EQ("hello", &dst);
    ASSERT_SIZE_EQ(5, dst.len);

    string_free(&dst);
}

/* ------------------------------------------------------------
   strchr / strrchr / strstr tests
------------------------------------------------------------ */

TEST(test_c_strchar_found) {
    char s[] = "hello";

    char *p = c_strchar(s, 'e');

    ASSERT_TRUE(p != NULL);
    ASSERT_INT_EQ(1, (int)(p - s));
    ASSERT_INT_EQ('e', *p);
}

TEST(test_c_strchar_not_found) {
    char s[] = "hello";

    char *p = c_strchar(s, 'z');

    ASSERT_TRUE(p == NULL);
}

TEST(test_c_strchar_null_terminator) {
    char s[] = "hello";

    char *p = c_strchar(s, '\0');

    ASSERT_TRUE(p != NULL);
    ASSERT_INT_EQ(5, (int)(p - s));
}

TEST(test_c_strrchar_found) {
    char s[] = "hello";

    char *p = c_strrchar(s, 'l');

    ASSERT_TRUE(p != NULL);
    ASSERT_INT_EQ(3, (int)(p - s));
    ASSERT_INT_EQ('l', *p);
}

TEST(test_c_strrchar_not_found) {
    char s[] = "hello";

    char *p = c_strrchar(s, 'z');

    ASSERT_TRUE(p == NULL);
}

TEST(test_c_strrchar_null_terminator_should_find_end) {
    char s[] = "hello";

    char *p = c_strrchar(s, '\0');

    ASSERT_TRUE(p != NULL);
    ASSERT_INT_EQ(5, (int)(p - s));
}

TEST(test_c_strstr_found) {
    char s[] = "hello world";

    char *p = c_strstr(s, "world");

    ASSERT_TRUE(p != NULL);
    ASSERT_CSTR_EQ("world", p);
}

TEST(test_c_strstr_not_found) {
    char s[] = "hello world";

    char *p = c_strstr(s, "batman");

    ASSERT_TRUE(p == NULL);
}

TEST(test_c_strstr_empty_needle_should_return_start) {
    char s[] = "hello";

    char *p = c_strstr(s, "");

    ASSERT_TRUE(p == s);
}

/* ------------------------------------------------------------
   strtok tests

   c_strtok returns vector<char *>.
   h_strtok returns vector<string_t>.

   These expected values are for split-style behavior that keeps
   empty tokens for c_strtok because your implementation currently
   pushes a token every time it sees a delimiter.
------------------------------------------------------------ */

TEST(test_c_strtok_basic) {
    vector_t toks = c_strtok("a,b,c", ',');

    ASSERT_SIZE_EQ(3, VEC_SIZE(toks));
    ASSERT_CSTR_EQ("a", VEC_GET_CSTR(toks, 0));
    ASSERT_CSTR_EQ("b", VEC_GET_CSTR(toks, 1));
    ASSERT_CSTR_EQ("c", VEC_GET_CSTR(toks, 2));

    free_cstr_token_vector(&toks);
}

TEST(test_c_strtok_repeated_delimiters_keeps_empty_tokens) {
    vector_t toks = c_strtok("a,,b", ',');

    ASSERT_SIZE_EQ(3, VEC_SIZE(toks));
    ASSERT_CSTR_EQ("a", VEC_GET_CSTR(toks, 0));
    ASSERT_CSTR_EQ("b", VEC_GET_CSTR(toks, 2));

    free_cstr_token_vector(&toks);
}

TEST(test_c_strtok_leading_delimiter_keeps_empty_first) {
    vector_t toks = c_strtok(",a,b", ',');

    ASSERT_SIZE_EQ(3, VEC_SIZE(toks));
    ASSERT_CSTR_EQ("a", VEC_GET_CSTR(toks, 1));
    ASSERT_CSTR_EQ("b", VEC_GET_CSTR(toks, 2));

    free_cstr_token_vector(&toks);
}

TEST(test_c_strtok_trailing_delimiter_drops_final_empty) {
    vector_t toks = c_strtok("a,b,", ',');

    ASSERT_SIZE_EQ(2, VEC_SIZE(toks));
    ASSERT_CSTR_EQ("a", VEC_GET_CSTR(toks, 0));
    ASSERT_CSTR_EQ("b", VEC_GET_CSTR(toks, 1));

    free_cstr_token_vector(&toks);
}

TEST(test_h_strtok_basic) {
    string_t s = string_literal("a,b,c");

    vector_t toks = h_strtok(&s, ',');

    ASSERT_SIZE_EQ(3, VEC_SIZE(toks));
    ASSERT_HSTR_EQ("a", VEC_GET_HSTR(toks, 0));
    ASSERT_HSTR_EQ("b", VEC_GET_HSTR(toks, 1));
    ASSERT_HSTR_EQ("c", VEC_GET_HSTR(toks, 2));

    free_hstr_token_vector(&toks);
    string_free(&s);
}

TEST(test_h_strtok_repeated_delimiters_should_skip_empty) {
    string_t s = string_literal("a,,b");

    vector_t toks = h_strtok(&s, ',');

    ASSERT_SIZE_EQ(2, VEC_SIZE(toks));
    ASSERT_HSTR_EQ("a", VEC_GET_HSTR(toks, 0));
    ASSERT_HSTR_EQ("b", VEC_GET_HSTR(toks, 1));

    free_hstr_token_vector(&toks);
    string_free(&s);
}

TEST(test_h_strtok_empty_string_should_return_zero_tokens) {
    string_t s = string_literal("");

    vector_t toks = h_strtok(&s, ',');

    ASSERT_SIZE_EQ(0, VEC_SIZE(toks));

    free_hstr_token_vector(&toks);
    string_free(&s);
}

/* ------------------------------------------------------------
   Main
------------------------------------------------------------ */

int main(void) {
    RUN_TEST(test_string_create);
    RUN_TEST(test_string_literal_normal);
    RUN_TEST(test_string_literal_empty);
    RUN_TEST(test_string_clone_independent_copy);
    RUN_TEST(test_string_clear);
    RUN_TEST(test_string_to_literal);

    RUN_TEST(test_c_strlen_empty);
    RUN_TEST(test_c_strlen_normal);
    RUN_TEST(test_h_strlen_empty);
    RUN_TEST(test_h_strlen_normal);

    RUN_TEST(test_c_strcat_cstr_normal);
    RUN_TEST(test_c_strcat_cstr_empty_src);
    RUN_TEST(test_c_strcat_c_normal);
    RUN_TEST(test_c_strcat_hstr_normal);
    RUN_TEST(test_h_strcat_c_normal);
    RUN_TEST(test_h_strcat_cstr_normal);
    RUN_TEST(test_h_strcat_hstr_normal);
    RUN_TEST(test_h_strcat_empty_src);

    RUN_TEST(test_c_strcmp_equal);
    RUN_TEST(test_c_strcmp_less_diff_char);
    RUN_TEST(test_c_strcmp_greater_diff_char);
    RUN_TEST(test_c_strcmp_prefix_shorter_should_be_negative);
    RUN_TEST(test_c_strcmp_prefix_longer_should_be_positive);
    RUN_TEST(test_c_strcmp_empty);
    RUN_TEST(test_c_strcmp_hstr_equal);
    RUN_TEST(test_c_strcmp_hstr_prefix_shorter_should_be_negative);
    RUN_TEST(test_h_strcmp_hstr_equal);
    RUN_TEST(test_h_strcmp_hstr_less_diff_char);
    RUN_TEST(test_h_strcmp_hstr_prefix_shorter_should_be_negative);
    RUN_TEST(test_h_strcmp_cstr_equal);
    RUN_TEST(test_h_strcmp_cstr_prefix_longer_should_be_positive);

    RUN_TEST(test_c_strcpy_cstr_normal);
    RUN_TEST(test_c_strcpy_hstr_normal);
    RUN_TEST(test_h_strcpy_cstr_normal);
    RUN_TEST(test_h_strcpy_hstr_normal);
    RUN_TEST(test_h_strcpy_cstr_empty);

    RUN_TEST(test_c_strncmp_equal_prefix);
    RUN_TEST(test_c_strncmp_diff_before_n);
    RUN_TEST(test_c_strncmp_zero);
    RUN_TEST(test_c_strncmp_prefix_shorter_large_n_should_be_negative);
    RUN_TEST(test_c_strncmp_prefix_longer_large_n_should_be_positive);
    RUN_TEST(test_c_strncmp_hstr_equal_prefix);
    RUN_TEST(test_h_strncmp_hstr_equal_prefix);
    RUN_TEST(test_h_strncmp_cstr_equal_prefix);

    RUN_TEST(test_c_strncpy_cstr_partial);
    RUN_TEST(test_c_strncpy_hstr_partial);
    RUN_TEST(test_h_strncpy_hstr_should_replace_with_prefix);
    RUN_TEST(test_h_strncpy_cstr_should_replace_with_prefix);

    RUN_TEST(test_c_strncat_cstr_partial);
    RUN_TEST(test_c_strncat_cstr_zero);
    RUN_TEST(test_c_strncat_hstr_partial);
    RUN_TEST(test_h_strncat_cstr_partial);
    RUN_TEST(test_h_strncat_hstr_partial);
    RUN_TEST(test_h_strncat_zero);

    RUN_TEST(test_c_strchar_found);
    RUN_TEST(test_c_strchar_not_found);
    RUN_TEST(test_c_strchar_null_terminator);
    RUN_TEST(test_c_strrchar_found);
    RUN_TEST(test_c_strrchar_not_found);
    RUN_TEST(test_c_strrchar_null_terminator_should_find_end);
    RUN_TEST(test_c_strstr_found);
    RUN_TEST(test_c_strstr_not_found);
    RUN_TEST(test_c_strstr_empty_needle_should_return_start);

    RUN_TEST(test_c_strtok_basic);
    RUN_TEST(test_c_strtok_repeated_delimiters_keeps_empty_tokens);
    RUN_TEST(test_c_strtok_leading_delimiter_keeps_empty_first);
    RUN_TEST(test_c_strtok_trailing_delimiter_drops_final_empty);
    RUN_TEST(test_h_strtok_basic);
    RUN_TEST(test_h_strtok_repeated_delimiters_should_skip_empty);
    RUN_TEST(test_h_strtok_empty_string_should_return_zero_tokens);

    printf("\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests failed: %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("All tests passed.\n");
        return 0;
    }

    return 1;
}
