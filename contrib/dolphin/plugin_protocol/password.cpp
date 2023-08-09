/*
 * Copyright (c) 2022 China Unicom Co.,Ltd.
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
 * password.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/password.cpp
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include "funcapi.h"
#include "utils/builtins.h"
#include "plugin_postgres.h"

#include "catalog/indexing.h"
#include "catalog/pg_authid.h"
#include "access/tableam.h"
#include "utils/acl.h"
#include "miscadmin.h" 

#include "plugin_protocol/password.h"

PG_FUNCTION_INFO_V1_PUBLIC(set_native_password);
extern "C" DLL_PUBLIC Datum set_native_password(PG_FUNCTION_ARGS);

Datum set_native_password(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(ARG_0)) {
        PG_RETURN_NULL();
    }

    text *role = PG_GETARG_TEXT_PP(ARG_0);
 
    if (PG_ARGISNULL(ARG_1)) {
        PG_RETURN_NULL();
    }
    text *password = PG_GETARG_TEXT_PP(ARG_1);

    char *sha1_password = TextDatumGetCString(DirectFunctionCall1(sha1, PointerGetDatum(password)));
    char sha1_password_byte[SHA_DIGEST_LENGTH + 1];
    sha1_hex_to_bytes(sha1_password, sha1_password_byte);

    Datum double_sha1_password = DirectFunctionCall1(sha1, CStringGetByteaDatum(sha1_password_byte, SHA_DIGEST_LENGTH));

    /*
    * Open pg_authid with RowExclusiveLock, do not release it until the end of the transaction.
    */
    Relation pg_authid_rel = heap_open(AuthIdRelationId, RowExclusiveLock);
    HeapTuple tuple = SearchSysCache1(AUTHNAME, PointerGetDatum(text_to_cstring(role)));
    if (!HeapTupleIsValid(tuple)) {
        if (!has_createrole_privilege(GetUserId())) {
            ereport(ERROR, (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE), errmsg("Permission denied.")));
        } else {
            ereport(ERROR, (errcode(ERRCODE_UNDEFINED_OBJECT), errmsg("role \"%s\" does not exist", text_to_cstring(role))));
        }   
    }

    Datum new_record[Natts_pg_authid];
    bool new_record_nulls[Natts_pg_authid] = {false};
    bool new_record_repl[Natts_pg_authid] = {false};

    errno_t errorno = memset_s(new_record, sizeof(new_record), 0, sizeof(new_record));
    securec_check(errorno, "\0", "\0");
    errorno = memset_s(new_record_nulls, sizeof(new_record_nulls), false, sizeof(new_record_nulls));
    securec_check(errorno, "\0", "\0");
    errorno = memset_s(new_record_repl, sizeof(new_record_repl), false, sizeof(new_record_repl));
    securec_check(errorno, "\0", "\0");

    new_record[Anum_pg_authid_rolpasswordext - 1] = double_sha1_password; 
    new_record_nulls[Anum_pg_authid_rolpasswordext - 1] = false;
    new_record_repl[Anum_pg_authid_rolpasswordext - 1] = true;

    TupleDesc pg_authid_dsc = RelationGetDescr(pg_authid_rel);
    HeapTuple new_tuple = (HeapTuple)tableam_tops_modify_tuple(tuple, pg_authid_dsc, new_record, new_record_nulls, new_record_repl);
    simple_heap_update(pg_authid_rel, &tuple->t_self, new_tuple);
    
    /* Update indexes */
    CatalogUpdateIndexes(pg_authid_rel, new_tuple);

    ReleaseSysCache(tuple);
    tableam_tops_free_tuple(new_tuple);

    heap_close(pg_authid_rel, RowExclusiveLock);

    return double_sha1_password;
}

/* Tranform string(40Bytes) to binary(20Bytes) */
void sha1_hex_to_bytes(const char b[SHA_DIGEST_LENGTH * 2], char t[SHA_DIGEST_LENGTH + 1])
{
    int i = 0;
    uint8 v1, v2;

    for (i = 0; i < SHA_DIGEST_LENGTH * 2; i += 2) {
        v1 = (b[i] >= 'a') ? (b[i] - 'a' + 10) : (b[i] - '0');
        v2 = (*((b + i) + 1)) >= 'a' ? (*((b + i) + 1)) - 'a' + 10 : (*((b + i) + 1)) - '0';
        t[i / 2] = (v1 << 4) + v2;
    }
    t[SHA_DIGEST_LENGTH] = 0x00;
}