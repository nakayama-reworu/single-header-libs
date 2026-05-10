#ifndef SPAN_H
#define SPAN_H

#include <stdlib.h>
#include <stdbool.h>

#define SPAN__Concat_(A, B)   A ## B
#define SPAN__Concat(A, B)    SPAN__Concat_(A, B)

#define SPAN__ArrayLength(Array) (sizeof(Array) / sizeof(*(Array)))

#define Span(Type)  \
struct {            \
    Type *Items;    \
    size_t Size;    \
}

#define ReadOnlySpan(Type)  \
struct {                    \
    Type const *Items;      \
    size_t Size;            \
}

#define Span_Empty(SpanType) ((SpanType) {0})

#define Span_FromPtr(SpanType, Ptr, Count) ((SpanType) {.Items = (Ptr), .Size = (Count)})

#define Span_FromArray(SpanType, Array) Span_FromPtr(SpanType, (Array), SPAN__ArrayLength(Array))

#define SPAN__Min(A, B)     \
({                          \
    __auto_type _a = (A);   \
    __auto_type _b = (B);   \
    _a < _b ? _a : _b;      \
})

#define SPAN__Max(A, B)     \
({                          \
    __auto_type _a = (A);   \
    __auto_type _b = (B);   \
    _a > _b ? _a : _b;      \
})

#define SPAN__ResolveBoundary(Size, Index)          \
({                                                  \
    long long _i = (Index);                         \
    size_t _sz = (Size);                            \
                                                    \
    if (_i < 0) {                                   \
        _i = SPAN__Max(0, _i + (long long) _sz);    \
    }                                               \
    _i = SPAN__Min(_sz, (size_t) _i);               \
    (size_t) _i;                                    \
})

#define Span_Slice(SpanType, Src, StartInclusive, EndExclusive)                 \
({                                                                              \
    __auto_type _src_take = (Src);                                              \
    size_t _start = SPAN__ResolveBoundary(_src_take.Size, (StartInclusive));    \
    size_t _end = SPAN__ResolveBoundary(_src_take.Size, (EndExclusive));        \
                                                                                \
    size_t _size = (_start <= _end) ? (_end - _start) : 0;                      \
    Span_FromPtr(SpanType, _src_take.Items + _start, _size);                    \
})

#define Span_SliceFrom(SpanType, Src, StartInclusive)                           \
({                                                                              \
    __auto_type _src_spanType = (Src);                                          \
    Span_Slice(SpanType, _src_spanType, (StartInclusive), _src_spanType.Size);  \
})

#define Span_SliceTo(SpanType, Src, EndExclusive) Span_Slice(SpanType, (Src), 0, (EndExclusive))

#define Span_IsEmpty(Span_) (0 == (Span_).Size)

#define Span_At(Span_, Index)                   \
({                                              \
    __auto_type _span_at = (Span_);             \
    long long _idx = (Index);                   \
    long long _sz = _span_at.Size;              \
    typeof(_span_at.Items[0]) *_value = NULL;   \
    if (0 <= _idx && _idx < _sz) {              \
        _value = _span_at.Items + _idx;         \
    }                                           \
    if (-_sz <= _idx && _idx < 0) {             \
        _value = _span_at.Items + _sz + _idx;   \
    }                                           \
    _value;                                     \
})

#define Span_ForEach(ValuePtr, Span_)                                                           \
size_t SPAN__Concat(_it_, __LINE__) = 0;                                                        \
__auto_type SPAN__Concat(_span_forEach_, __LINE__) = (Span_);                                   \
for (                                                                                           \
    typeof(*(SPAN__Concat(_span_forEach_, __LINE__).Items)) *ValuePtr =                         \
        Span_At(SPAN__Concat(_span_forEach_, __LINE__), 0);                                     \
    SPAN__Concat(_it_, __LINE__) < SPAN__Concat(_span_forEach_, __LINE__).Size;                 \
    SPAN__Concat(_it_, __LINE__)++,                                                             \
    ValuePtr = Span_At(SPAN__Concat(_span_forEach_, __LINE__), SPAN__Concat(_it_, __LINE__))    \
)

#endif // SPAN_H
