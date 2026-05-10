# Collections

* [Span](#span)
* [Vector](#vector)
* [Map](#map)
* [List](#list)

## Span

[span.h](span.h), [span_test.c](span_test.c)

A view over contiguous region of memory.

### Type constructors

* [Span](#span-1)
* [ReadOnlySpan](#readonlyspan)

#### Span
```c
#define Span(Type)  \
struct {            \
    Type *Items;    \
    size_t Size;    \
}
```

#### ReadOnlySpan
```c
#define ReadOnlySpan(Type)  \
struct {                    \
    Type const *Items;      \
    size_t Size;            \
}
```

### Functions

* [Span_Empty](#span_empty)
* [Span_FromPtr](#span_fromptr)
* [Span_FromArray](#span_fromarray)
* [Span_From](#span_from)
* [Span_Of](#span_of)
* [Span_Slice](#span_slice)
* [Span_SliceFrom](#span_slicefrom)
* [Span_SliceTo](#span_sliceto)
* [Span_IsEmpty](#span_isempty)
* [Span_At](#span_at)
* [Span_ForEach](#span_foreach)

#### Span_Empty
```c
#define Span_Empty(SpanType)
```
Returns an empty span of type `SpanType`.

#### Span_FromPtr
```c
#define Span_FromPtr(SpanType, Ptr, Count)
```
Construct a span of type `SpanType` from memory address.

#### Span_FromArray
```c
#define Span_FromArray(SpanType, Array)
```
Construct a span of type `SpanType` from automatic array
or array literal.

#### Span_From
```c
#define Span_From(SpanType, Src)
```
Construct a span of type `SpanType` from `Src.Items` and 
`Src.Size`.

#### Span_Of
```c
#define Span_Of(SpanType, ...)
```
Construct a span of type `SpanType` with given elements.

Example:
```c
IntSpan span = Span_Of(IntSpan, 1, 2, 3, 4, 5);
```

#### Span_Slice
```c
#define Span_Slice(SpanType, Src, StartInclusive, EndExclusive)
```
Construct a span of type `SpanType` containing 
elements from `Src` from `StartInclusive` to
`EndExclusive`, not including `EndExclusive`.

Both `StartInclusive` and `EndExclusive` can be negative
in range from `-Src.Size` to `-1`. Negative indices are treated
as offsets from the end of `Src`, i.e. `-Src.Size` is `0` and
`-1` is `Src.Size - 1`.

#### Span_SliceFrom
```c
#define Span_SliceFrom(SpanType, Src, StartInclusive)
```
Wraps `Span_Slice` with `EndExclusive` set to `Src.Size`.

#### Span_SliceTo
```c
#define Span_SliceTo(SpanType, Src, EndExclusive)
```
Wraps `Span_Slice` with `StartInclusive` set to `0`.

#### Span_IsEmpty
```c
#define Span_IsEmpty(Span_)
```
Returns true iff `Span_` contains no elements.

#### Span_At
```c
#define Span_At(Span_, Index)
```
Returns a pointer to element at `Index` or `NULL` if `Index`
is not valid for `Span_`.

`Index` can be negative in range from `-Src.Size` to `-1`. 
Negative indices are treated as offsets 
from the end of `Src`, i.e. `-Src.Size` 
is `0` and `-1` is `Src.Size - 1`.

If `Index` if not in range from `-Src.Size` to `Src.Size - 1`,
`NULL` is returned.

#### Span_ForEach
```c
#define Span_ForEach(ValuePtr, Span_)
```
Expands into `for`-loop header that would iterate over 
elements of `Span_`.

Example:
```c
typedef ReadOnlySpan(int) IntConstSpan;

IntConstSpan const nums = Span_FromArray(IntConstSpan, ((int[]) {1, 2, 3, 4, 5, 6}));

Span_ForEach(it, nums) {
    printf("%d\n", *it);
}
```

## Vector

[vector.h](vector.h), [vector_test.c](vector_test.c)

A dynamic array.

### Type constructors

* [Vector](#vector-1)

#### Vector
```c
#define Vector(TValue)  \
struct {                \
    size_t Size;        \
    size_t Capacity;    \
    TValue *Items;      \
}
```

### Functions

* [Vector_Empty](#vector_empty)
* [Vector_FromPtr](#vector_fromptr)
* [Vector_FromArray](#vector_fromarray)
* [Vector_From](#vector_from)
* [Vector_Of](#vector_of)
* [Vector_PushBack](#vector_pushback)
* [Vector_TryPopBack](#vector_trypopback)
* [Vector_Reverse](#vector_reverse)
* [Vector_IsEmpty](#vector_isempty)
* [Vector_At](#vector_at)
* [Vector_Clear](#vector_clear)

#### Vector_Empty
```c
#define Vector_Empty(VectorType)
```
Construct an empty vector of type `VectorType`.

#### Vector_FromPtr
```c
#define Vector_FromPtr(VectorType, Ptr, Count)
```
Construct a vector of type `VectorType` and copy `Count` elements
from `Ptr`.

#### Vector_FromArray
```c
#define Vector_FromArray(VectorType, Array)
```
Construct a vector of type `VectorType` and elements of `Array`. `Array` must
be an automatic array or an array literal. 

#### Vector_From
```c
#define Vector_From(VectorType, Src)
```
Construct a vector of type `VectorType` and copy `Src.Size` elements 
from `Src,Items`. Can be used to make a copy of a vector.

#### Vector_Of
```c
Vector_Of(VectorType, ...)
```
Construct a vector of type `VectorType` with given elements.

#### Vector_Free
```c
#define Vector_Free(VecPtr)
```
Free a vector.

#### Vector_Reserve
```c
#define Vector_Reserve(VecPtr, NewCapacity)
```
Preallocate enough space to store `NewCapacity` elements.
Does nothing if `NewCapacity` is less of equal to
`VecPtr->Capacity`.

#### Vector_PushBack
```c
#define Vector_PushBack(VecPtr, Val)
```
Append a value.

#### Vector_TryPopBack
```c
#define Vector_TryPopBack(VecPtr, ValuePtr)
```
If vector at `VecPtr` contains elements, copy last element to 
`ValuePtr`, decrement `Size` and return `true`. If vector at `VecPtr`
is empty, return `false`.

#### Vector_Reverse
```c
#define Vector_Reverse(VecPtr)
```
Reverse the order of elements in a vector.

#### Vector_IsEmpty
```c
#define Vector_IsEmpty(Vec)
```
Returns `true` iff `Vec` contains no elements.

#### Vector_At
```c
#define Vector_At(Vec, Index)
```
See [Span_At](#span_at).

#### Vector_Clear
```c
#define Vector_Clear(VecPtr)
```
Set `VecPtr->Size` to `0`. Does not free any memory.

## Map

[map.h](map.h), [map_test.c](map_test.c)

A collections of key-value pairs.

### Type constructors

* [Map](#map-1)

#### Map
```c
#define Map(TKey, TValue)           \
struct {                            \
    size_t Size;                    \
    size_t Capacity;                \
    size_t (*Hash)(TKey);           \
    bool (*KeyEquals)(TKey, TKey);  \
    struct {                        \
        TKey Key;                   \
        TValue Value;               \
        bool Used;                  \
    } *Entries;                     \
}
```

### Functions

* [Map_Empty](#map_empty)
* [Map_Of](#map_of)
* [Map_Free](#map_free)
* [Map_Put](#map_put)
* [Map_At](#map_at)
* [Map_TryGet](#map_tryget)
* [Map_GetOrDefault](#map_getordefault)
* [Map_ForEach](#map_foreach)
* [Map_IsEmpty](#map_isempty)

#### Map_Empty
```c
#define Map_Empty(MapType, Hash_, KeyEquals_)
```
Return an empty map of type `MapType`, with hash function `Hash_` and 
key equality function `KeyEquals_`.

`Hash_` must have the following signature:
```c
size_t (*)(TKey)
```
`KeyEquals_` must have the following signature:
```c
bool (*)(TKey, TKey)
```

#### Map_Of
```c
#define Map_Of(MapType, Hash_, KeyEquals_, ...)
```
Construct a map of type `MapType`, with hash function `Hash_` and
key equality function `KeyEquals_`, and given key-value pairs.

Example:
```c
IntIntMap map = Map_Of(
        IntIntMap, IntHash, IntEquals,
        { .Key = 1, .Value = 2 },
        { .Key = 2, .Value = 7 },
        { .Key = 3, .Value = -42 },
);
```

#### Map_Free
```c
#define Map_Free(MapPtr)
```
Free a map at `MapPtr` and set it to:
```c
Map_Empty(typeof(*MapPtr), MapPtr->Hash, MapPtr->KeyEquals)
```

#### Map_Put
```c
#define Map_Put(MapPtr, Key_, Value_)
```
Insert a key-value pair if `Key_` is not present, 
update existing value otherwise.

Existing key is never reassigned.

#### Map_At
```c
#define Map_At(Map_, Key_)
```
Return a pointer to value associated with `Key_` or `NULL` if 
`Key_` is not present.

#### Map_TryGet
```c
#define Map_TryGet(Map_, Key_, ValuePtr)
```
If `Key_` is present, assign existing value to `*ValuePtr` 
and return `true`; return `false` otherwise.

#### Map_GetOrDefault
```c
#define Map_GetOrDefault(Map_, Key_, DefaultExpr)
```
Return a value associated with `Key_` if it is present, return the 
value of `DefaultExpr` otherwise. 

`DefaultExpr` is not evaluated if `Key_` exists.

#### Map_ForEach
```c
#define Map_ForEach(EntryPtr, Map_)
```
Expands into a `for` loop header that allows iterating over map entries.

#### Map_IsEmpty
```c
#define Map_IsEmpty(Map_)
```
Return `true` iff `Map_` contains no entries.

## List

[list.h](list.h), [list_test.c](list_test.c)

A singly linked list.

### Type constructors

_TODO_

### Functions

_TODO_
