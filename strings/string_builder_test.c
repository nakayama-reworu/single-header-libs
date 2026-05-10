#define STRING_BUILDER_IMPLEMENTATION

#include "string_builder.h"

#include "testing/testing.h"

Testing_Fact(ToCStr_returns_empty_string_for_empty_builder) {
    StringBuilder sut = StringBuilder_Empty();

    char *const str = StringBuilder_ToCStr(sut);
    Testing_Assert(0 == strcmp("", str), "expected empty string but got \"%s\"", str);

    free(str);
    StringBuilder_Free(&sut);
}

Testing_Fact(Append_appends_a_string) {
    StringBuilder sut = StringBuilder_Empty();
    char const *strings[] = {
            "A", " ", "quick", " ", "brown", " ", "fox", " ", "jumps", NULL
    };
    char const *const expected = "A quick brown fox jumps";

    for (char const **it = strings; NULL != *it; it++) {
        StringBuilder_Append(&sut, *it);
    }

    char *const str = StringBuilder_ToCStr(sut);
    Testing_Assert(0 == strcmp(expected, str), "expected \"%s\" but got \"%s\"", expected, str);

    free(str);
    StringBuilder_Free(&sut);
}

Testing_Fact(AppendChars_appends_a_given_number_of_characters) {
    StringBuilder sut = StringBuilder_Empty();

    char const *const src = "Totally more than 10 chars";
    size_t const count = 10;
    char const *const expected = "Totally mo";

    StringBuilder_AppendChars(&sut, count, src);

    char *const str = StringBuilder_ToCStr(sut);
    Testing_Assert(0 == strcmp(expected, str), "expected \"%s\" but got \"%s\"", expected, str);

    free(str);
    StringBuilder_Free(&sut);
}

Testing_Fact(Sprintf_acts_as_Append_if_no_additional_arguments_are_provided) {
    StringBuilder builder = StringBuilder_Empty();

    StringBuilder_AppendChars(&builder, 6, "A long string");
    StringBuilder_Append(&builder, "\nstring");
    StringBuilder_Sprintf(&builder, "\n%d %s", 42, "another string");

    char *s = StringBuilder_ToCStr(builder);
    printf("%s", s);
    // A long
    // string
    // 42 another string
    StringBuilder_Free(&builder);


    StringBuilder sut = StringBuilder_Empty();
    char const *strings[] = {
            "A", " ", "quick", " ", "brown", " ", "fox", " ", "jumps", NULL
    };
    char const *const expected = "A quick brown fox jumps";

    for (char const **it = strings; NULL != *it; it++) {
        StringBuilder_Sprintf(&sut, *it);
    }

    char *const str = StringBuilder_ToCStr(sut);
    Testing_Assert(0 == strcmp(expected, str), "expected \"%s\" but got \"%s\"", expected, str);

    free(str);
    StringBuilder_Free(&sut);
}

Testing_Fact(Sprintf_allows_appending_formatted_stirngs) {
    StringBuilder sut = StringBuilder_Empty();
    char const *strings[] = {
            "A", "quick", "brown", "fox", "jumps", NULL
    };
    char const *const expected =
            "[0]=A\n"
            "[1]=quick\n"
            "[2]=brown\n"
            "[3]=fox\n"
            "[4]=jumps\n";

    for (size_t i = 0; NULL != strings[i]; i++) {
        StringBuilder_Sprintf(&sut, "[%zu]=%s\n", i, strings[i]);
    }

    char *const str = StringBuilder_ToCStr(sut);
    Testing_Assert(0 == strcmp(expected, str), "expected \"%s\" but got \"%s\"", expected, str);

    free(str);
    StringBuilder_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(ToCStr_returns_empty_string_for_empty_builder),
        Testing_AddTest(Append_appends_a_string),
        Testing_AddTest(AppendChars_appends_a_given_number_of_characters),
        Testing_AddTest(Sprintf_acts_as_Append_if_no_additional_arguments_are_provided),
        Testing_AddTest(Sprintf_allows_appending_formatted_stirngs),
};

Testing_RunAllTests();
