#include <check.h>
#include <stdlib.h>

#include "buf.h"

START_TEST(test_empty_buffer_init)
{
    int *v = NULL;
    ck_assert_uint_eq(buf_size(v), 0u);
    ck_assert_uint_eq(buf_capacity(v), 0u);
    buf_free(v);
    ck_assert_ptr_eq(v, NULL);
}
END_TEST

START_TEST(test_push_pop_int)
{
    int *v = NULL;

    for (int i = 0; i < 100; ++i) {
        buf_push(v, i);
        ck_assert_uint_eq(buf_size(v), (unsigned int)(i + 1));
        ck_assert(buf_capacity(v) >= buf_size(v));
        ck_assert_int_eq(v[i], i);
    }

    for (int i = 99; i >= 0; --i) {
        int x = buf_pop(v);
        ck_assert_int_eq(x, i);
        ck_assert_uint_eq(buf_size(v), (unsigned int)i);
    }

    buf_free(v);
    ck_assert_ptr_eq(v, NULL);
}
END_TEST

START_TEST(test_grow_and_trunc)
{
    double *v = NULL;

    /* Reserve capacity explicitly */
    v = buf_grow(v, 50);
    ck_assert_uint_ge(buf_capacity(v), 50u);
    ck_assert_uint_eq(buf_size(v), 0u);

    /* Push some values */
    for (int i = 0; i < 10; ++i) {
        buf_push(v, (double)i * 0.5);
    }
    ck_assert_uint_eq(buf_size(v), 10u);

    /* Truncate capacity down to current size */
    v = buf_trunc(v, 10);
    ck_assert_uint_ge(buf_capacity(v), 10u);
    ck_assert_uint_eq(buf_size(v), 10u);

    /* Truncate to smaller size, verify data preserved */
    v = buf_trunc(v, 5);
    ck_assert_uint_ge(buf_capacity(v), 5u);
    ck_assert_uint_eq(buf_size(v), 5u);
    for (int i = 0; i < 5; ++i) {
        ck_assert_double_eq_tol(v[i], (double)i * 0.5, 1e-12);
    }

    buf_free(v);
}
END_TEST

START_TEST(test_clear_keeps_capacity)
{
    char *v = NULL;

    for (int i = 0; i < 32; ++i) {
        buf_push(v, (char)('a' + (i % 26)));
    }
    size_t cap_before = buf_capacity(v);
    ck_assert_uint_eq(buf_size(v), 32u);

    buf_clear(v);
    ck_assert_uint_eq(buf_size(v), 0u);
    ck_assert_uint_eq(buf_capacity(v), cap_before);

    /* Reuse buffer after clear */
    for (int i = 0; i < 16; ++i) {
        buf_push(v, 'x');
    }
    ck_assert_uint_eq(buf_size(v), 16u);
    ck_assert_uint_ge(buf_capacity(v), cap_before);

    buf_free(v);
}
END_TEST

START_TEST(test_multiple_types_independent)
{
    int *ints = NULL;
    char *chars = NULL;

    for (int i = 0; i < 10; ++i) {
        buf_push(ints, i);
        buf_push(chars, (char)('a' + i));
    }

    ck_assert_uint_eq(buf_size(ints), 10u);
    ck_assert_uint_eq(buf_size(chars), 10u);

    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(ints[i], i);
        ck_assert_int_eq(chars[i], 'a' + i);
    }

    buf_free(ints);
    buf_free(chars);
}
END_TEST

static Suite *buf_suite(void)
{
    Suite *s = suite_create("growable_buf");

    TCase *tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_empty_buffer_init);
    tcase_add_test(tc_core, test_push_pop_int);
    tcase_add_test(tc_core, test_grow_and_trunc);
    tcase_add_test(tc_core, test_clear_keeps_capacity);
    tcase_add_test(tc_core, test_multiple_types_independent);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = buf_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_ENV);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
