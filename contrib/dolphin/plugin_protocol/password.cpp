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

#include "plugin_protocol/auth.h"
#include "plugin_protocol/password.h"

PG_FUNCTION_INFO_V1_PUBLIC(set_native_password);
PG_FUNCTION_INFO_V1_PUBLIC(set_caching_sha2_password);
extern "C" DLL_PUBLIC Datum set_native_password(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum set_caching_sha2_password(PG_FUNCTION_ARGS);
bool check_password(HeapTuple tuple, text* user, text *input_password);

Datum set_native_password(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(ARG_0)) {
        PG_RETURN_NULL();
    }

    text *role = PG_GETARG_TEXT_PP(ARG_0);
 
    if (PG_ARGISNULL(ARG_1)) {
        PG_RETURN_NULL();
    }
    text *new_password = PG_GETARG_TEXT_PP(ARG_1);

    if (PG_ARGISNULL(ARG_2)) {
        PG_RETURN_NULL();
    }
    text *old_password = PG_GETARG_TEXT_PP(ARG_2);

    char *sha1_password = TextDatumGetCString(DirectFunctionCall1(sha1, PointerGetDatum(new_password)));
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
    if (!check_password(tuple, role, old_password)) {
        ereport(ERROR,  (errcode(ERRCODE_INVALID_PASSWORD), errmsg("Set authentication method verification failed, "
                         "password does not match original password")));
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

void generate_user_salt(char* buffer, int buffer_len)
{
    char* end = buffer + buffer_len - 1;
    RAND_bytes((unsigned char*)buffer, buffer_len);

    /* Sequence must be a legal UTF8 string */
    for (; buffer < end; buffer++) {
        *buffer &= 0x7f;
        if (*buffer == '\0' || *buffer == '$') {
            *buffer = *buffer + 1;
        }
    }
    /* Make sure the buffer is terminated properly */
    *end = '\0';
}

Datum set_caching_sha2_password(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(ARG_0)) {
        PG_RETURN_NULL();
    }

    text *role = PG_GETARG_TEXT_PP(ARG_0);
 
    if (PG_ARGISNULL(ARG_1)) {
        PG_RETURN_NULL();
    }
    text *new_password = PG_GETARG_TEXT_PP(ARG_1);
    
    if (PG_ARGISNULL(ARG_2)) {
        PG_RETURN_NULL();
    }
    text *old_password = PG_GETARG_TEXT_PP(ARG_2);

    char* plaintext = text_to_cstring(new_password);
    char salt[SHA1_HASH_SIZE + 1] = { 0 };
    generate_user_salt(salt, SHA1_HASH_SIZE + 1);

    Datum sha2_password = DirectFunctionCall2(make_scrambled_full_password_sha2,
                                              PointerGetDatum(plaintext), PointerGetDatum(salt));

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

    if (!check_password(tuple, role, old_password)) {
        ereport(ERROR,  (errcode(ERRCODE_INVALID_PASSWORD), errmsg("Set authentication method verification failed, "
                        "password does not match original password")));
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

    new_record[Anum_pg_authid_rolpasswordext - 1] = sha2_password;
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

    return sha2_password;
}

#define PARAMNUM_2 2
#define PARAMNUM_3 3

bool check_password(HeapTuple tuple, text* role, text* input_password)
{
    char *stored_password;
    char *new_password;
    bool isNull;
    // 1.get rollpasswordext from pg_authid
    /* Get role info from pg_authid */
    Datum datum = SysCacheGetAttr(AUTHNAME, tuple, Anum_pg_authid_rolpasswordext, &(isNull));
    if (isNull) {
        return true;
    }
    stored_password = TextDatumGetCString(datum);
    char *user = text_to_cstring(role);
    /*If the previous password used SHA2 authentication method and the authentication method set is native,
    the original authentication method needs to be encrypted and compared*/
    if (stored_password[0] == '$') {
        char salt[SHA1_HASH_SIZE + 1] = { 0 };
        char* plaintext = text_to_cstring(input_password);
        char parts[PARAMNUM_3][CRYPT_MAX_PASSWORD_SIZE + 1] = { 0 };
        int num_parts = 0;
        errno_t rc = 0;
        char *str_text = (char *)palloc(strlen(stored_password) + 1);
        rc = strcpy_s(str_text, strlen(stored_password) + 1, stored_password);
        securec_check(rc, "\0", "\0");
        char *token = strtok(str_text, "$");
        while (token != NULL && num_parts < PARAMNUM_3) {
            rc = strncpy_s(parts[num_parts], CRYPT_MAX_PASSWORD_SIZE, token, strlen(token));
            securec_check(rc, "\0", "\0");
            num_parts++;
            token = strtok(NULL, "$");
        }  
        rc = strncpy_s(salt, SHA1_HASH_SIZE + 1, parts[PARAMNUM_2], CRYPT_SALT_LENGTH);
        securec_check(rc, "\0", "\0");
        pfree(str_text);
        Datum sha2_password = DirectFunctionCall2(make_scrambled_full_password_sha2,
                                                  PointerGetDatum(plaintext), PointerGetDatum(salt));
        new_password = TextDatumGetCString(sha2_password);
    } else {
        char *sha1_password = TextDatumGetCString(DirectFunctionCall1(sha1, PointerGetDatum(input_password)));
        char sha1_password_byte[SHA_DIGEST_LENGTH + 1];
        sha1_hex_to_bytes(sha1_password, sha1_password_byte);
        Datum double_sha1_password = DirectFunctionCall1(sha1,
                                                         CStringGetByteaDatum(sha1_password_byte, SHA_DIGEST_LENGTH));
        new_password = TextDatumGetCString(double_sha1_password);
    }
    
    if (!strcmp(stored_password, new_password)) {
        if (stored_password[0] == '$') {
            AutoMutexLock UserCachedLinesHashLock(&gUserCachedLinesHashLock);
            UserCachedLinesHashLock.lock();
            UserCachedLinesHashTableAccess(HASH_REMOVE, user, NULL);
            UserCachedLinesHash* result =  UserCachedLinesHashTableAccess(HASH_FIND, user, NULL);
            UserCachedLinesHashLock.unLock();
            if (result) {
                ereport(ERROR, (errmsg("Drop UserCachedLines cache failed !")));
            }
        }
        return true;
    } else {
        return false;
    }
}