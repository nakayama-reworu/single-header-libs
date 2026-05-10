#include "span.h"

#include "testing/testing.h"

typedef Span(int) IntSpan;
typedef ReadOnlySpan(int) IntConstSpan;

Testing_Fact(Span_allows_modifying_elements) {
    int nums[] = {1, 2, 3, 4, 5, 6};
    int const expectedValue = 42;

    IntSpan span = Span_FromArray(IntSpan, nums);
    span.Items[1] = expectedValue;

    Testing_Assert(expectedValue == nums[1], "expected value to be %d but was %d", expectedValue, nums[1]);
}

Testing_Fact(Slice_returns_empty_span_if_start_is_greater_or_equal_to_end) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    Testing_Assert(Span_IsEmpty(Span_Slice(IntConstSpan, sut, 0, 0)), "expected span to be empty");
    Testing_Assert(
            Span_IsEmpty(Span_Slice(IntConstSpan, sut, sut.Size, sut.Size)),
            "expected span to be empty"
    );

    Testing_Assert(Span_IsEmpty(Span_Slice(IntConstSpan, sut, 3, 3)), "expected span to be empty");
    Testing_Assert(Span_IsEmpty(Span_Slice(IntConstSpan, sut, 4, 2)), "expected span to be empty");

    Testing_Assert(Span_IsEmpty(Span_Slice(IntConstSpan, sut, -3, -3)), "expected span to be empty");
    Testing_Assert(Span_IsEmpty(Span_Slice(IntConstSpan, sut, -2, -4)), "expected span to be empty");
}

Testing_Fact(Slice_returns_all_elements_if_start_is_0_and_end_is_Size) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, 0, sut.Size);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(sut.Size == span.Size, "expected Size to be %zu but was %zu", sut.Size, span.Size);
}

Testing_Fact(Slice_handles_valid_negative_start) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    int const *const expectedItems = sut.Items + sut.Size - 3;
    size_t const expectedSize = 3;

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, -3, sut.Size);

    Testing_Assert(expectedItems == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(Slice_handles_negative_start_out_of_bounds) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, -(long long) sut.Size, sut.Size);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(sut.Size == span.Size, "expected Size to be %zu but was %zu", sut.Size, span.Size);
}

Testing_Fact(Slice_handles_start_greater_than_size) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, sut.Size, sut.Size);

    Testing_Assert(Span_IsEmpty(span), "expected span to be empty");
}

Testing_Fact(Slice_handles_valid_negative_end) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    size_t const expectedSize = 3;

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, 0, -3);

    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(Slice_handles_negative_end_out_of_bounds) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan span = Span_Slice(IntConstSpan, sut, 0, -(long long) sut.Size);

    Testing_Assert(Span_IsEmpty(span), "expected span to be empty");
}

Testing_Fact(Slice_handles_end_greater_than_size) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, 0, 999);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(sut.Size == span.Size, "expected Size to be %zu but was %zu", sut.Size, span.Size);
}

Testing_Fact(Slice_returns_span_with_given_start_and_end) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    long long const start = 2;
    long long const end = -1;
    size_t const expectedSize = 3;
    int const *expectedItems = &sut.Items[start];

    IntConstSpan const span = Span_Slice(IntConstSpan, sut, start, end);

    Testing_Assert(expectedItems == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(SliceFrom_returns_all_elements_if_start_is_0) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_SliceFrom(IntConstSpan, sut, 0);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(sut.Size == span.Size, "expected Size to be %zu but was %zu", sut.Size, span.Size);
}

Testing_Fact(SliceFrom_returns_span_that_starts_from_given_positive_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    long long const start = 2;
    size_t const expectedSize = 4;
    int const *expectedItems = &sut.Items[(sut.Size + start) % sut.Size];

    IntConstSpan const span = Span_SliceFrom(IntConstSpan, sut, start);

    Testing_Assert(expectedItems == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(SliceFrom_returns_span_that_starts_from_given_negative_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    long long const start = -3;
    size_t const expectedSize = 3;
    int const *expectedItems = &sut.Items[(sut.Size + start) % sut.Size];

    IntConstSpan const span = Span_SliceFrom(IntConstSpan, sut, start);

    Testing_Assert(expectedItems == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(SliceTo_returns_all_elements_if_end_is_Size) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    IntConstSpan const span = Span_SliceTo(IntConstSpan, sut, sut.Size);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(sut.Size == span.Size, "expected Size to be %zu but was %zu", sut.Size, span.Size);
}

Testing_Fact(SliceTo_returns_span_that_ends_at_given_positive_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    long long const end = 4;
    size_t const expectedSize = end;

    IntConstSpan const span = Span_SliceTo(IntConstSpan, sut, end);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(SliceTo_returns_span_that_ends_at_given_negative_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));
    long long const start = -2;
    size_t const expectedSize = 4;

    IntConstSpan const span = Span_SliceTo(IntConstSpan, sut, start);

    Testing_Assert(sut.Items == span.Items, "Items points to wrong element");
    Testing_Assert(expectedSize == span.Size, "expected Size to be %zu but was %zu", expectedSize, span.Size);
}

Testing_Fact(At_returns_pointer_to_element_for_valid_positive_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    for (size_t i = 0; i < sut.Size; i++) {
        int const *const it = Span_At(sut, i);
        Testing_Assert(&sut.Items[i] == it, "At returned wrong pointer");
    }
}

Testing_Fact(At_returns_pointer_to_element_for_valid_negative_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    for (int i = -(int) sut.Size; i <= -1; i++) {
        int const *const it = Span_At(sut, i);
        Testing_Assert(&sut.Items[sut.Size + i] == it, "At returned wrong pointer");
    }
}

Testing_Fact(At_returns_NULL_for_invalid_index) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    Testing_Assert(NULL == Span_At(sut, sut.Size), "expected NULL for out of bounds index");
    Testing_Assert(NULL == Span_At(sut, -(int) sut.Size - 1), "expected NULL for out of bounds index");
    Testing_Assert(NULL == Span_At(sut, 42), "expected NULL for out of bounds index");
    Testing_Assert(NULL == Span_At(sut, -42), "expected NULL for out of bounds index");
}

Testing_Fact(ForEach_never_executes_body_for_empty_span) {
    IntConstSpan const sut = Span_Empty(IntConstSpan);

    int bodyExecuted = false;
    Span_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty span");
}

Testing_Fact(ForEach_iterates_over_all_elements) {
    IntConstSpan const sut = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

    size_t j = 0;
    Span_ForEach(it, sut) {
        Testing_Assert(Span_At(sut, j) == it, "");
        j++;
    }
    Testing_Assert(sut.Size == j, "expected count of elements iterated over to be %zu but was %zu", sut.Size, j);
}

Testing_AllTests = {
        Testing_AddTest(Span_allows_modifying_elements),
        Testing_AddTest(Slice_returns_empty_span_if_start_is_greater_or_equal_to_end),
        Testing_AddTest(Slice_returns_all_elements_if_start_is_0_and_end_is_Size),
        Testing_AddTest(Slice_handles_valid_negative_start),
        Testing_AddTest(Slice_handles_negative_start_out_of_bounds),
        Testing_AddTest(Slice_handles_start_greater_than_size),
        Testing_AddTest(Slice_handles_valid_negative_end),
        Testing_AddTest(Slice_handles_negative_end_out_of_bounds),
        Testing_AddTest(Slice_handles_end_greater_than_size),
        Testing_AddTest(Slice_returns_span_with_given_start_and_end),
        Testing_AddTest(SliceFrom_returns_all_elements_if_start_is_0),
        Testing_AddTest(SliceFrom_returns_span_that_starts_from_given_positive_index),
        Testing_AddTest(SliceFrom_returns_span_that_starts_from_given_negative_index),
        Testing_AddTest(SliceTo_returns_all_elements_if_end_is_Size),
        Testing_AddTest(SliceTo_returns_span_that_ends_at_given_positive_index),
        Testing_AddTest(SliceTo_returns_span_that_ends_at_given_negative_index),
        Testing_AddTest(At_returns_pointer_to_element_for_valid_positive_index),
        Testing_AddTest(At_returns_pointer_to_element_for_valid_negative_index),
        Testing_AddTest(At_returns_NULL_for_invalid_index),
        Testing_AddTest(ForEach_never_executes_body_for_empty_span),
        Testing_AddTest(ForEach_iterates_over_all_elements),
};

Testing_RunAllTests();
