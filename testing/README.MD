# Testing

[testing.h](testing.h)

Minimal testing library.

See [testing_demo.c](testing_demo.c) for a full demo.

### Declaring and running tests

* [Testing_Fact](#testingfact)
* [Testing_Assert](#testingassert)
* [Testing_AllTests](#testingalltests)
* [Testing_RunAllTests](#testingrunalltests)

#### Testing_Fact
```c
#define Testing_Fact(name)
```
Define a test with given `name`.

Example:
```c
Testing_Fact(strlen_returns_correct_length) {
    const char *s = "abc";
    const size_t expected = 3;

    const size_t length = strlen(s);

    Testing_Assert(length == expected, "expected length to be %zu but was %zu", expected, length);
}
```

#### Testing_Assert
```c
#define Testing_Assert(assertion, format, ...)
```
If `assertion` is `true`, continue execution.

If `assertion` is `false`, 
print a failure message with given `format` 
and stop execution of the current test.

#### Testing_AllTests
An array of test cases to be executed.

Example:

(Given `one_plus_one_equals_two`, `always_false` and
`strlen_returns_correct_length` were declared using
`Testing_Fact`)
```c
Testing_AllTests = {
        Testing_AddTest(one_plus_one_equals_two),
        Testing_AddTest(always_false),
        Testing_AddTest(strlen_returns_correct_length),
};
```

#### Testing_RunAllTests
```c
#define Testing_RunAllTests()
```
Expands into `main` definition that will run tests in 
`Testing_AllTests` when executed.

### Control macros

* [TESTING_SUCCESS_STREAM](#testingsuccessstream)
* [TESTING_FAILURE_STREAM](#testingfailurestream)
* [TESTING_FILE_NAME_ONLY](#testingfilenameonly)

#### TESTING_SUCCESS_STREAM
Define before including `testing.h` to set output 
stream for information about passed tests.

Default is `stdout`.

#### TESTING_FAILURE_STREAM
Define before including `testing.h` to set output
stream for information about failed tests.

Default is `stdout`.

#### TESTING_FILE_NAME_ONLY
Define before including `testing.h` to make failure messages
only print file name instead of full path.

Examples:

`TESTING_FILE_NAME_ONLY` is not defined:
```
[TEST 02] FAILED always_false
C:/Users/TUF/Desktop/demo/testing/testing_demo.c:24: Assertion 'a == b' failed, expected a=1 to be equal b=2
```

`TESTING_FILE_NAME_ONLY` is defined:
```
[TEST 02] FAILED always_false
testing_demo.c:24: Assertion 'a == b' failed, expected a=1 to be equal b=2
```
