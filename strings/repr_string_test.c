#include <string.h>
#include <stdio.h>

#include "repr_string.h"

#include "testing/testing.h"

Testing_Fact(CStr_ReprSize_returns_strlen_plus_two_for_string_with_no_special_chars) {
    char const *s = "abc1234!@#$%^&*()_+<>?,./";

    size_t expectedSize = strlen(s) + 2;
    int reprSize = CStr_ReprSize(s);
    Testing_Assert(reprSize > 0, "expected %s to return non-negative value", Testing_Nameof(CStr_ReprSize));
    Testing_Assert(expectedSize == (size_t) reprSize, "expected size to be %zu but was %d", expectedSize, reprSize);
}

Testing_Fact(CStr_ReprSize_returns_correct_size_for_string_with_special_chars) {
    char const *s = "abc\n\t123\a\xff";

    size_t expectedSize =
            1 // "
            + 3 // abc
            + 2 // \n
            + 2 // \t
            + 3 // 123
            + 2 // \a
            + 4 // \xff
            + 1; // "
    int reprSize = CStr_ReprSize(s);
    Testing_Assert(reprSize > 0, "expected %s to return non-negative value", Testing_Nameof(CStr_ReprSize));
    Testing_Assert(expectedSize == (size_t) reprSize, "expected size to be %zu but was %d", expectedSize, reprSize);
}

Testing_Fact(CStr_ReprToString_fails_if_buffer_or_string_is_NULL) {
    Testing_Assert(
            false == CStr_ReprToString(NULL, 42, "abc"), "expected %s to fail for NULL buffer",
            Testing_Nameof(CStr_ReprToString)
    );

    char buf[512] = {0};
    Testing_Assert(
            false == CStr_ReprToString(buf, sizeof(buf), NULL), "expected %s to fail for NULL string",
            Testing_Nameof(CStr_ReprToString)
    );
}

Testing_Fact(CStr_ReprToString_succedes_for_empty_string) {
    char buf[512] = {0};
    Testing_Assert(
            CStr_ReprToString(buf, sizeof(buf), ""),
            "expected %s to succeed", Testing_Nameof(CStr_ReprToString)
    );
    Testing_Assert(0 == strcmp("\"\"", buf), "expected repr to be \"\" but was %s", buf);
}

Testing_Fact(CStr_ReprToString_handles_escape_sequences) {
    char const *s = "abc\ndef\a\t\r123";
    char const *expectedRepr = "\"abc\\ndef\\a\\t\\r123\"";

    char buf[512] = {0};
    Testing_Assert(
            CStr_ReprToString(buf, sizeof(buf), s),
            "expected %s to succeed", Testing_Nameof(CStr_ReprToString)
    );
    Testing_Assert(0 == strcmp(expectedRepr, buf), "expected repr to be %s but was %s", expectedRepr, buf);
}

Testing_Fact(CStr_ReprToString_handles_non_printable_bytes) {
    char const *s = "abc\x1a def\xff ";
    char const *expectedRepr = "\"abc\\x1a def\\xff \"";

    char buf[512] = {0};
    Testing_Assert(
            CStr_ReprToString(buf, sizeof(buf), s),
            "expected %s to succeed", Testing_Nameof(CStr_ReprToString)
    );
    Testing_Assert(0 == strcmp(expectedRepr, buf), "expected repr to be %s but was %s", expectedRepr, buf);
}

Testing_Fact(CStr_ReprToString_requires_at_least_ReprSize_plus_one) {
    char const *s = "abc\x1a def\xff ";
    char const *expectedRepr = "\"abc\\x1a def\\xff \"";

    char buf[512] = {0};
    Testing_Assert(
            false == CStr_ReprToString(buf, CStr_ReprSize(s), s),
            "expected %s to fail", Testing_Nameof(CStr_ReprToString)
    );

    memset(buf, 0, sizeof(buf));
    Testing_Assert(
            CStr_ReprToString(buf, CStr_ReprSize(s) + 1, s),
            "expected %s to succeed", Testing_Nameof(CStr_ReprToString)
    );
    Testing_Assert(0 == strcmp(expectedRepr, buf), "expected repr to be %s but was %s", expectedRepr, buf);
}

Testing_Fact(CStr_ReprToFile_works) {
    char const *s = "abc\x1a def\xff ";
    char const *expectedRepr = "\"abc\\x1a def\\xff \"";

    FILE *f = fopen(Testing_Nameof(CStr_ReprToFile_works), "w+");
    Testing_Assert(NULL != f, "failed to create a temporary file");
    CStr_ReprToFile(f, s);
    fseek(f, 0, SEEK_SET);

    char buf[512] = {0};
    size_t read = 0;
    while (0 < (read = fread(buf + read, sizeof(*buf), sizeof(buf) - read, f)));
    fclose(f);

    Testing_Assert(0 == strcmp(expectedRepr, buf), "expected repr to be %s but was %s", expectedRepr, buf);
}

Testing_AllTests = {
        Testing_AddTest(CStr_ReprSize_returns_strlen_plus_two_for_string_with_no_special_chars),
        Testing_AddTest(CStr_ReprSize_returns_correct_size_for_string_with_special_chars),
        Testing_AddTest(CStr_ReprToString_fails_if_buffer_or_string_is_NULL),
        Testing_AddTest(CStr_ReprToString_succedes_for_empty_string),
        Testing_AddTest(CStr_ReprToString_handles_escape_sequences),
        Testing_AddTest(CStr_ReprToString_handles_non_printable_bytes),
        Testing_AddTest(CStr_ReprToString_requires_at_least_ReprSize_plus_one),
        Testing_AddTest(CStr_ReprToFile_works),
};

Testing_RunAllTests();
