# String Utilities

 * [C-style string representation](#c-style-string-representation)
 * [String builder](#string-builder)

### C-style string representation

[repr_cstr.h](repr_cstr.h), [repr_string_test.c](repr_string_test.c)

Define `REPR_CSTR_IMPLEMENTATION` before `#include "repr_cstr.h"` to 
include function definitions.

 * `int CStr_ReprSize(char const *str)` - return the number of bytes
required to represent a string **not including** `'\0'`.
 * `bool CStr_ReprToString(char *dst, size_t dstCapacity, char const *str)` - 
print a valid C representation of a string `str` to `dst`.
At most `dstCapacity` will be printed **including** `'\0'`.
Returns `true` iff no errors occurred and `dstCapacity` was sufficient.
 * `void CStr_ReprToFile(FILE file[static 1], char const *str)` - similar to
`CStr_ReprToString` but prints to a `file`.

Examples:

```c
char const *s = "abc\x1a def\xff ";

size_t bufSize = 1 + CStr_ReprSize(s);
char *buf = calloc(bufSize, sizeof(*buf));
CStr_ReprToString(buf, bufSize, s);
```

### String builder

[string_builder.h](string_builder.h), [string_builder_test.c](string_builder_test.c)

Define `STRING_BUILDER_IMPLEMENTATION` before `#include "string_builder.h"` to
include function definitions.

 * `StringBuilder_Empty()` - returns an empty string builder.
 * `void StringBuilder_Free(StringBuilder *)` - frees a string builder
and sets its value to `StringBuilder_Empty()`.
 * `void StringBuilder_Sprintf(StringBuilder builder[static 1], char const *format, ...)` - 
see docs for [`sprintf`](https://linux.die.net/man/3/sprintf).
Safe version that pre-allocates enough space.
 * `void StringBuilder_Append(StringBuilder builder[static 1], char const *src)` - 
append a string.
 * `void StringBuilder_AppendChars(StringBuilder builder[static 1], size_t count, char const src[static count])` - 
append a list of characters.
 * `char *StringBuilder_ToCStr(StringBuilder builder)` - return
a copy of builder's contents. Returned string must be freed manually
using `free`.

Examples:

```c
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
```
