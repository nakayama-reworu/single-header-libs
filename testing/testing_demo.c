#define TEST_FILE_NAME_ONLY
#define TEST_FAILURE_STREAM stderr
#include "testing/testing.h"

#include <string.h>

Test_Fact(one_plus_one_equals_two) {
    // arrange
    int a = 1, b = 1;

    // act
    int c = a + b;

    // assert
    Test_Assert(c == 2, "math does not work");
}

Test_Fact(always_false) {
    Test_Assert(1 == 2, "expected %d to be equal %d", 1, 2);
}

Test_Fact(strlen_returns_correct_length) {
    const char* s = "abc";
    const size_t expected = 3;

    const size_t length = strlen(s);

    Test_Assert(length == expected, "expected length to be %zu but was %zu", expected, length);
}

Test_AllTests = {
        Test_Case(one_plus_one_equals_two),
        Test_Case(always_false),
        Test_Case(strlen_returns_correct_length),
};

Test_RunAllTests()
