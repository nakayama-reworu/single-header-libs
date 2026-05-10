#include "vector.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define print_error(failed_call)                \
fprintf(                                        \
    stderr,                                     \
    "%s:%d (%s) `%s` failed: %s (errno=%d)\n",  \
    __FILE_NAME__, __LINE__, __FUNCTION__,      \
    ({ (void) failed_call; #failed_call; }),    \
    strerror(errno), errno                      \
)

#ifdef assert
#undef assert
#endif
#define assert_always(expr)                         \
do {                                                \
    if (expr) { break; }                            \
    fprintf(                                        \
        stderr,                                     \
        "%s:%d (%s) Assertion failed: `" #expr "`", \
        __FILE_NAME__, __LINE__, __FUNCTION__       \
    );                                              \
    exit(EXIT_FAILURE);                             \
} while (0)

#define on_malloc_error() exit(EXIT_FAILURE)

typedef struct {
    size_t size;
    size_t capacity;
    size_t element_size;
    uint8_t data[];
} Header;

#define max(a, b)           \
({                          \
    __auto_type _a = (a);   \
    __auto_type _b = (b);   \
    _a > _b ? _a : _b;      \
})

static Header *vector_header(void *data) { return (Header *) data - 1; }

static const Header *vector_header_const(const void *data) { return (const Header *) data - 1; }

void *vector_new(size_t element_size, size_t elements_count) {
    const size_t capacity = max((size_t) 1, elements_count);
    const size_t total_size = sizeof(Header) + capacity * element_size;

    Header *header = calloc(1, total_size);
    if (NULL == header) {
        print_error(calloc);
        on_malloc_error();
    }

    header->size = elements_count;
    header->capacity = capacity;
    header->element_size = element_size;

    return header->data;
}

void vector_free_(void *vector) {
    assert_always(NULL != vector);
    free(vector_header(vector));
}

#define vector_at(header, index) ((header)->data + (header)->element_size * (index))

const void *vector_end_(const void *vector) {
    assert_always(NULL != vector);

    const Header *header = vector_header_const(vector);
    return vector_at(header, header->size);
}

size_t vector_size(const void *vector) {
    assert_always(NULL != vector);
    return vector_header_const(vector)->size;
}

bool vector_empty(const void *vector) {
    assert_always(NULL != vector);
    return 0 == vector_size(vector);
}

static void vector_set_from(Header *header, size_t index, const void *value) {
    assert_always(index < header->size);

    memcpy(vector_at(header, index), value, header->element_size);
}

static void vector_get_to(const Header *header, size_t index, void *dst) {
    assert_always(index < header->size);

    memcpy(dst, vector_at(header, index), header->element_size);
}

static Header *vector_realloc(Header *header, size_t new_capacity) {
    const size_t new_total_size = sizeof(Header) + new_capacity * header->element_size;
    Header *new_header = realloc(header, new_total_size);
    if (NULL == new_header) {
        print_error(realloc);
        on_malloc_error();
    }

    new_header->capacity = new_capacity;
    return new_header;
}

static size_t increased_capacity(size_t capacity) {
    const size_t new_capacity = capacity * 3 / 2 + 1;
    assert_always(new_capacity > capacity);
    return new_capacity;
}

void *vector_with_appended_from(void *vector, const void *value) {
    assert_always(NULL != vector);
    assert_always(NULL != value);

    Header *header = vector_header(vector);
    if (header->size == header->capacity) {
        const size_t new_capacity = increased_capacity(header->capacity);
        if (NULL == (header = vector_realloc(header, new_capacity))) {
            return NULL;
        }
    }

    header->size++;
    vector_set_from(header, header->size - 1, value);

    return vector;
}

void *vector_with_range_appended_from(void *vector, const void *src, size_t count) {
    assert_always(NULL != vector);
    assert_always(NULL != src);

    Header *header = vector_header(vector);
    const size_t total_size = header->size + count;
    if (total_size > header->capacity) {
        size_t new_capacity = header->capacity;
        while (new_capacity < total_size) {
            new_capacity = increased_capacity(new_capacity);
        }

        if (NULL == (header = vector_realloc(header, new_capacity))) {
            return NULL;
        }
    }

    const size_t begin_index = header->size;
    header->size = total_size;
    for (size_t src_index = 0, dst_index = begin_index; src_index < count; src_index++, dst_index++) {
        vector_set_from(header, dst_index, (const uint8_t *) src + src_index * header->element_size);
    }

    return header->data;
}

bool vector_pop_to(void *vector, void *dst) {
    assert_always(NULL != vector);
    assert_always(NULL != dst);

    Header *header = vector_header(vector);
    if (0 == header->size) {
        return false;
    }

    vector_get_to(header, header->size - 1, dst);
    header->size--;

    return true;
}
