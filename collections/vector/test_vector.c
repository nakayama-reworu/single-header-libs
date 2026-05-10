#include <string.h>

#define TEST_FILE_NAME_ONLY

#include "testing/testing.h"

#include "vector.h"

test_fact(vector_is_created_empty) {
    int *v = vector_empty_of_type(int);

    test_assert(vector_empty(v), "vector was not empty");

    vector_free(v);
}

test_fact(vector_is_created_with_given_size) {
    const size_t expected_size = 42;
    int *v = vector_new_of_type(int, expected_size);

    const size_t size = vector_size(v);

    test_assert(expected_size == size, "Expected size to be %zu but was %zu", expected_size, vector_size(v));

    vector_free(v);
}

test_fact(vector_contains_appended_element) {
    int *v = vector_empty_of_type(int);
    const int element = 42;
    const size_t expected_size = 1;

    vector_append(v, element);

    test_assert(vector_size(v) == expected_size, "expected vector size to be %zu", expected_size);
    test_assert(v[vector_size(v) - 1] == element, "expected last element to be equal to %d", element);

    vector_free(v);
}

test_fact(vector_contains_all_appended_elements) {
    int *v = vector_empty_of_type(int);
    const int elements[] = {1, 2, -3};
    const size_t expected_size = sizeof(elements) / sizeof(elements[0]);

    vector_append_values(v, elements[0], elements[1], elements[2]);

    test_assert(vector_size(v) == expected_size, "expected vector size to be %zu", expected_size);
    for (size_t i = 0; i < expected_size; i++) {
        test_assert(
                v[i] == elements[i],
                "expected `%s[%zu]` to be equal %d", test_nameof(v), i, elements[i]
        );
    }

    vector_free(v);
}

test_fact(can_pop_from_non_empty_vector) {
    const int expected_value = 42;
    int *v = vector_empty_of_type(int);
    vector_append_values(v, 1, 2, 3, expected_value);
    const size_t expected_size = vector_size(v) - 1;

    int result = 0;
    const bool ok = vector_pop_to(v, &result);

    test_assert(ok, "expected `%s` to succeed", test_nameof(vector_pop_to));
    test_assert(vector_size(v) == expected_size, "expected vector size to be %zu", expected_size);
    test_assert(expected_value == result, "expected popped value to be %d but was %d", expected_value, result);

    vector_free(v);
}

test_fact(cannot_pop_from_empty_vector) {
    int *v = vector_empty_of_type(int);

    int result = 0;
    const bool ok = vector_pop_to(v, &result);

    test_assert(false == ok, "expected `%s` to fail", test_nameof(vector_pop_to));
    test_assert(vector_empty(v), "expected vector remain empty");

    vector_free(v);
}

test_fact(vector_pop_removes_and_returns_last_element) {
    int *v = vector_empty_of_type(int);
    const int elements[] = {1, 2, -3};
    const size_t expected_size = sizeof(elements) / sizeof(elements[0]);

    vector_append_values(v, elements[2], elements[1], elements[0]);

    for (size_t i = 0; i < expected_size; i++) {
        const int value = vector_pop(v);
        test_assert(
                value == elements[i],
                "expected popped value to be equal %d (`%s` == %zu)", elements[i], test_nameof(i), i
        );
    }
    test_assert(vector_empty(v), "expected vector remain empty");

    vector_free(v);
}

test_fact(vector_foreach_iterates_over_all_elements) {
    int *v = vector_empty_of_type(int);
    const int elements[] = {1, 2, -3};
    const size_t elements_count = sizeof(elements) / sizeof(elements[0]);
    int result[elements_count];

    vector_append_range(v, elements, elements_count);

    int *it = result;
    vector_foreach(v_it, v) {
        *it = *v_it;
        it++;
    }
    test_assert(
            0 == memcmp(result, elements, sizeof(elements)),
            "expected elements iterated over to be the same as appended"
    );

    vector_free(v);
}

test_all_tests = {
        test_case(vector_is_created_empty),
        test_case(vector_is_created_with_given_size),
        test_case(vector_contains_appended_element),
        test_case(vector_contains_all_appended_elements),
        test_case(can_pop_from_non_empty_vector),
        test_case(cannot_pop_from_empty_vector),
        test_case(vector_pop_removes_and_returns_last_element),
        test_case(vector_foreach_iterates_over_all_elements),
};

test_run_all_tests()