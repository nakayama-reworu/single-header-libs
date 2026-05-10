#ifndef PLAYGROUND_MACROS_H
#define PLAYGROUND_MACROS_H

#include <stdio.h>

#define CONCAT_(x, y)   x ## y
#define CONCAT(x, y)    CONCAT_(x, y)

// #define nameof(it)              NAME_OF(it)
#define nameof_identifier(it)   ({ (void) it; #it; })

#define LOG(Level, Message, ...)                        \
do {                                                    \
    fprintf(                                            \
        stderr,                                         \
        "[" #Level "] [%s:%d] %s - " Message "\n",      \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__ \
    );                                                  \
    fflush(stderr);                                     \
} while (0)

#ifdef DEBUG
#define LOG_DEBUG(Message, ...) LOG(DEBUG, Message, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)  ((void) 0)
#endif

#define LOG_ERROR(Message, ...) LOG(ERROR, Message, ##__VA_ARGS__)
#define LOG_NULL(It)            LOG_ERROR("%s is NULL", nameof_identifier(It))

#define typeof_first(First, ...)    typeof(First)
#define sizeof_first(...)           sizeof(typeof_first(__VA_ARGS__))

#define ARRAY_LITERAL_FROM_VA_ARGS_(...)                ((typeof_first(__VA_ARGS__)[]) {__VA_ARGS__})
#define ARRAY_LITERAL_FROM_VA_ARGS_TYPE_(Type, ...)     ((Type[]) {__VA_ARGS__})

#define array_literal(...)                  ARRAY_LITERAL_FROM_VA_ARGS_(__VA_ARGS__)
#define array_literal_of_type(type, ...)    ARRAY_LITERAL_FROM_VA_ARGS_TYPE_(type, __VA_ARGS__)

#define array_literal_size(...) \
    (sizeof(array_literal(__VA_ARGS__)) / sizeof(typeof_first(__VA_ARGS__)))

#define array_literal_sized(...) \
    array_literal(__VA_ARGS__), array_literal_size(__VA_ARGS__)

#define array_literal_of_type_sized(type, ...) \
    array_literal_of_type(type, __VA_ARGS__), array_literal_size(__VA_ARGS__)

#define SHIFT(Ptr, OffsetBytes) ((void *) ((uint8_t *) Ptr + (OffsetBytes)))

#define typeof_member(Type, MemberName) typeof(((Type){0}).MemberName)
#define sizeof_member(Type, MemberName) sizeof(((Type){0}).MemberName)

#endif //PLAYGROUND_MACROS_H
