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

* [Span_Empty](#spanempty)
* [Span_FromPtr](#spanfromptr)
* [Span_FromArray](#spanfromarray)
* [Span_From](#spanfrom)
* [Span_Of](#spanof)
* [Span_Slice](#spanslice)
* [Span_SliceFrom](#spanslicefrom)
* [Span_SliceTo](#spansliceto)
* [Span_IsEmpty](#spanisempty)
* [Span_At](#spanat)
* [Span_ForEach](#spanforeach)

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

* [Vector_Empty](#vectorempty)
* [Vector_FromPtr](#vectorfromptr)
* [Vector_FromArray](#vectorfromarray)
* [Vector_From](#vectorfrom)
* [Vector_Of](#vectorof)
* [Vector_PushBack](#vectorpushback)
* [Vector_TryPopBack](#vectortrypopback)
* [Vector_IsEmpty](#vectorisempty)
* [Vector_Clear](#vectorclear)

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

#### Vector_IsEmpty
```c
#define Vector_IsEmpty(Vec)
```
Returns `true` iff `Vec` contains no elements.

#### Vector_Clear
```c
#define Vector_Clear(VecPtr)
```
Set `VecPtr->Size` to `0`. Does not free any memory.

## Map

A collections of key-value pairs.

## List

A singly linked list.
