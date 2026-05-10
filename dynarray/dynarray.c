#include "dynarray.h"


typedef struct {
    size_t ElementSizeBytes;
    size_t Size;
    size_t Capacity;
} ArrayHeader;

#define IMPL_ARRAY_HEADER(Array) (((ArrayHeader *) Array) - 1)

#define IMPL_ARRAY_GROW_FACTOR 2

void *ArrayNew(const size_t size, const size_t element_size_bytes) {
    size_t capacity = size > ARRAY_MIN_CAPACITY ? size : ARRAY_MIN_CAPACITY;

    const size_t total_size_bytes = capacity * element_size_bytes + sizeof(ArrayHeader);
    ArrayHeader *data;
    if (NULL == (data = (ArrayHeader *) malloc(total_size_bytes))) {
        perror(NAME_OF(ArrayNew));
        return NULL;
    }

    data[0].ElementSizeBytes = element_size_bytes;
    data[0].Size = size;
    data[0].Capacity = capacity;

    return (void *) (data + 1);
}


void ArrayFree(void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return;
    }

    free((void *) IMPL_ARRAY_HEADER(array));
}


size_t ArrayElementSize(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->ElementSizeBytes;
}


size_t ArraySize(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Size;
}


bool ArrayIsEmpty(const void *array) {
    return 0 == ArraySize(array);
}


size_t ArrayCapacity(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Capacity;
}


void *ArrayReserveToFit(void *array, size_t size) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    if (ArrayCapacity(array) >= size) {
        return array;
    }

    size_t new_capacity = ArrayCapacity(array);
    while (new_capacity < size) {
        new_capacity *= IMPL_ARRAY_GROW_FACTOR;
    }

    if (new_capacity < size) {
        LOG_ERROR(
                NAME_OF(new_capacity) "=%d is not enough for " NAME_OF(size) "=%d elements",
                (int) new_capacity, (int) size
        );
        return NULL;
    }
    assert(new_capacity >= size);

    LOG_DEBUG(NAME_OF(new_capacity) "=%d", (int) new_capacity);

    void *new_array;
    if (NULL == (new_array = ArrayNew(new_capacity, ArrayElementSize(array)))) {
        perror(NAME_OF(ArrayReserveToFit));
        return NULL;
    }

    memcpy(new_array, array, ArraySize(array) * ArrayElementSize(array));
    IMPL_ARRAY_HEADER(new_array)->Size = ArraySize(array);

    ArrayFree(array);

    return new_array;
}


void *ArrayAppend(void *array, const void *element) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    const size_t old_size = ArraySize(array);
    const size_t old_capacity = ArrayCapacity(array);

    const size_t new_size = old_size + 1;

    if (new_size > old_capacity) {
        if (NULL == (array = ArrayReserveToFit(array, new_size))) {
            LOG_ERROR("Failed to increase array capacity");
            return NULL;
        }
    }

    IMPL_ARRAY_HEADER(array)->Size = new_size;
    memcpy(ARRAY_AT(array, old_size), element, ArrayElementSize(array));

    LOG_DEBUG(
            NAME_OF(old_size) "=%d, " NAME_OF(new_size) "=%d, "
            NAME_OF(old_capacity) "=%d, " NAME_OF(new_capacity) "=%d",
            (int) old_size, (int) new_size, (int) old_capacity, (int) ArrayCapacity(array)
    );

    return array;
}


void *ArrayExtendWithValues(
        void *array,
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    if (ArrayElementSize(array) != element_size) {
        LOG_ERROR(
                NAME_OF(element_size) "=%d is different from array element size (%d)",
                (int) element_size, (int) ArrayElementSize(array)
        );
        return NULL;
    }

    const size_t original_size = ArraySize(array);
    if (NULL == (array = ArrayReserveToFit(array, original_size + elements_count))) {
        LOG_ERROR("Failed to increase capacity");
        return NULL;
    }

    for (size_t i = 0; i < elements_count; i++) {
        array = ArrayAppend(array, (uint8_t *) data + i * element_size);
    }

    assert(ArraySize(array) == original_size + elements_count);

    return array;
}


void *ArrayExtend(void *array, const void *other) {
    return ArrayExtendWithValues(array, other, ArraySize(other), ArrayElementSize(other));
}


void *ArrayOfValues(
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    void *array = ArrayNew(elements_count, element_size);
    IMPL_ARRAY_HEADER(array)->Size = 0;  // ArrayExtendWithValues will work properly, space already reserved
    return ArrayExtendWithValues(array, data, elements_count, element_size);
}


bool ArrayPop(void *array, void *dst) {
    if (NULL == array) {
        LOG_NULL(array);
        return false;
    }

    if (ArrayIsEmpty(array)) {
        LOG_ERROR("Array is empty");
        return false;
    }

    IMPL_ARRAY_HEADER(array)->Size -= 1;

    if (NULL == dst) {
        return true;
    }

    const void *src = ARRAY_AT(array, ArraySize(array));
    memcpy(dst, src, ArrayElementSize(array));

    return true;
}
