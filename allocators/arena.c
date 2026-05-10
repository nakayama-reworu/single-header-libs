#include <stdio.h>

#include "call_checked.h"
#include "arena.h"

struct ArenaRegion {
    ArenaRegion *Next;

    uint8_t *Current;
    uint8_t *End;

    uint8_t Data[];
};

#define ARENA_REGION_DEFAULT_CAPACITY   ((size_t) (4 * 1024))

ArenaRegion *ArenaRegion_New(size_t capacity, ArenaRegion *next) {
    printf("ArenaRegion_New - creating a region with capacity %zu bytes\n", capacity);
    ArenaRegion *region = CallChecked(calloc, (1, sizeof(ArenaRegion) + capacity));
    *region = (ArenaRegion) {
            .Current = region->Data,
            .End = region->Data + capacity,
            .Next = next,
    };

    return region;
}

void ArenaRegion_Free(ArenaRegion *region) {
    printf(
            "ArenaRegion_Free - freeing a region with size=%td bytes and capacity=%td bytes\n",
            region->Current - region->Data,
            region->End - region->Data
    );
    free(region);
}

#define ARENA_MAX(A, B)     \
({                          \
    __auto_type _a = (A);   \
    __auto_type _b = (B);   \
    _a > _b ? _a : _b;      \
})

#define ARENA_ALIGN(Ptr, Alignment)                                                 \
({                                                                                  \
    __auto_type _alignment = (Alignment);                                           \
    (uint8_t *) (((intptr_t) (Ptr) + _alignment - 1) / _alignment * _alignment);    \
})

void *Arena_Allocate(ArenaAllocator allocator[static 1], size_t size, size_t alignment) {
    if (0 == size) {
        return NULL;
    }

    if (0 == alignment) {
        alignment = 1;
    }

    ArenaRegion *region = allocator->Region;
    if (NULL == region || ARENA_ALIGN(region->Current, alignment) + size >= region->End) {
        allocator->Region = region =
                ArenaRegion_New(ARENA_MAX(size + alignment - 1, ARENA_REGION_DEFAULT_CAPACITY), region);
    }

    uint8_t *const result = ARENA_ALIGN(region->Current, alignment);
    region->Current = result + size;
    return result;
}

void Arena_Free(ArenaAllocator allocator[1]) {
    ArenaRegion *region = allocator->Region;
    while (NULL != region) {
        ArenaRegion *next = region->Next;
        ArenaRegion_Free(region);
        region = next;
    }

    *allocator = Arena_Empty();
}
