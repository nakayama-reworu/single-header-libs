#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "call_checked.h"
#include "vector.h"

#include "testing/testing.h"

Testing_Fact(empty_vector_has_size_and_capacity_of_0) {
    Vector_Of(int) sut = {0};

    Testing_Assert(0 == sut.Size, "size must be 0 for empty vector");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 for empty vector");
}

Testing_Fact(PushBack_appends_elements) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);

    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    Testing_Assert(elementsCount == sut.Size, "expected size to be %zu but was %zu", elementsCount, sut.Size);
    for (size_t i = 0; i < elementsCount; i++) {
        Testing_Assert(
                elements[i] == sut.Items[i],
                "expected element %zu to be %d but was %d", i, elements[i], sut.Items[i]
        );
    }

    Vector_Free(&sut);
}

Testing_Fact(Free_sets_size_and_capacity_to_0) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Free(&sut);
    Testing_Assert(0 == sut.Size, "size must be 0 after free");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 after free");
}

Testing_Fact(TryPopBack_returns_false_for_empty_vector) {
    Vector_Of(int) sut = {0};

    int value;
    Testing_Assert(false == Vector_TryPopBack(&sut, &value), "expected TryPopBack to return false for empty vector");
}

Testing_Fact(TryPopBack_returns_elements_in_reverse_order) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);

    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    size_t j = elementsCount - 1;
    int value;
    while (Vector_TryPopBack(&sut, &value)) {
        Testing_Assert(elements[j] == value, "expected element %zu to be %d but was %d", j, elements[j], value);
        j--;
    }
    Testing_Assert(0 == sut.Size, "expected vector to be empty after TryPopBack returns false");

    Vector_Free(&sut);
}

Testing_Fact(ForEach_never_executes_body_for_empty_list) {
    Vector_Of(int) sut = {0};

    int bodyExecuted = false;
    Vector_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty list");

    Vector_Free(&sut);
}

Testing_Fact(ForEach_iterates_over_all_elements) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    size_t j = 0;
    Vector_ForEach(pValue, sut) {
        Testing_Assert(elements[j] == *pValue, "expected value %zu to be %d but was %d", j, elements[j], *pValue);
        j++;
    }
    Testing_Assert(
            elementsCount == j,
            "expected count of elements iterated over to be %zu but was %zu", elementsCount, j
    );

    Vector_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(empty_vector_has_size_and_capacity_of_0),
        Testing_AddTest(PushBack_appends_elements),
        Testing_AddTest(Free_sets_size_and_capacity_to_0),
        Testing_AddTest(TryPopBack_returns_false_for_empty_vector),
        Testing_AddTest(TryPopBack_returns_elements_in_reverse_order),
        Testing_AddTest(ForEach_never_executes_body_for_empty_list),
        Testing_AddTest(ForEach_iterates_over_all_elements),
};

Testing_RunAllTests();