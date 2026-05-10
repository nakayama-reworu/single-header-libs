# Collections

* [span.h - View over contiguous region of memory](#span)
* [vector.h - Dynamic array](#vector)
* [map.h - Collection of key-value pairs](#map)
* [list.h - Singly linked list](#list)

## Span

[span.h](span.h), [span_test.c](span_test.c)

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

## Map

## List
