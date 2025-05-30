/* -------------------------------------------------------------------------
 *
 * arrayutils.c
 *	  This file contains some support routines required for array functions.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/arrayutils.c
 *
 * -------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include "catalog/pg_type.h"
#include "common/int.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/memutils.h"

/*
 * ArrayGetIntegerTypmods: verify that argument is a 1-D cstring array,
 * and get the contents converted to integers.	Returns a palloc'd array
 * and places the length at *n.
 */
int32* ArrayGetIntegerTypmods(ArrayType* arr, int* n)
{
    int32* result = NULL;
    Datum* elem_values = NULL;
    int i;

    if (ARR_ELEMTYPE(arr) != CSTRINGOID)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_ELEMENT_ERROR), errmsg("typmod array must be type cstring[]")));

    if (ARR_NDIM(arr) != 1)
        ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR), errmsg("typmod array must be one-dimensional")));

    if (array_contains_nulls(arr))
        ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED), errmsg("typmod array must not contain nulls")));

    /* hardwired knowledge about cstring's representation details here */
    deconstruct_array(arr, CSTRINGOID, -2, false, 'c', &elem_values, NULL, n);

    result = (int32*)palloc(*n * sizeof(int32));

    for (i = 0; i < *n; i++)
        result[i] = pg_strtoint32(DatumGetCString(elem_values[i]));

    pfree_ext(elem_values);

    return result;
}
