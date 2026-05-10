#include "repr_string.h"

#include <ctype.h>
#include <stdio.h>

#include "call_checked.h"

char const *ESCAPE_SEQUENCES[256] = {
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
        if (NULL != (escapeSequence = ESCAPE_SEQUENCES[c])) {
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

#define MIN_CAPACITY ((size_t) 3)

#define SnprintfShiftChecked(DstPtr, SizePtr, Format, ...)                  \
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
    if (NULL == dst || NULL == str || dstCapacity < MIN_CAPACITY) {
        return false;
    }

    if (false == SnprintfShiftChecked(&dst, &dstCapacity, "\"")) {
        return false;
    }

    for (unsigned char c = *str; '\0' != *str; str++, c = *str) {
        if (NULL != ESCAPE_SEQUENCES[c]) {
            if (false == SnprintfShiftChecked(&dst, &dstCapacity, "%s", ESCAPE_SEQUENCES[c])) {
                return false;
            }
            continue;
        }

        if (isprint(c)) {
            if (false == SnprintfShiftChecked(&dst, &dstCapacity, "%c", c)) {
                return false;
            }
            continue;
        }

        if (false == SnprintfShiftChecked(&dst, &dstCapacity, "\\x%02hhx", c)) {
            return false;
        }
    }

    return SnprintfShiftChecked(&dst, &dstCapacity, "\"");
}

void CStr_ReprToFile(FILE file[static 1], char const *str) {
    if (NULL == str) {
        return;
    }

    CallChecked(fprintf, (file, "\""));

    for (unsigned char c = *str; '\0' != *str; str++, c = *str) {
        char const *escapeSequence;
        if (NULL != (escapeSequence = ESCAPE_SEQUENCES[c])) {
            CallChecked(fprintf, (file, "%s", escapeSequence));
            continue;
        }

        if (isprint(c)) {
            CallChecked(fputc, (c, file));
            continue;
        }

        CallChecked(fprintf, (file, "\\x%02hhx", c));
    }

    CallChecked(fprintf, (file, "\""));
}
