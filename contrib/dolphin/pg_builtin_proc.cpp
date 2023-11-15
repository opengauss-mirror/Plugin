/*
 * Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * pg_builtin_proc.cpp
 *
 * IDENTIFICATION
 *    src/common/backend/catalog/pg_builtin_proc.cpp
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"
#include "fmgrtab.h"
#include "catalog/pg_language.h"
#include "catalog/pg_authid.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_proc.h"
#include "db4ai/predict_by.h"
#include "db4ai/explain_model.h"
#include "access/transam.h"
#include "storage/smgr/segment.h"
#include "plugin_utils/fmgroids.h"
#include "plugin_utils/pg_builtin_proc.h"

#ifdef DOLPHIN
/*
 * we init b_oidhash and b_nameHash by tmp value, to avoid other backend access b_oidhash/b_nameHash
 * before we have init all entry in it.
 */
struct HTAB* g_tmp_b_nameHash = NULL;
struct HTAB* g_tmp_b_oidHash = NULL;
#endif

static_assert(sizeof(true) == sizeof(char), "illegal bool size");
static_assert(sizeof(false) == sizeof(char), "illegal bool size");

#ifdef ENABLE_MULTIPLE_NODES
    FuncGroup b_func_groups[] = {
        #include "include/builtin_funcs.ini"
        #include "../../../distribute/kernel/catalog/distribute_builtin_funcs.ini"
    };
#else
    FuncGroup b_func_groups[] = {
        #include "include/builtin_funcs.ini"
    };
#endif
extern struct HTAB* b_nameHash;
extern struct HTAB* b_oidHash;
const int b_nfuncgroups = sizeof(b_func_groups) / sizeof(FuncGroup);

static const int MAX_PROC_NAME_LEN = NAMEDATALEN;

typedef struct HashEntryNameToFuncGroup {
    char name[MAX_PROC_NAME_LEN];
    const FuncGroup* group;
} HashEntryNameToFuncGroup;


typedef struct HashEntryOidToBuiltinFunc {
    Oid oid;
    const Builtin_func* func;
} HashEntryOidToBuiltinFunc;

static int cmp_func_by_oid(const void* a, const void* b)
{
    const Builtin_func* fa = *(const Builtin_func**)a;
    const Builtin_func* fb = *(const Builtin_func**)b;

    return (int)fa->foid - (int)fb->foid;
}

static int FuncGroupCmp(const void* a, const void* b)
{
    return pg_strcasecmp(((FuncGroup*)a)->funcName, ((FuncGroup*)b)->funcName);
}

static void SortBuiltinFuncGroups(FuncGroup* funcGroups)
{
    qsort(funcGroups, b_nfuncgroups, sizeof(FuncGroup), FuncGroupCmp);
}

const Builtin_func* g_sorted_funcs[nBuiltinFuncs];

static void InitHashTable(int size)
{
    HASHCTL info = {0};
    info.keysize = MAX_PROC_NAME_LEN;
    info.entrysize = sizeof(HashEntryNameToFuncGroup);
    info.hash = string_hash;
    info.hcxt = g_instance.builtin_proc_context;
    g_tmp_b_nameHash = hash_create("builtin proc name Lookup Table", size, &info,
                                HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT);
    info.keysize = sizeof(Oid);
    info.entrysize = sizeof(HashEntryOidToBuiltinFunc);
    info.hash = oid_hash;
    info.hcxt = g_instance.builtin_proc_context;
    g_tmp_b_oidHash = hash_create("builtin proc Oid Lookup Table", size, &info,
                                HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT);
}

static const FuncGroup* NameHashTableAccess(HASHACTION action, const char* name, const FuncGroup* group, HTAB* hashp)
{
    char temp_name[MAX_PROC_NAME_LEN] = {0};
    int rc = strncpy_s((char*)temp_name, MAX_PROC_NAME_LEN, name, strlen(name));
    securec_check(rc, "\0", "\0");
    HashEntryNameToFuncGroup *result = NULL;
    bool found = false;

    Assert(name != NULL);

    result = (HashEntryNameToFuncGroup *)hash_search(hashp, &temp_name, action, &found);
    if (action == HASH_ENTER) {
        Assert(!found);
        result->group = group;
        return group;
    }
    else if (action == HASH_FIND) {
        if (found)
            return result->group;
        else
            return NULL;
    } else
        return NULL;
}


static const Builtin_func* OidHashTableAccess(HASHACTION action, Oid oid, const Builtin_func* func, HTAB* hashp)
{
    HashEntryOidToBuiltinFunc *result = NULL;
    bool found = false;
    Assert(oid > 0);

    result = (HashEntryOidToBuiltinFunc *)hash_search(hashp, &oid, action, &found);
    if (action == HASH_ENTER) {
        Assert(!found);
        result->func = func;
        return func;
    }
    else if (action == HASH_FIND) {
        if (found)
            return result->func;
        else
            return NULL;
    } else
        return NULL;
}


static void CheckNameLength(const char* name)
{
    if (strlen(name) > MAX_PROC_NAME_LEN) {
        ereport(PANIC,
            (errmsg("the built-in function name length exceed the limit of %d", MAX_PROC_NAME_LEN)));
    }
}

void initBSQLBuiltinFuncs()
{
    InitHashTable(b_nfuncgroups);
    SortBuiltinFuncGroups(b_func_groups);
    int nfunc = 0;
    for (int i = 0; i < b_nfuncgroups; i++) {
        const FuncGroup* fg = &b_func_groups[i];
        CheckNameLength(fg->funcName);
        NameHashTableAccess(HASH_ENTER, fg->funcName, fg, g_tmp_b_nameHash);

        for (int j = 0; j < fg->fnums; j++) {
            CheckNameLength(fg->funcs[j].funcName);
            OidHashTableAccess(HASH_ENTER, fg->funcs[j].foid, &fg->funcs[j], g_tmp_b_oidHash);
            g_sorted_funcs[nfunc++] = &fg->funcs[j];
        }
    }

    if (nfunc != nBuiltinFuncs) {
        ereport(PANIC,
            (errmsg("initialize the built-in function failed: %s",
                "the number of functions in is mismatch with the declaration")));
    }
    qsort(g_sorted_funcs, nBuiltinFuncs, sizeof(g_sorted_funcs[0]), cmp_func_by_oid);
    /* all have beed inited, assign value to real b_nameHash/b_oidHash */
    b_nameHash = g_tmp_b_nameHash;
    b_oidHash = g_tmp_b_oidHash;
}
