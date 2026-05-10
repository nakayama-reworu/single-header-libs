#ifndef STRING_BUILDER
#define STRING_BUILDER

#include <stdlib.h>
#include <assert.h>

typedef struct StringBuilder StringBuilder;
struct StringBuilder {
    char *Chars;
    size_t CurrentLength;
    size_t MaxLength;
};

#define StringBuilder_Empty() ((StringBuilder) {0})

void StringBuilder_Free(StringBuilder *);

void StringBuilder_Sprintf(StringBuilder builder[static 1], char const *format, ...);

void StringBuilder_Append(StringBuilder builder[static 1], char const *src);

void StringBuilder_AppendChars(StringBuilder builder[static 1], size_t count, char const src[static count]);

char *StringBuilder_ToCStr(StringBuilder builder);

#endif // STRING_BUILDER

#ifdef STRING_BUILDER_IMPLEMENTATION

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define STRING_BUILDER__CallChecked(Callee, ArgsList)    \
({                                          \
    errno = 0;                              \
    __auto_type _r = Callee ArgsList;       \
    if (errno) {                            \
        fprintf(                            \
            stderr, "[%s:%d] %s%s: %s\n",   \
            __FILE_NAME__, __LINE__,        \
            #Callee, #ArgsList,             \
            strerror(errno)                 \
        );                                  \
        exit(EXIT_FAILURE);                 \
    }                                       \
    _r;                                     \
})

void StringBuilder_Free(StringBuilder *builder) {
    if (NULL == builder) { return; }

    free(builder->Chars);
    *builder = StringBuilder_Empty();
}

void STRING_BUILDER__ReserveToFit(StringBuilder builder[static 1], size_t strLen) {
    size_t const newMaxLen = builder->CurrentLength + strLen;
    if (newMaxLen <= builder->MaxLength) {
        return;
    }
    builder->Chars =
            STRING_BUILDER__CallChecked(realloc, (builder->Chars, (1 + newMaxLen) * sizeof(char)));
    builder->MaxLength = newMaxLen;
}

void StringBuilder_Sprintf(StringBuilder builder[static 1], char const *format, ...) {
    va_list varargs;

    va_start(varargs, format);
    int const formattedStringLength =
            STRING_BUILDER__CallChecked(vsnprintf, (NULL, 0, format, varargs));
    va_end(varargs);

    STRING_BUILDER__ReserveToFit(builder, formattedStringLength);

    va_start(varargs, format);
    STRING_BUILDER__CallChecked(
            vsnprintf,
            (builder->Chars + builder->CurrentLength, 1 + formattedStringLength, format, varargs)
    );
    va_end(varargs);

    builder->CurrentLength += formattedStringLength;
    builder->Chars[builder->CurrentLength] = '\0';
}

void StringBuilder_Append(StringBuilder builder[static 1], char const *src) {
    StringBuilder_AppendChars(builder, strlen(src), src);
}

void StringBuilder_AppendChars(StringBuilder builder[static 1], size_t count, char const src[static count]) {
    STRING_BUILDER__ReserveToFit(builder, count);

    memcpy(builder->Chars + builder->CurrentLength, src, count);

    builder->CurrentLength += count;
    builder->Chars[builder->CurrentLength] = '\0';
}

char *StringBuilder_ToCStr(StringBuilder builder) {
    if (NULL == builder.Chars) {
        return strdup("");
    }

    return strdup(builder.Chars);
}

#endif // STRING_BUILDER_IMPLEMENTATION
