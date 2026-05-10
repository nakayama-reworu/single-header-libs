#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "call_checked.h"
#include "list.h"

#include "testing/testing.h"

Testing_Fact(Free_sets_head_pointer_to_null) {
    List_Of(int) *sut = NULL;

    List_PushFront(&sut, 1);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, 3);

    List_Free(&sut);

    Testing_Assert(NULL == sut, "head must be NULL");
}

Testing_Fact(PushFront_inserts_elements) {
    List_Of(int) *sut = NULL;

    List_PushFront(&sut, 42);
    List_PushFront(&sut, 1337);
    List_PushFront(&sut, 69);

    typeof(sut) cur = sut;
    Testing_Assert(69 == cur->Value, "unexpected first element");
    cur = cur->Next;
    Testing_Assert(1337 == cur->Value, "unexpected second element");
    cur = cur->Next;
    Testing_Assert(42 == cur->Value, "unexpected third element");
    Testing_Assert(NULL == cur->Next, "list contains extra elements");

    List_Free(&sut);
}

Testing_Fact(Tail_returns_null_for_empty_list) {
    List_Of(int) *sut = NULL;

    Testing_Assert(NULL == List_Tail(sut), "tail of empty list must be NULL");
}

Testing_Fact(Tail_returns_last_node) {
    List_Of(int) *sut = NULL;
    List_PushFront(&sut, 0);
    typeof(sut) const lastNode = sut;

    for (size_t i = 0; i < 10; i++) {
        List_PushFront(&sut, i);
    }
    typeof(sut) tail = List_Tail(sut);

    Testing_Assert(lastNode == tail, "wrong tail: expected Value=%d, got Value=%d", lastNode->Value, tail->Value);

    List_Free(&sut);
}

Testing_Fact(PushBack_inserts_elements) {
    List_Of(int) *sut = NULL;
    typeof(sut) tail = List_Tail(sut);

    int const values[] = {1, 2, 3, 4, 5, 6, 7};
    size_t const valuesCount = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < valuesCount; i++) {
        List_PushBack(&sut, &tail, values[i]);
    }

    typeof(sut) cur = sut;
    for (size_t i = 0; i < valuesCount; i++, cur = cur->Next) {
        Testing_Assert(values[i] == cur->Value, "expected element %zu to be %d but was %d", i, values[i], cur->Value);
    }

    List_Free(&sut);
}

Testing_Fact(Size_returns_0_for_empty_list) {
    List_Of(int) *sut = NULL;

    size_t const size = List_Size(sut);

    Testing_Assert(0 == size, "expected size to be 0 but was %zu", size);

    List_Free(&sut);
}

Testing_Fact(Size_returns_number_of_elements) {
    List_Of(int) *sut = NULL;
    size_t const expectedSize = 10;

    for (size_t i = 0; i < expectedSize; i++) {
        List_PushFront(&sut, i);
    }
    size_t const size = List_Size(sut);

    Testing_Assert(expectedSize == size, "expected size to be %zu but was %zu", expectedSize, size);

    List_Free(&sut);
}

Testing_Fact(TryPopFront_returns_false_for_empty_list) {
    List_Of(int) *sut = NULL;

    int value;
    Testing_Assert(false == List_TryPopFront(&sut, &value), "expected TryPopFront to return false");

    List_Free(&sut);
}

Testing_Fact(TryPopFront_pops_first_element) {
    List_Of(int) *sut = NULL;

    List_PushFront(&sut, 2);
    List_PushFront(&sut, 1);
    typeof(sut) const second = sut->Next;

    int value = 0;
    bool const ok = List_TryPopFront(&sut, &value);
    Testing_Assert(ok, "expected TryPopFront to return true");
    Testing_Assert(1 == value, "expected first value to be 1 but was %d", value);
    Testing_Assert(second == sut, "expected head to be the second node");

    List_Free(&sut);
}

Testing_Fact(ForEach_never_executes_body_for_empty_list) {
    List_Of(int) *sut = NULL;

    int bodyExecuted = false;
    List_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty list");

    List_Free(&sut);
}

Testing_Fact(ForEach_iterates_over_all_elements) {
    List_Of(int) *sut = NULL;
    typeof(sut) tail = List_Tail(sut);

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        List_PushBack(&sut, &tail, elements[i]);
    }

    size_t j = 0;
    List_ForEach(pValue, sut) {
        Testing_Assert(elements[j] == *pValue, "expected value %zu to be %d but was %d", j, elements[j], *pValue);
        j++;
    }
    Testing_Assert(
            elementsCount == j,
            "expected count of elements iterated over to be %zu but was %zu", elementsCount, j
    );

    List_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(Free_sets_head_pointer_to_null),
        Testing_AddTest(PushFront_inserts_elements),
        Testing_AddTest(Tail_returns_null_for_empty_list),
        Testing_AddTest(Tail_returns_last_node),
        Testing_AddTest(PushBack_inserts_elements),
        Testing_AddTest(Size_returns_0_for_empty_list),
        Testing_AddTest(Size_returns_number_of_elements),
        Testing_AddTest(TryPopFront_returns_false_for_empty_list),
        Testing_AddTest(TryPopFront_pops_first_element),
        Testing_AddTest(ForEach_never_executes_body_for_empty_list),
        Testing_AddTest(ForEach_iterates_over_all_elements),
};

Testing_RunAllTests();
