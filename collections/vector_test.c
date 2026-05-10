#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "vector.h"

#include "testing/testing.h"

typedef Vector(int) IntVector;

Testing_Fact(Empty_returns_vector_with_Size_and_Capacity_set_to_0) {
    IntVector sut = Vector_Empty(IntVector);

    Testing_Assert(0 == sut.Size, "size must be 0 for empty vector");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 for empty vector");
}

Testing_Fact(FromPtr_returns_vector_with_elements_from_given_address) {
    int nums[] = {1, 2, 3, 4, 5};
    size_t const numsSize = sizeof(nums) / sizeof(*nums);

    IntVector sut = Vector_FromPtr(IntVector, nums, numsSize);

    Testing_Assert(numsSize == sut.Size, "wrong Size");
    Testing_Assert(0 == memcmp(nums, sut.Items, numsSize * sizeof(int)), "wrong contents");

    Vector_Free(&sut);
}

Testing_Fact(FromArray_returns_vector_with_elements_from_given_automatic_array) {
    int nums[] = {1, 2, 3, 4, 5};
    size_t const numsSize = sizeof(nums) / sizeof(*nums);

    IntVector sut = Vector_FromArray(IntVector, nums);

    Testing_Assert(numsSize == sut.Size, "wrong Size");
    Testing_Assert(0 == memcmp(nums, sut.Items, numsSize * sizeof(int)), "wrong contents");

    Vector_Free(&sut);
}

Testing_Fact(FromArray_returns_vector_with_elements_from_given_array_literal) {
    int nums[] = {1, 2, 3, 4, 5};
    size_t const numsSize = sizeof(nums) / sizeof(*nums);

    IntVector sut = Vector_FromArray(IntVector, ((int[]) {1, 2, 3, 4, 5}));

    Testing_Assert(numsSize == sut.Size, "wrong Size");
    Testing_Assert(0 == memcmp(nums, sut.Items, numsSize * sizeof(int)), "wrong contents");

    Vector_Free(&sut);
}

Testing_Fact(From_returns_vector_with_elements_from_given_struct) {
    IntVector src = Vector_FromArray(IntVector, ((int[]) {1, 2, 3, 4, 5}));

    IntVector sut = Vector_From(IntVector, src);

    Testing_Assert(src.Size == sut.Size, "wrong Size");
    Testing_Assert(0 == memcmp(src.Items, sut.Items, src.Size * sizeof(int)), "wrong contents");

    Vector_Free(&sut);
}

Testing_Fact(Of_returns_vector_with_given_elements) {
    int nums[] = {1, 2, 3, 4, 5};
    size_t const numsSize = sizeof(nums) / sizeof(*nums);

    IntVector sut = Vector_Of(IntVector, 1, 2, 3, 4, 5);

    Testing_Assert(numsSize == sut.Size, "wrong Size");
    Testing_Assert(0 == memcmp(nums, sut.Items, numsSize * sizeof(int)), "wrong contents");

    Vector_Free(&sut);
}

Testing_Fact(PushBack_appends_elements) {
    IntVector sut = Vector_Empty(IntVector);

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
    IntVector sut = Vector_Empty(IntVector);

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Free(&sut);
    Testing_Assert(0 == sut.Size, "size must be 0 after free");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 after free");
}

Testing_Fact(TryPopBack_returns_false_for_empty_vector) {
    IntVector sut = Vector_Empty(IntVector);

    int value;
    Testing_Assert(false == Vector_TryPopBack(&sut, &value), "expected TryPopBack to return false for empty vector");
}

Testing_Fact(TryPopBack_returns_elements_in_reverse_order) {
    IntVector sut = Vector_Empty(IntVector);

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

Testing_Fact(Empty_returns_true_for_empty_vector) {
    IntVector sut = Vector_Empty(IntVector);

    Testing_Assert(Vector_IsEmpty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_false_for_non_empty) {
    IntVector sut = Vector_Empty(IntVector);

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Testing_Assert(false == Vector_IsEmpty(sut), "expected Empty to return false");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_vector_with_all_elements_removed) {
    IntVector sut = Vector_Empty(IntVector);

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    int value;
    while (Vector_TryPopBack(&sut, &value)) {}

    Testing_Assert(Vector_IsEmpty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_after_Clear) {
    IntVector sut = Vector_Empty(IntVector);

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Clear(&sut);

    Testing_Assert(Vector_IsEmpty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(Empty_returns_vector_with_Size_and_Capacity_set_to_0),
        Testing_AddTest(FromPtr_returns_vector_with_elements_from_given_address),
        Testing_AddTest(FromArray_returns_vector_with_elements_from_given_automatic_array),
        Testing_AddTest(FromArray_returns_vector_with_elements_from_given_array_literal),
        Testing_AddTest(From_returns_vector_with_elements_from_given_struct),
        Testing_AddTest(Of_returns_vector_with_given_elements),
        Testing_AddTest(PushBack_appends_elements),
        Testing_AddTest(Free_sets_size_and_capacity_to_0),
        Testing_AddTest(TryPopBack_returns_false_for_empty_vector),
        Testing_AddTest(TryPopBack_returns_elements_in_reverse_order),
        Testing_AddTest(Empty_returns_true_for_empty_vector),
        Testing_AddTest(Empty_returns_false_for_non_empty),
        Testing_AddTest(Empty_returns_true_for_vector_with_all_elements_removed),
        Testing_AddTest(Empty_returns_true_after_Clear),
};

Testing_RunAllTests();
