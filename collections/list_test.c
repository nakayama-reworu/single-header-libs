#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "call_checked.h"
#include "list.h"

#include "testing/testing.h"

Testing_Fact(PushFront_inserts_elements) {
    List_Of(int) *sut = LIST_EMPTY;

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
}

Testing_Fact(Tail_gets_last_node) {
    List_Of(int) *sut = LIST_EMPTY;
    List_PushFront(&sut, 0);
    typeof(sut) const lastNode = sut;

    for (size_t i = 0; i < 10; i++) {
        List_PushFront(&sut, i);
    }
    typeof(sut) tail = List_Tail(sut);

    Testing_Assert(lastNode == tail, "wrong tail: expected Value=%d, got Value=%d", lastNode->Value, tail->Value);
}

Testing_Fact(PushBack_inserts_elements) {
    List_Of(int) *sut = LIST_EMPTY;
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
}

Testing_AllTests = {
        Testing_Test(PushFront_inserts_elements),
        Testing_Test(Tail_gets_last_node),
        Testing_Test(PushBack_inserts_elements),
};

Testing_RunAllTests();

//int main(void) {
//    List_Of(int) *list = LIST_EMPTY;
//
//    List_PushFront(&list, 1);
//    List_PushFront(&list, 2);
//    List_PushFront(&list, 3);
//
//    __auto_type tail = List_Tail(list);
//
//    List_PushBack(&list, &tail, 4);
//    List_PushBack(&list, &tail, 5);
//    List_PushBack(&list, &tail, 6);
//
//    List_ForEach(pValue, list) {
//        printf("%d\n", *pValue);
//    }
//    printf("size=%zu\n", List_Size(list));
//
//    int value;
//    while (List_TryPopFront(&list, &value)) {
//        printf("%d\n", value);
//    }
//    printf("size=%zu\n", List_Size(list));
//
//    List_Free(list);
//    list = NULL, tail = NULL;
//
//    List_PushBack(&list, &tail, 420);
//    List_PushBack(&list, &tail, 1337);
//    List_PushBack(&list, &tail, 1999);
//
//    List_ForEach(pValue, list) {
//        printf("%d\n", *pValue);
//    }
//    printf("size=%zu\n", List_Size(list));
//    List_Free(list);
//
//    return EXIT_SUCCESS;
//}