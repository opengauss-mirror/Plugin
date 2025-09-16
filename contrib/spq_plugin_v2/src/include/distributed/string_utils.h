/*-------------------------------------------------------------------------
 *
 * string_utils.h
 *   Utilities related to strings.
 *
 * Copyright (c) Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef CITUS_STRING_UTILS_H
#define CITUS_STRING_UTILS_H

#include "postgres.h"

extern char* ConvertIntToString(int val);

#define StringStartsWith(str, prefix) (strncmp(str, prefix, strlen(prefix)) == 0)

extern char* pg_clean_ascii(const char* str, int alloc_flags);

bool SplitGUCList(char* rawstring, char separator, List** namelist);
#endif /* CITUS_STRING_UTILS_H */
