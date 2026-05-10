#ifndef PLAYGROUND_MACROS_H
#define PLAYGROUND_MACROS_H

#include <stdio.h>

#define NAME_OF(It) #It

#define LOG(Level, Message, ...) fprintf( \
    stderr, \
    "[" #Level "] [%s:%d] %s - " Message "\n", \
    __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__ \
)

#ifdef DEBUG
#define LOG_DEBUG(Message, ...) LOG(DEBUG, Message, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)  ((void) 0)
#endif

#define LOG_ERROR(Message, ...) LOG(ERROR, Message, ##__VA_ARGS__)
#define LOG_NULL(It)            LOG_ERROR(NAME_OF(It) " is NULL")

#define TYPE_OF_FIRST(First, ...)                   typeof(First)
#define ARRAY_LITERAL_FROM_VA_ARGS(...)             ((TYPE_OF_FIRST(__VA_ARGS__)[]) {__VA_ARGS__})
#define ARRAY_LITERAL_FROM_VA_ARGS_TYPE(Type, ...)  ((Type[]) {__VA_ARGS__})

#define SHIFT(Ptr, OffsetBytes) ((void *) ((uint8_t *) Ptr + (OffsetBytes)))

#define TYPEOF_MEMBER(Type, MemberName) typeof(((Type){0}).MemberName)
#define SIZEOF_MEMBER(Type, MemberName) sizeof(((Type){0}).MemberName)

#endif //PLAYGROUND_MACROS_H
