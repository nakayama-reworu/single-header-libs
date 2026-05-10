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

typedef void (*TestImplementation)(size_t, const char *, bool *);

typedef struct {
    const TestImplementation Implementation;
    const char *const Name;
} TestCase;

#define Testing_Nameof(it) ({(void) it; #it;})

#define Testing_Fact(name) static void name(const size_t __test_no, const char* const __test_name, bool* const __ok)

#define Testing_Assert(assertion, format, ...)                                      \
do {                                                                                \
    const bool _success = (assertion);                                              \
    if (false == _success) {                                                        \
        *__ok = false;                                                              \
        fprintf(                                                                    \
            TEST_FAILURE_STREAM,                                                    \
            "[TEST %02zu] FAILED %s\n%s:%d: Assertion '%s' failed, " format "\n",   \
            __test_no, __test_name,                                                 \
            TEST_FILE, __LINE__,                                                    \
            #assertion, ##__VA_ARGS__                                               \
        );                                                                          \
        fflush(TEST_FAILURE_STREAM);                                                \
        return;                                                                     \
    }                                                                               \
} while (0)

#define Testing_Test(name) (TestCase) { .Name = #name, .Implementation = name }

#define Testing_AllTests static const TestCase tests[]

#define Testing_RunAllTests()                                   \
int main(void) {                                                \
    const size_t testsCount = sizeof(tests) / sizeof(*tests);   \
    size_t failed = 0;                                          \
    for (size_t i = 0; i < testsCount; i++) {                   \
        bool ok = true;                                         \
        tests[i].Implementation(i + 1, tests[i].Name, &ok);     \
        if (false == ok) {                                      \
            failed++;                                           \
            continue;                                           \
        }                                                       \
                                                                \
        fprintf(                                                \
            TEST_SUCCESS_STREAM,                                \
            "[TEST %02zu] PASSED %s\n",                         \
            i + 1, tests[i].Name                                \
        );                                                      \
    }                                                           \
                                                                \
    fprintf(                                                    \
        TEST_SUCCESS_STREAM,                                    \
        "\n%zu tests, %zu passed, %zu failed\n",                \
        testsCount, testsCount - failed, failed                 \
    );                                                          \
    fflush(TEST_SUCCESS_STREAM);                                \
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;            \
}                                                               \
struct Testing_DummyStruct_ {}

#endif //PLAYGROUND_TESTING_H
