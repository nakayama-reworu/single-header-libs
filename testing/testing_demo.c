#define TEST_FILE_NAME_ONLY
#define TEST_SUCCESS_STREAM stdout
#define TEST_FAILURE_STREAM stdout

#include "testing.h"

#include <string.h>

test_fact(one_plus_one_equals_two) {
    // arrange
    int a = 1, b = 1;

    // act
    int c = a + b;

    // assert
    test_assert(c == 2, "math does not work");
}

test_fact(always_false) {
    int a = 1, b = 2;

    test_assert(a == a, "expected %s=%d to be equal %s=%d", test_nameof(a), a, test_nameof(a), a);
    test_assert(a == b, "expected %s=%d to be equal %s=%d", test_nameof(a), a, test_nameof(b), b);
}

test_fact(strlen_returns_correct_length) {
    const char *s = "abc";
    const size_t expected = 3;

    const size_t length = strlen(s);

    test_assert(length == expected, "expected length to be %zu but was %zu", expected, length);
}

test_all_tests = {
        test_case(one_plus_one_equals_two),
        test_case(always_false),
        test_case(strlen_returns_correct_length),
};

test_run_all_tests()
