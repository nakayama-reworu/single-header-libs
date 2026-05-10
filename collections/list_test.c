#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"

#include "testing/testing.h"

typedef List(int) IntsList;

Testing_Fact(PushFront_sets_Head_and_Tail_to_new_node_for_empty_list) {
    IntsList sut = List_Empty(IntsList);
    int const expectedValue = 42;

    List_PushFront(&sut, expectedValue);

    Testing_Assert(NULL != sut.Head, "expected Head to not be NULL");
    Testing_Assert(NULL != sut.Tail, "expected Tail to not be NULL");
    Testing_Assert(sut.Head == sut.Tail, "expected Head and Tail to point to the same node");
    Testing_Assert(
            expectedValue == sut.Head->Value,
            "expected value to be %d but was %d", expectedValue, sut.Head->Value
    );

    List_Free(&sut);
}

Testing_Fact(PushFront_sets_Head_to_new_node_and_links_it_with_previous_head) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 1);
    typeof(sut.Head) const oldHead = sut.Head;
    int const expectedValue = 42;

    List_PushFront(&sut, expectedValue);

    Testing_Assert(
            expectedValue == sut.Head->Value,
            "expected value to be %d but was %d", expectedValue, sut.Head->Value
    );
    Testing_Assert(oldHead == sut.Head->Next, "expected Next to point to previous Head");
    Testing_Assert(oldHead->Prev == sut.Head, "expected previous Head's Prev to point to new Head");

    List_Free(&sut);
}

Testing_Fact(PushBack_sets_Head_and_Tail_to_new_node_for_empty_list) {
    IntsList sut = List_Empty(IntsList);
    int const expectedValue = 42;

    List_PushBack(&sut, expectedValue);

    Testing_Assert(NULL != sut.Head, "expected Head to not be NULL");
    Testing_Assert(NULL != sut.Tail, "expected Tail to not be NULL");
    Testing_Assert(sut.Head == sut.Tail, "expected Head and Tail to point to the same node");
    Testing_Assert(
            expectedValue == sut.Head->Value,
            "expected value to be %d but was %d", expectedValue, sut.Head->Value
    );

    List_Free(&sut);
}

Testing_Fact(PushBack_sets_Tail_to_new_node_and_links_it_with_previous_tail) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 1);
    typeof(sut.Tail) const oldTail = sut.Tail;
    int const expectedValue = 42;

    List_PushBack(&sut, expectedValue);

    Testing_Assert(
            expectedValue == sut.Tail->Value,
            "expected value to be %d but was %d", expectedValue, sut.Tail->Value
    );
    Testing_Assert(oldTail == sut.Tail->Prev, "expected Prev to point to previous Tail");
    Testing_Assert(oldTail->Next == sut.Tail, "expected previous Tail's Next to point to new Tail");

    List_Free(&sut);
}

Testing_Fact(Size_is_0_for_empty_list) {
    IntsList sut = List_Empty(IntsList);

    Testing_Assert(0 == sut.Size, "expected size to be 0 but was %zu", sut.Size);

    List_Free(&sut);
}

Testing_Fact(Size_is_equal_to_number_of_elements_inserted) {
    IntsList sut = List_Empty(IntsList);
    size_t const expectedSize = 10;

    for (size_t i = 0; i < expectedSize; i++) {
        List_PushFront(&sut, i);
    }

    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    List_Free(&sut);
}

Testing_Fact(TryPopFront_returns_false_for_empty_list) {
    IntsList sut = List_Empty(IntsList);

    int value;
    Testing_Assert(false == List_TryPopFront(&sut, &value), "expected TryPopFront to return false");

    List_Free(&sut);
}

Testing_Fact(TryPopFront_pops_first_element_and_decreases_size) {
    IntsList sut = List_Empty(IntsList);
    int const expectedValue = 1;
    List_PushFront(&sut, 4);
    List_PushFront(&sut, 3);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, expectedValue);
    size_t const expectedSize = sut.Size - 1;
    void const *const expectedHead = sut.Head->Next;

    int value = 0;
    bool const ok = List_TryPopFront(&sut, &value);
    Testing_Assert(ok, "expected TryPopFront to return true");
    Testing_Assert(expectedValue == value, "expected first value to be %d but was %d", expectedValue, value);
    Testing_Assert(expectedHead == sut.Head, "head points to wrong node");
    Testing_Assert(NULL == sut.Head->Prev, "expected Prev of Head to be NULL");
    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    List_Free(&sut);
}

Testing_Fact(TryPopFront_sets_both_Head_and_Tail_to_NULL_if_there_is_single_element) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 2);

    int value = 0;
    bool const ok = List_TryPopFront(&sut, &value);
    Testing_Assert(ok, "expected TryPopFront to return true");
    Testing_Assert(NULL == sut.Head, "expected Head to be NULL");
    Testing_Assert(NULL == sut.Tail, "expected Tail to be NULL");

    List_Free(&sut);
}

Testing_Fact(TryPopFront_discards_first_elememnt_if_value_pointer_is_NULL) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 4);
    List_PushFront(&sut, 3);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, 1);
    size_t const expectedSize = sut.Size - 1;
    void const *const expectedHead = sut.Head->Next;

    bool const ok = List_TryPopFront(&sut, NULL);
    Testing_Assert(ok, "expected TryPopFront to return true");
    Testing_Assert(expectedHead == sut.Head, "head points to wrong node");
    Testing_Assert(NULL == sut.Head->Prev, "expected Prev of Head to be NULL");
    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    List_Free(&sut);
}

Testing_Fact(TryPopBack_returns_false_for_empty_list) {
    IntsList sut = List_Empty(IntsList);

    int value;
    Testing_Assert(false == List_TryPopBack(&sut, &value), "expected TryPopFront to return false");

    List_Free(&sut);
}

Testing_Fact(TryPopBack_pops_first_element_and_decreases_size) {
    IntsList sut = List_Empty(IntsList);
    int const expectedValue = 4;
    List_PushFront(&sut, expectedValue);
    List_PushFront(&sut, 3);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, 1);
    size_t const expectedSize = sut.Size - 1;
    void const *const expectedTail = sut.Tail->Prev;

    int value = 0;
    bool const ok = List_TryPopBack(&sut, &value);
    Testing_Assert(ok, "expected TryPopFront to return true");
    Testing_Assert(expectedValue == value, "expected first value to be %d but was %d", expectedValue, value);
    Testing_Assert(expectedTail == sut.Tail, "Tail points to wrong node");
    Testing_Assert(NULL == sut.Tail->Next, "expected Next of Tail to be NULL");
    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    List_Free(&sut);
}

Testing_Fact(TryPopBack_sets_both_Head_and_Tail_to_NULL_if_there_is_single_element) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 2);

    int value = 0;
    bool const ok = List_TryPopBack(&sut, &value);
    Testing_Assert(ok, "expected TryPopBack to return true");
    Testing_Assert(NULL == sut.Head, "expected Head to be NULL");
    Testing_Assert(NULL == sut.Tail, "expected Tail to be NULL");

    List_Free(&sut);
}

Testing_Fact(TryPopBack_discards_first_elememnt_if_value_pointer_is_NULL) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 4);
    List_PushFront(&sut, 3);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, 1);
    size_t const expectedSize = sut.Size - 1;
    void const *const expectedTail = sut.Tail->Prev;

    bool const ok = List_TryPopBack(&sut, NULL);
    Testing_Assert(ok, "expected TryPopBack to return true");
    Testing_Assert(expectedTail == sut.Tail, "head points to wrong node");
    Testing_Assert(NULL == sut.Tail->Next, "expected Next of Tail to be NULL");
    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    List_Free(&sut);
}

Testing_Fact(ForEach_never_executes_body_for_empty_list) {
    IntsList sut = List_Empty(IntsList);

    int bodyExecuted = false;
    List_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty list");

    List_Free(&sut);
}

Testing_Fact(ForEach_iterates_over_all_elements_in_list) {
    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t const elementsCount = sizeof(elements) / sizeof(*elements);
    IntsList sut = List_Empty(IntsList);
    for (size_t i = 0; i < elementsCount; i++) {
        List_PushBack(&sut, elements[i]);
    }

    size_t j = 0;
    List_ForEach(it, sut) {
        Testing_Assert(elements[j] == *it, "expected element %zu to be %d but was %d", j, elements[j], *it);
        j++;
    }

    Testing_Assert(elementsCount == j, "expected ForEach to iterate over all elements");

    List_Free(&sut);
}

Testing_Fact(IsEmpty_returns_true_for_new_empty_list) {
    IntsList sut = List_Empty(IntsList);

    Testing_Assert(List_IsEmpty(sut), "expected Empty to return true");

    List_Free(&sut);
}

Testing_Fact(IsEmpty_returns_false_for_non_empty_lists) {
    IntsList sut = List_Empty(IntsList);
    List_PushFront(&sut, 42);

    Testing_Assert(false == List_IsEmpty(sut), "expected Empty to return false");

    List_Free(&sut);
}

Testing_Fact(IsEmpty_returns_true_after_all_list_elements_were_removed) {
    IntsList sut = List_Empty(IntsList);

    List_PushFront(&sut, 42);
    List_PushFront(&sut, 1337);

    int value;
    while (List_TryPopFront(&sut, &value)) {
        (void) 5;
    }

    Testing_Assert(List_IsEmpty(sut), "expected Empty to return true");

    List_Free(&sut);
}

Testing_Fact(Free_sets_list_to_Empty) {
    IntsList sut = List_Empty(IntsList);

    List_PushFront(&sut, 1);
    List_PushFront(&sut, 2);
    List_PushFront(&sut, 3);

    List_Free(&sut);

    Testing_Assert(List_IsEmpty(sut), "head must be NULL");
}

Testing_Fact(At_returns_pointer_to_value_for_valid_non_negative_index) {
    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t const elementsCount = sizeof(elements) / sizeof(*elements);
    int const factor = 2;
    IntsList sut = List_Empty(IntsList);
    for (size_t i = 0; i < elementsCount; i++) {
        List_PushBack(&sut, elements[i]);
    }

    for (size_t i = 0; i < elementsCount; i++) {
        int *const it = List_At(sut, i);
        Testing_Assert(NULL != it, "expected pointer to value at %zu to not be NULL", i);
        *it *= factor;
    }

    size_t j = 0;
    List_ForEach(it, sut) {
        int const expected = factor * elements[j];
        Testing_Assert(expected == *it, "expected element %zu to be %d but was %d", j, expected, *it);
        j++;
    }

    List_Free(&sut);
}

Testing_Fact(At_returns_pointer_to_value_for_valid_negative_index) {
    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t const elementsCount = sizeof(elements) / sizeof(*elements);
    int const factor = 2;
    IntsList sut = List_Empty(IntsList);
    for (size_t i = 0; i < elementsCount; i++) {
        List_PushBack(&sut, elements[i]);
    }

    for (long long i = -(long long) sut.Size; i <= -1; i++) {
        int *const it = List_At(sut, i);
        Testing_Assert(NULL != it, "expected pointer to value at %lld to not be NULL", i);
        *it *= factor;
    }

    size_t j = 0;
    List_ForEach(it, sut) {
        int const expected = factor * elements[j];
        Testing_Assert(expected == *it, "expected element %zu to be %d but was %d", j, expected, *it);
        j++;
    }

    List_Free(&sut);
}

Testing_Fact(At_returns_NULL_for_invalid_index) {
    Testing_Assert(NULL == List_At(List_Empty(IntsList), 0), "expected At to return NULL for empty list");
    Testing_Assert(NULL == List_At(List_Empty(IntsList), -1), "expected At to return NULL for empty list");

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t const elementsCount = sizeof(elements) / sizeof(*elements);
    IntsList sut = List_Empty(IntsList);
    for (size_t i = 0; i < elementsCount; i++) {
        List_PushBack(&sut, elements[i]);
    }

    Testing_Assert(NULL == List_At(sut, sut.Size), "expected At to return NULL for out-of-range index");
    Testing_Assert(
            NULL == List_At(sut, -(long long) sut.Size - 1),
            "expected At to return NULL for out-of-range index"
    );

    List_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(PushFront_sets_Head_and_Tail_to_new_node_for_empty_list),
        Testing_AddTest(PushFront_sets_Head_to_new_node_and_links_it_with_previous_head),
        Testing_AddTest(PushBack_sets_Head_and_Tail_to_new_node_for_empty_list),
        Testing_AddTest(PushBack_sets_Tail_to_new_node_and_links_it_with_previous_tail),
        Testing_AddTest(Size_is_0_for_empty_list),
        Testing_AddTest(Size_is_equal_to_number_of_elements_inserted),
        Testing_AddTest(TryPopFront_returns_false_for_empty_list),
        Testing_AddTest(TryPopFront_pops_first_element_and_decreases_size),
        Testing_AddTest(TryPopFront_sets_both_Head_and_Tail_to_NULL_if_there_is_single_element),
        Testing_AddTest(TryPopFront_discards_first_elememnt_if_value_pointer_is_NULL),
        Testing_AddTest(TryPopBack_returns_false_for_empty_list),
        Testing_AddTest(TryPopBack_pops_first_element_and_decreases_size),
        Testing_AddTest(TryPopBack_sets_both_Head_and_Tail_to_NULL_if_there_is_single_element),
        Testing_AddTest(TryPopBack_discards_first_elememnt_if_value_pointer_is_NULL),
        Testing_AddTest(ForEach_never_executes_body_for_empty_list),
        Testing_AddTest(ForEach_iterates_over_all_elements_in_list),
        Testing_AddTest(IsEmpty_returns_true_for_new_empty_list),
        Testing_AddTest(IsEmpty_returns_false_for_non_empty_lists),
        Testing_AddTest(IsEmpty_returns_true_after_all_list_elements_were_removed),
        Testing_AddTest(Free_sets_list_to_Empty),
        Testing_AddTest(At_returns_pointer_to_value_for_valid_non_negative_index),
        Testing_AddTest(At_returns_pointer_to_value_for_valid_negative_index),
        Testing_AddTest(At_returns_NULL_for_invalid_index),
};

Testing_RunAllTests();
