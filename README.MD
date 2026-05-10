# Single Header Libraries

### Collections

Generic collections with operations implemented as macros.

Include [call_checked.h](call_checked.h)
before including the collection header.

* [Vector](#vector)
* [Linked List](#linked-list)
* [Map](#map)

### Strings

See [readme for strings](strings/README.MD).

### Other

* [Header-only testing micro-framework](#testing)

## Collections

### Vector

[vector.h](collections/vector.h), [vector_test.c](collections/vector_test.c)

_Interface:_

 * `Vector_Of(TValue)` - construct a vector type;
 * `Vector_Free(VecPtr)` - free a vector;
 * `Vector_PushBack(VecPtr, Val)` - append an element;
 * `Vector_TryPopBack(VecPtr, ValuePtr)` - copy last value to
location pointed to by `ValuePtr`, decrease `Size` by `1` and 
return `true` if vector contains elements, and `false` otherwise;
 * `Vector_At(Vec, Index) -> TValue*` - return pointer to element at `Index`:
   * Element at `Index` if `0 <= Index < Size`;
   * Element at `Size - Index` if `-Size <= Index < 0` (`Index`th element from the end);
   * `NULL` otherwise.
 * `Vector_ForEach(ValuePtr, Vec)` - generates a `for` header to
 iterate over vector elements;
 * `Vector_Empty(Vec) -> bool` - `true` iff vector is empty;
 * `Vector_Clear(VecPtr)` - set `Size` to `0`;
 * `Vector_Any(Vec, Value, PredicateExpr) -> bool` - `true` iff `PredicateExpr`
is true for any element;
 * `Vector_Slice(TSlice, Vec, Start, End) -> TSlice`, 
`Vector_SliceFrom(TSlice, Vec, Start) -> TSlice`,
`Vector_SliceTo(TSlice, Vec, End) -> TSlice` - get a mutable view
of sub-vector.

Simple dynamic array of type `T`. Any struct with fields `size_t Size`, 
`size_t Capacity` and `T *Items` is compatible with `Vector_*` operations.

Macro `Vector_Of(T)` expands into definition of an
anonymous struct with fields specified above.

Usage example:
```c
Vector_Of(char const *) words = {0};

Vector_PushBack(&words, "A");
Vector_PushBack(&words, "quick");
Vector_PushBack(&words, "brown");
Vector_PushBack(&words, "fox");

Vector_ForEach(valuePtr, words) {
    printf("%s\n", *valuePtr);
}

char const *word;
while (Vector_TryPopBack(&words, &word)) {
    printf("%s\n", word);
}

printf("last=%s\n", *Vector_At(words, -1));
printf("contains fox? %s\n", Vector_Any(words, it, 0 == strcmp(it, "fox")) ? "yes" : "no");

Vector_Free(&words);
```

### Linked List

[list.h](collections/list.h), [list_test.c](collections/list_test.c)

_Interface:_

 * `List_Of(TValue)` - construct a list node type;
 * `List_Free(HeadPtrPtr)` - free a list;
 * `List_PushFront(HeadPtrPtr, Val)` - prepend a value;
 * `List_TryPopFront(HeadPtrPtr, ValuePtr) -> TValue` - returns `true`, 
copies the value in the first node to location pointed
to by `ValuePtr` and removes the first node if list is not empty,
returns `false` otherwise;
 * `List_PushBack(HeadPtrPtr, TailPtrPtr, Val)` - append an element;
 * `List_ForEach(ValuePtr, HeadPtr)` - generates a `for` header to
iterate over list elements;
 * `List_Tail(HeadPtr) -> typeof(HeadPtr)` - returns pointer
to last node;
 * `List_Size(HeadPtr) -> size_t` - count the number of elements
in the list;
 * `List_Empty(HeadPtr) -> bool` - returns `true` if list contains
   values, `false` otherwise.

A singly linked list. A node is a struct `NodeT` that 
has `T Value` and `NodeT *Next` field.

Macro `List_Of(T)` expands into a definition of a
suitable struct with a unique name.

Usage example:
```c
List_Of(int) *squares = NULL;

List_PushFront(&squares, 9);
List_PushFront(&squares, 4);
List_PushFront(&squares, 1);

typeof(squares) /* or auto since C23 */ tail = List_Tail(squares);
List_PushBack(&squares, &tail, 16);
List_PushBack(&squares, &tail, 25);

printf("Size=%zu\n", List_Size(squares));

List_ForEach(valuePtr, squares) {
    printf("%d\n", *valuePtr);
}

int square;
while (List_TryPopFront(&squares, &square)) {
    printf("%d\n", square);
}

List_Free(&squares);
```

### Map

[map.h](collections/map.h), [map_test.c](collections/map_test.c)

_Interface:_

 * `Map_Of(TKey, TValue)` - construct a map type;
 * `Map_Free(MapPtr)` - free a map;
 * `Map_Put(MapPtr, Key_, Value_)` - put a `Value_` at `Key_`;
 * `Map_At(Map, Key_) -> TValue*` - get a pointer to value at `Key_`
or `NULL` if `Key_` does not exist;
 * `Map_TryGet(Map, Key_, ValuePtr) -> bool` - if `Key_` is present,
puts the corresponding value to location `ValuePtr` points to and
returns `true`, otherwise returns `false`;
 * `Map_GetOrDefault(Map, Key_, DefaultExpr) -> TValue` - returns
the value at `Key_` if present, value of `DefaultExpr` otherwise;
 * `Map_ForEach(EntryPtr, Map)` - generates a `for` header to 
iterate over map entries.
 * `Map_Empty(Map) -> bool` - returns `true` if map contains
values, `false` otherwise.

A map (dictionary) with keys of type `TKey` and values of
type `TValue`. 

Use macro `Map_Of(TKey, TValue)` to get a definition for a 
suitable struct. 

When initialising a map, you must set `size_t (*Hash)(TKey)` 
and `bool (*KeyEquals)(TKey, TKey)` fields.

Usage example:
```c
Map_Of(char const *, size_t) frequencies = {
        .Hash = StrHash,
        .KeyEquals = StrEquals
};

char const *words[] = {
        "A", "quick", "brown", "dog", "jumps", "over", "a",
        "quick", "brown", "fox", NULL
};
char const **p = words;
while (NULL != *p) {
    Map_Put(&frequencies, *p, Map_GetOrDefault(frequencies, *p, 0));
}
/* or */
while (NULL != *p) {
    size_t freq;
    if (Map_TryGet(frequencies, *p, &freq)) {
        Map_Put(&frequencies, *p, freq + 1);
    } else {
        Map_Put(&frequencies, strdup(*p), 1);
    }
}

Map_ForEach(entry, frequencies) {
    printf("Key=%s Value=%zu\n", entry->Key, entry->Value);
}

Map_Free(&frequencies);
```

## Other

### Testing

[testing.h](testing/testing.h)

Minimal testing library. 

Example usage ([testing_demo.c](testing/testing_demo.c)):
```c
#define TEST_FILE_NAME_ONLY
#define TEST_SUCCESS_STREAM stdout
#define TEST_FAILURE_STREAM stdout

#include "testing.h"

#include <string.h>

Testing_Fact(one_plus_one_equals_two) {
    // arrange
    int a = 1, b = 1;

    // act
    int c = a + b;

    // assert
    Testing_Assert(c == 2, "math does not work");
}

Testing_Fact(always_false) {
    int a = 1, b = 2;

    Testing_Assert(a == a, "expected %s=%d to be equal %s=%d", Testing_Nameof(a), a, Testing_Nameof(a), a);
    Testing_Assert(a == b, "expected %s=%d to be equal %s=%d", Testing_Nameof(a), a, Testing_Nameof(b), b);
}

Testing_Fact(strlen_returns_correct_length) {
    const char *s = "abc";
    const size_t expected = 3;

    const size_t length = strlen(s);

    Testing_Assert(length == expected, "expected length to be %zu but was %zu", expected, length);
}

Testing_AllTests = {
        Testing_AddTest(one_plus_one_equals_two),
        Testing_AddTest(always_false),
        Testing_AddTest(strlen_returns_correct_length),
};

Testing_RunAllTests();
```

Example output:
```
[TEST 01] PASSED one_plus_one_equals_two
[TEST 02] FAILED always_false
testing_demo.c:24: Assertion 'a == b' failed, expected a=1 to be equal b=2
[TEST 03] PASSED strlen_returns_correct_length

3 tests, 2 passed, 1 failed
```
