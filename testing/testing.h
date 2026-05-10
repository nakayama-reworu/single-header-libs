#ifndef PLAYGROUND_TESTING_H
#define PLAYGROUND_TESTING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TEST_SUCCESS_STREAM
#define TEST_SUCCESS_STREAM stdout
#endif

#ifndef TEST_FAILURE_STREAM
#define TEST_FAILURE_STREAM stdout
#endif

#ifdef TEST_FILE_NAME_ONLY
#define TEST_FILE __FILE_NAME__
#else
#define TEST_FILE __FILE__
#endif

typedef void (*Test)(size_t, const char*, bool*);

typedef struct {
    const Test test_body;
    const char *const test_name;
} TestCase;

#define test_nameof(it) ({(void) it; #it;})

#define test_fact(name) void name(size_t test_no, const char* test_name, bool* ok)

#define test_assert(assertion, format, ...)                                         \
do {                                                                                \
    const bool success = (assertion);                                               \
    if (false == success) {                                                         \
        *ok = false;                                                                \
        fprintf(                                                                    \
            TEST_FAILURE_STREAM,                                                    \
            "[TEST %02zu] FAILED %s\n%s:%d: Assertion '%s' failed, " format "\n",   \
            test_no, test_name,                                                     \
            TEST_FILE, __LINE__,                                                    \
            #assertion, ##__VA_ARGS__                                               \
        );                                                                          \
        fflush(TEST_FAILURE_STREAM);                                                \
        return;                                                                     \
    }                                                                               \
} while (0)

#define test_case(name) (TestCase) { .test_name = #name, .test_body = name }

#define test_all_tests static const TestCase tests[]

#define test_run_all_tests()                                    \
int main(void) {                                                \
    const size_t tests_count = sizeof(tests) / sizeof(*tests);  \
    size_t failed = 0;                                          \
    for (size_t i = 0; i < tests_count; i++) {                  \
        bool ok = true;                                         \
        tests[i].test_body(i + 1, tests[i].test_name, &ok);     \
        if (false == ok) {                                      \
            failed++;                                           \
            continue;                                           \
        }                                                       \
                                                                \
        fprintf(                                                \
            TEST_SUCCESS_STREAM,                                \
            "[TEST %02zu] PASSED %s\n",                         \
            i + 1, tests[i].test_name                           \
        );                                                      \
    }                                                           \
                                                                \
    fprintf(                                                    \
        TEST_SUCCESS_STREAM,                                    \
        "\n%zu tests, %zu passed, %zu failed\n",                \
        tests_count, tests_count - failed, failed               \
    );                                                          \
    fflush(TEST_SUCCESS_STREAM);                                \
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;            \
}

#endif //PLAYGROUND_TESTING_H
