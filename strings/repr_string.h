#pragma once

#include <stdio.h>
#include <stdbool.h>

int CStr_ReprSize(char const *str);

bool CStr_ReprToString(char *dst, size_t dstCapacity, char const *str);

void CStr_ReprToFile(FILE file[static 1], char const *str);
