# String Utilities

 * [repr_cstr.h - C-style string representation](#c-style-string-representation)
 * [string_builder.h - String builder](#string-builder)

## C-style string representation

[repr_cstr.h](repr_cstr.h), [repr_string_test.c](repr_string_test.c)

Define `REPR_CSTR_IMPLEMENTATION` before `#include "repr_cstr.h"` to 
include function definitions.

### Examples

```c
char const *s = "abc\x1a def\xff ";

size_t bufSize = 1 + CStr_ReprSize(s);
char *buf = calloc(bufSize, sizeof(*buf));
CStr_ReprToString(buf, bufSize, s);
```

### Functions

* [CStr_ReprSize](#cstrreprsize)
* [CStr_ReprToString](#cstrreprtostring)
* [CStr_ReprToFile](#cstrreprtofile)

#### CStr_ReprSize
```c
int CStr_ReprSize(char const *str);
```
Return the number of bytes
required to represent a string **not including** `'\0'`.

#### CStr_ReprToString
```c
bool CStr_ReprToString(char *dst, size_t dstCapacity, char const *str);
```
Write a valid C representation of a string `str` to `dst`.
At most `dstCapacity` bytes will be written **including** `'\0'`.
Returns `true` iff no errors occurred and `dstCapacity` was sufficient.

#### CStr_ReprToFile
```c
void CStr_ReprToFile(FILE file[static 1], char const *str);
```
Similar to `CStr_ReprToString`, but writes to a `file`.

## String builder

[string_builder.h](string_builder.h), [string_builder_test.c](string_builder_test.c)

Define `STRING_BUILDER_IMPLEMENTATION` before `#include "string_builder.h"` to
include function definitions.

### Examples

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

### Functions

* [StringBuilder_Empty](#stringbuilderempty)
* [StringBuilder_Free](#stringbuilderfree)
* [StringBuilder_Sprintf](#stringbuildersprintf)
* [StringBuilder_Append](#stringbuilderappend)
* [StringBuilder_AppendChars](#stringbuilderappendchars)
* [StringBuilder_ToCStr](#stringbuildertocstr)

#### StringBuilder_Empty
```c
#define StringBuilder_Empty()
```
Returns an empty string builder.

#### StringBuilder_Free
```c
void StringBuilder_Free(StringBuilder *);
```
Frees a string builder
and sets it to `StringBuilder_Empty()`.

#### StringBuilder_Sprintf
```c
void StringBuilder_Sprintf(StringBuilder builder[static 1], char const *format, ...);
```
See docs for [`sprintf`](https://linux.die.net/man/3/sprintf).
Safe version that pre-allocates enough space.

#### StringBuilder_Append
```c
void StringBuilder_Append(StringBuilder builder[static 1], char const *src);
```
Append a string.

#### StringBuilder_AppendChars
```c
void StringBuilder_AppendChars(StringBuilder builder[static 1], size_t count, char const src[static count]);
```
Append a list of characters.

#### StringBuilder_ToCStr
```c
char *StringBuilder_ToCStr(StringBuilder builder);
```
Return
a copy of builder's contents. Returned string must be freed manually
using `free`.
