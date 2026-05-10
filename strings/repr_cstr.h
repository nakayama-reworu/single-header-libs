#ifndef REPR_CSTR
#define REPR_CSTR

#include <stdio.h>
#include <stdbool.h>

int CStr_ReprSize(char const *str);

bool CStr_ReprToString(char *dst, size_t dstCapacity, char const *str);

void CStr_ReprToFile(FILE file[static 1], char const *str);

#endif // REPR_CSTR

#ifdef REPR_CSTR_IMPLEMENTATION

#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define REPR_CSTR__CallChecked(Callee, ArgsList)    \
({                                              \
    errno = 0;                                  \
    __auto_type _r = Callee ArgsList;           \
    if (errno) {                                \
        fprintf(                                \
            stderr, "[%s:%d] %s%s: %s\n",       \
            __FILE_NAME__, __LINE__,            \
            #Callee, #ArgsList,                 \
            strerror(errno)                     \
        );                                      \
        exit(EXIT_FAILURE);                     \
    }                                           \
    _r;                                         \
})

char const *REPR_CSTR_ESCAPE_SEQUENCES[256] = {
        ['\a'] = "\\a",
        ['\b'] = "\\b",
        ['\f'] = "\\f",
        ['\n'] = "\\n",
        ['\r'] = "\\r",
        ['\t'] = "\\t",
        ['\v'] = "\\v",
        ['\\'] = "\\\\",
        ['\"'] = "\\\"",
};

int CStr_ReprSize(char const *str) {
    if (NULL == str) {
        return 0;
    }

    int size = 2;
    int charSize;

    for (unsigned char c = *str; '\0' != *str; str++, c = *str) {
        char const *escapeSequence;
        if (NULL != (escapeSequence = REPR_CSTR_ESCAPE_SEQUENCES[c])) {
            if ((charSize = snprintf(NULL, 0, "%s", escapeSequence)) < 0) {
                return charSize;
            }
            size += charSize;
            continue;
        }

        if (isprint(c)) {
            size += 1;
            continue;
        }

        if ((charSize = snprintf(NULL, 0, "\\x%02hhx", c)) < 0) {
            return charSize;
        }
        size += charSize;
    }

    return size;
}

#define REPR_CSTR_MIN_CAPACITY ((size_t) 3)

#define REPR_CSTR__SnprintfShiftChecked(DstPtr, SizePtr, Format, ...)       \
({                                                                          \
    int _printed = snprintf(*(DstPtr), *(SizePtr), Format, ##__VA_ARGS__);  \
    bool _ok = false;                                                       \
    if (_printed >= 0 && (size_t) _printed < *(SizePtr)) {                  \
        _ok = true;                                                         \
        *(DstPtr) += _printed;                                              \
        *(SizePtr) -= _printed;                                             \
    }                                                                       \
    _ok;                                                                    \
})

bool CStr_ReprToString(char *dst, size_t dstCapacity, char const *str) {
    if (NULL == dst || NULL == str || dstCapacity < REPR_CSTR_MIN_CAPACITY) {
        return false;
    }

    if (false == REPR_CSTR__SnprintfShiftChecked(&dst, &dstCapacity, "\"")) {
        return false;
    }

    for (unsigned char c = *str; '\0' != *str; str++, c = *str) {
        if (NULL != REPR_CSTR_ESCAPE_SEQUENCES[c]) {
            if (false == REPR_CSTR__SnprintfShiftChecked(&dst, &dstCapacity, "%s", REPR_CSTR_ESCAPE_SEQUENCES[c])) {
                return false;
            }
            continue;
        }

        if (isprint(c)) {
            if (false == REPR_CSTR__SnprintfShiftChecked(&dst, &dstCapacity, "%c", c)) {
                return false;
            }
            continue;
        }

        if (false == REPR_CSTR__SnprintfShiftChecked(&dst, &dstCapacity, "\\x%02hhx", c)) {
            return false;
        }
    }

    return REPR_CSTR__SnprintfShiftChecked(&dst, &dstCapacity, "\"");
}

void CStr_ReprToFile(FILE file[static 1], char const *str) {
    if (NULL == str) {
        return;
    }

    REPR_CSTR__CallChecked(fprintf, (file, "\""));

    for (unsigned char c = *str; '\0' != *str; str++, c = *str) {
        char const *escapeSequence;
        if (NULL != (escapeSequence = REPR_CSTR_ESCAPE_SEQUENCES[c])) {
            REPR_CSTR__CallChecked(fprintf, (file, "%s", escapeSequence));
            continue;
        }

        if (isprint(c)) {
            REPR_CSTR__CallChecked(fputc, (c, file));
            continue;
        }

        REPR_CSTR__CallChecked(fprintf, (file, "\\x%02hhx", c));
    }

    REPR_CSTR__CallChecked(fprintf, (file, "\""));
}

#endif // REPR_CSTR_IMPLEMENTATION
