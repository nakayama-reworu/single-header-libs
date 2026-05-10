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

Testing_Fact(At_returns_pointer_to_element_for_valid_non_negative_index) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    for (size_t i = 0; i < sut.Size; i++) {
        int const *valuePtr = Vector_At(sut, i);
        Testing_Assert(&sut.Items[i] == valuePtr, "At returned wrong pointer");
    }

    Vector_Free(&sut);
}

Testing_Fact(At_returns_pointer_to_element_for_valid_negative_index) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    for (int i = 1; i <= (int) sut.Size; i++) {
        int const *valuePtr = Vector_At(sut, -i);
        Testing_Assert(&sut.Items[sut.Size - i] == valuePtr, "At returned wrong pointer");
    }

    Vector_Free(&sut);
}

Testing_Fact(At_returns_NULL_for_invalid_indices) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    Testing_Assert(NULL == Vector_At(sut, sut.Size), "expected At to return NULL");
    Testing_Assert(NULL == Vector_At(sut, -(int) sut.Size - 1), "expected At to return NULL");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_empty_vector) {
    Vector_Of(int) sut = {0};

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_false_for_non_empty) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Testing_Assert(false == Vector_Empty(sut), "expected Empty to return false");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_vector_with_all_elements_removed) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    int value;
    while (Vector_TryPopBack(&sut, &value)) {}

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_after_Clear) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Clear(&sut);

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Any_returns_false_if_no_elements_match) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Testing_Assert(false == Vector_Any(sut, value, 42 == value), "expected Any to return false");

    Vector_Free(&sut);
}

Testing_Fact(Any_returns_true_if_an_elements_matches) {
    Vector_Of(char const *) sut = {0};

    Vector_PushBack(&sut, "a");
    Vector_PushBack(&sut, "quick");
    Vector_PushBack(&sut, "brown");
    Vector_PushBack(&sut, "fox");

    Testing_Assert(
            Vector_Any(sut, value, ({
                0 == strcmp("brown", value);
            })),
            "expected Any to return true"
    );

    Vector_Free(&sut);
}

Testing_Fact(Slice_returns_an_empty_slice_if_start_and_end_are_equal) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);

    typedef VectorSlice_Of(int) IntSlice;
    IntSlice slice = Vector_Slice(IntSlice, sut, 2, 2);

    Testing_Assert(Vector_Empty(slice), "expected slice to be empty");

    Vector_Free(&sut);
}

Testing_Fact(Slice_handles_out_of_bounds_start) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);

    typedef VectorSlice_Of(int) IntSlice;

    Testing_Assert(Vector_Empty(Vector_SliceFrom(IntSlice, sut, sut.Size)), "expected slice to be empty");

    IntSlice s1 = Vector_Slice(IntSlice, sut, -42, 2);
    size_t const expectedSize1 = 2;
    Testing_Assert(expectedSize1 == s1.Size, "expected size to be %zu but was %zu", expectedSize1, s1.Size);

    IntSlice s2 = Vector_Slice(IntSlice, sut, 42, 2);
    size_t const expectedSize2 = 0;
    Testing_Assert(expectedSize2 == s2.Size, "expected size to be %zu but was %zu", expectedSize2, s2.Size);

    Vector_Free(&sut);
}

Testing_Fact(Slice_handles_out_of_bounds_end) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);

    typedef VectorSlice_Of(int) IntSlice;

    Testing_Assert(Vector_Empty(Vector_SliceTo(IntSlice, sut, 0)), "expected slice to be empty");

    IntSlice s1 = Vector_Slice(IntSlice, sut, 2, 42);
    size_t const expectedSize1 = 2;
    Testing_Assert(expectedSize1 == s1.Size, "expected size to be %zu but was %zu", expectedSize1, s1.Size);

    IntSlice s2 = Vector_Slice(IntSlice, sut, 2, -42);
    size_t const expectedSize2 = 0;
    Testing_Assert(expectedSize2 == s2.Size, "expected size to be %zu but was %zu", expectedSize2, s2.Size);

    Vector_Free(&sut);
}

Testing_Fact(Slice_returns_correct_elements) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);

    typedef VectorSlice_Of(int) IntSlice;

    size_t start = 1;
    size_t end = 3;
    size_t expectedSize = end - start;
    IntSlice slice = Vector_Slice(IntSlice, sut, start, end);

    Testing_Assert(expectedSize == slice.Size, "expected slice size to be %zu but was %zu", expectedSize, slice.Size);
    Testing_Assert(
            Vector_At(sut, start) == slice.Items,
            "expected slice.Items to point to vector element at %zu", start
    );

    Vector_Free(&sut);
}

Testing_Fact(Slice_returns_all_elememnts_if_start_is_0_and_end_is_Size) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);

    typedef VectorSlice_Of(int) IntSlice;
    IntSlice slice = Vector_Slice(IntSlice, sut, 0, sut.Size);

    Testing_Assert(sut.Size == slice.Size, "expected slice size to be equal to vector size");
    Testing_Assert(sut.Items == slice.Items, "expected slice.Items to point to vector's Items");

    Vector_Free(&sut);
}

Testing_Fact(Can_take_slice_of_a_slice) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);
    Vector_PushBack(&sut, 5);
    Vector_PushBack(&sut, 6);

    typedef VectorSlice_Of(int) IntSlice;
    size_t start1 = 1, end1 = 4;
    IntSlice s1 = Vector_Slice(IntSlice, sut, start1, end1);
    size_t start2 = 1, end2 = 2;
    size_t expectedSize2 = end2 - start2;
    IntSlice s2 = Vector_Slice(IntSlice, s1, start2, end2);

    Testing_Assert(expectedSize2 == s2.Size, "expected size to be %zu but was %zu", expectedSize2, s2.Size);
    Testing_Assert(Vector_At(sut, start1 + start2) == s2.Items, "slice's Items points to wrong element");

    Vector_Free(&sut);
}

Testing_Fact(SliceTo_and_SliceFrom_combined_contain_all_elements_of_initial_vector) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);
    Vector_PushBack(&sut, 4);
    Vector_PushBack(&sut, 5);
    Vector_PushBack(&sut, 6);

    typedef VectorSlice_Of(int) IntSlice;

    size_t splitIndex = 3;
    IntSlice s1 = Vector_SliceTo(IntSlice, sut, splitIndex);
    IntSlice s2 = Vector_SliceFrom(IntSlice, sut, splitIndex);

    Testing_Assert(sut.Size == s1.Size + s2.Size, "expected total size of slices to be equal to vector's Size");
    Testing_Assert(
            s1.Items + s1.Size == s2.Items,
            "expected Items of second slice to begin after items of first slice"
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
        Testing_AddTest(At_returns_pointer_to_element_for_valid_non_negative_index),
        Testing_AddTest(At_returns_pointer_to_element_for_valid_negative_index),
        Testing_AddTest(At_returns_NULL_for_invalid_indices),
        Testing_AddTest(Empty_returns_true_for_empty_vector),
        Testing_AddTest(Empty_returns_false_for_non_empty),
        Testing_AddTest(Empty_returns_true_for_vector_with_all_elements_removed),
        Testing_AddTest(Empty_returns_true_after_Clear),
        Testing_AddTest(Any_returns_false_if_no_elements_match),
        Testing_AddTest(Any_returns_true_if_an_elements_matches),
        Testing_AddTest(Slice_returns_an_empty_slice_if_start_and_end_are_equal),
        Testing_AddTest(Slice_handles_out_of_bounds_start),
        Testing_AddTest(Slice_handles_out_of_bounds_end),
        Testing_AddTest(Slice_returns_correct_elements),
        Testing_AddTest(Slice_returns_all_elememnts_if_start_is_0_and_end_is_Size),
        Testing_AddTest(Can_take_slice_of_a_slice),
        Testing_AddTest(SliceTo_and_SliceFrom_combined_contain_all_elements_of_initial_vector),
};

Testing_RunAllTests();
