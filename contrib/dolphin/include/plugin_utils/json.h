/*-------------------------------------------------------------------------
 *
 * json.h
 *	  Declarations for JSON data type support.
 *
 * Portions Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2014, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * dolphin/include/plugin_utils/json.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PLUGIN_JSON_H
#define PLUGIN_JSON_H
#define VALTYPE_IS_JSON(v) \
    ((v) == UNKNOWNOID || (v) == JSONOID || (v) == JSONBOID || (v) == CSTRINGOID || (v) == TEXTOID || \
     (v) == CHAROID || (v) == VARCHAROID || (v) == BPCHAROID)

extern void get_keys_order(char **a, int l, int r, int pos[]);
#endif   /* PLUGIN_JSON_H */