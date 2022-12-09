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
 * server_startup.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/server_startup.cpp
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "miscadmin.h"
#include "utils/guc.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "libpq/libpq.h"
#include "executor/executor.h"

#include "plugin_protocol/proto_com.h"
#include "plugin_protocol/handler.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/auth.h"
#include "plugin_protocol/startup.h"
#include "plugin_postgres.h"

#define DOLPHINE_DEFAULT_SERVER_NAME "Dolphin-Server"

/* Global variables */
dolphin_proto_ctx g_proto_ctx;

struct HTAB* b_typoid2DolphinMarcoHash;
TypeItem b_type_items[] = {
    // int type
    {"int4", DOLPHIN_TYPE_LONG, INT4OID, 0, 0x2d},                                // integer
    {"uint4", DOLPHIN_TYPE_LONG, 0, UNSIGNED_FLAG, 0x2d},                         // integer unsigned
    {"int1", DOLPHIN_TYPE_TINY, INT1OID, 0, 0x2d},                                // tinyint
    {"uint1", DOLPHIN_TYPE_TINY, 0, UNSIGNED_FLAG, 0x2d},                         // tinyint unsigned
    {"int2", DOLPHIN_TYPE_SHORT, INT2OID, 0, 0x2d},                               // shortint
    {"uint2", DOLPHIN_TYPE_SHORT, 0, UNSIGNED_FLAG, 0x2d},                        // shortint unsigned
    {"int8", DOLPHIN_TYPE_LONGLONG, INT8OID, 0, 0x2d},                            // bigint
    {"uint8", DOLPHIN_TYPE_LONGLONG, 0, UNSIGNED_FLAG, 0x2d},                     // bigint unsigned
    //float type
    {"float4", DOLPHIN_TYPE_FLOAT, FLOAT4OID, 0, 0x2d},                           // float4 or real
    {"float8", DOLPHIN_TYPE_DOUBLE, FLOAT8OID, 0, 0x2d},                          // float8 or double
    // bit type
    {"bit", DOLPHIN_TYPE_BIT, BITOID, 0, 0x2d},                                   // bit
    {"varbit", DOLPHIN_TYPE_BIT, VARBITOID, 0, 0x2d},                             // bit
    // bool type
    {"bool", DOLPHIN_TYPE_TINY, BOOLOID, 0, 0x2d},                                 // bool
    //char type
    {"char", DOLPHIN_TYPE_STRING, CHAROID, 0, 0x2d},                              // char
    {"bpchar", DOLPHIN_TYPE_STRING, BPCHAROID, 0, 0x2d},                          // bpchar
    {"binary", DOLPHIN_TYPE_STRING, 0, BINARY_FLAG, COLLATE_BINARY},              // binary
    // varchar type
    {"varchar", DOLPHIN_TYPE_VARCHAR, VARCHAROID, 0, 0x2d},                       // varchar
    {"nvarchar2", DOLPHIN_TYPE_VARCHAR, NVARCHAR2OID, 0, 0x2d},                   // nvarchar
    {"varbinary", DOLPHIN_TYPE_VAR_STRING, 0, BINARY_FLAG, COLLATE_BINARY},       // varbinary
    {"bytea", DOLPHIN_TYPE_VAR_STRING, BYTEAOID, BINARY_FLAG, COLLATE_BINARY},    // varbinary
    // date and time
    {"year", DOLPHIN_TYPE_YEAR, 0, 0, 0x2d},                                      // year
    {"date", DOLPHIN_TYPE_DATE, DATEOID, 0, 0x2d},                                // date
    {"time", DOLPHIN_TYPE_TIME, TIMEOID, 0, 0x2d},                                // time
    {"timestamptz", DOLPHIN_TYPE_TIMESTAMP, TIMESTAMPTZOID, 0, 0x2d},             // timestamp, but not maxdb mode
    {"timestamp", DOLPHIN_TYPE_DATETIME, TIMESTAMPOID, 0, 0x2d},                  // datetime
    // blob type
    {"tinyblob", DOLPHIN_TYPE_TINY_BLOB, 0, BLOB_FLAG | BINARY_FLAG, 0x2d},       // tinyblob
    {"blob", DOLPHIN_TYPE_BLOB, BLOBOID, BLOB_FLAG | BINARY_FLAG, COLLATE_BINARY},// blob
    {"mediumblob", DOLPHIN_TYPE_MEDIUM_BLOB, 0, BLOB_FLAG | BINARY_FLAG, 0x2d},   // mediumblob
    {"longblob", DOLPHIN_TYPE_LONG_BLOB, 0, BLOB_FLAG | BINARY_FLAG, 0x2d},       // longblob
    // spatial type
    {"point", DOLPHIN_TYPE_GEOMETRY, POINTOID, 0, 0x2d},                          // point
    {"line", DOLPHIN_TYPE_GEOMETRY, LINEOID, 0, 0x2d},                            // line
    {"polygon", DOLPHIN_TYPE_GEOMETRY, POLYGONOID, 0, 0x2d},                      // polygon
    // text type, og is not support tinytext, meidumtext, longtext
    {"text", DOLPHIN_TYPE_BLOB, TEXTOID, 0, 0x2d},                                // text
    // fixed type
    {"numeric", DOLPHIN_TYPE_DECIMAL, NUMERICOID, 0, 0x2d},                       // numeric
    // json
    {"json", DOLPHIN_TYPE_JSON, JSONOID, 0, 0x2d}                                 // json
};

pthread_mutex_t gMarcoHashLock;

const int b_ntype_items = sizeof(b_type_items) / sizeof(TypeItem);
static const TypeItem* TypoidHashTableAccess(HASHACTION action, Oid oid, const TypeItem* item);
static void InitDolphinMicroHashTable(int size);

static void AssignDatabaseName(const char* newval, void* extra);

static ProtocolExtensionConfig dolphin_protocol_config = {
    true,
    pq_init,
    dophin_conn_handshake, /* fn_start */
    dophin_client_authentication, /* fn_authenticate */
    dophin_send_message,  /* fn_send_message */
    NULL,
    NULL,
    dophin_send_ready_for_query, /* fn_send_ready_for_query */
    dophin_read_command, /* fn_read_command*/
    dophin_printtup_create_DR, /* fn_printtup_create_DR */
    dolphin_set_DR_params,
    dolphin_process_command, /* fn_process_command */
    NULL
};

/*
 * Define various GUCs which are part of dolphin protocol
 */
void define_dolphin_server_guc()
{
    DefineCustomStringVariable(
                "dolphin.default_database_name",
                gettext_noop("Predefined dolphin database name"),
                NULL,
                &GetSessionContext()->default_database_name,
                g_proto_ctx.database_name.data,
                PGC_SIGHUP,
                GUC_NOT_IN_SAMPLE,
                NULL, AssignDatabaseName, NULL);
}

static void AssignDatabaseName(const char *newval, void *extra)
{
    if (strcmp(newval, g_proto_ctx.database_name.data)) {
        int ret = strcpy_s(g_proto_ctx.database_name.data, NAMEDATALEN, newval);
        securec_check(ret, "\0", "\0");
    }
}

static bool isNotWildcard(void *val1, void *val2)
{
    ListCell* cell = (ListCell*)val1;
    char* nodename = (char*)val2;
    char* curhost = (char*)lfirst(cell);
    
    return strcmp(curhost, nodename) != 0;
}

/*
 * server_listen_init - Create the TCP server socket(s)
 */
void server_listen_init(void)
{
    int     status;
    bool    listen_addr_saved = false;

    if (g_instance.attr.attr_network.ListenAddresses && !dummyStandbyMode) {
        char* rawstring = NULL;
        List* elemlist = NULL;
        ListCell* l = NULL;
        int success = 0;

        /* Need a modifiable copy of g_instance.attr.attr_network.ListenAddresses */
        rawstring = pstrdup(g_instance.attr.attr_network.ListenAddresses);

        /* Parse string into list of identifiers */
        if (!SplitIdentifierString(rawstring, ',', &elemlist)) {
            /* syntax error in list */
            ereport(FATAL,
                (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid list syntax for \"listen_addresses\"")));
        }

        bool haswildcard = false;
        foreach (l, elemlist) {
            char* curhost = (char*)lfirst(l);
            if (strcmp(curhost, "*") == 0) {
                haswildcard = true;
                break;
            }
        }
        
        if (haswildcard) {
            char *wildcard = "*";
            elemlist = list_cell_clear(elemlist, (void *)wildcard, isNotWildcard);
        }

        foreach (l, elemlist) {
            char* curhost = (char*)lfirst(l);

            if (strcmp(curhost, "*") == 0) {
                status = StreamServerPort(AF_UNSPEC,
                    NULL,
                    g_instance.attr.attr_network.dolphin_server_port,
                    g_instance.attr.attr_network.UnixSocketDir,
                    g_instance.listen_cxt.ListenSocket,
                    MAXLISTEN,
                    true,
                    true,
                    false,
                    &dolphin_protocol_config);
            } else {
                status = StreamServerPort(AF_UNSPEC,
                    curhost,
                    g_instance.attr.attr_network.dolphin_server_port,
                    g_instance.attr.attr_network.UnixSocketDir,
                    g_instance.listen_cxt.ListenSocket,
                    MAXLISTEN,
                    true,
                    true,
                    false,
                    &dolphin_protocol_config);
            }

            if (status == STATUS_OK) {
                success++;
            } else {
                ereport(FATAL,
                    (errmsg("could not create listen socket for \"%s:%d\"",
                        curhost,
                        g_instance.attr.attr_network.dolphin_server_port)));
            }

            /* record the first successful host addr in lockfile */
            if (!listen_addr_saved) {
                AddToDataDirLockFile(LOCK_FILE_LINE_LISTEN_ADDR, curhost);
                listen_addr_saved = true;
            }
        }

        if (!success && list_length(elemlist))
            ereport(FATAL, (errmsg("could not create any TCP/IP sockets")));

        list_free_ext(elemlist);
        pfree(rawstring);
    }
}

void InitTypoid2DolphinMacroHtab()
{
    InitDolphinMicroHashTable(b_ntype_items);
    for (int i = 0; i < b_ntype_items; i++) {
        TypeItem* typeItem = &b_type_items[i];
        if (strlen(typeItem->og_typname) > MAX_TYPE_NAME_LEN) {
            ereport(ERROR, (errmsg("the type name length exceed the limit of %d", MAX_TYPE_NAME_LEN)));
        }
        if (!OidIsValid(typeItem->og_type_oid)) {
            typeItem->og_type_oid = get_typeoid(PG_CATALOG_NAMESPACE, typeItem->og_typname);
        }
        TypoidHashTableAccess(HASH_ENTER, typeItem->og_type_oid, typeItem);
    }
}

const TypeItem* GetItemByTypeOid(Oid oid)
{
    // double check is used to solve concurrent error problems
    if (b_typoid2DolphinMarcoHash == NULL) {
        AutoMutexLock marcoHashLock(&gMarcoHashLock);
        marcoHashLock.lock();
        if (b_typoid2DolphinMarcoHash == NULL) {
        InitTypoid2DolphinMacroHtab();
        }
        marcoHashLock.unLock();
    }
    if (!OidIsValid(oid)) {
        ereport(ERROR, (errmsg("the oid of data type is invalid")));
    }
    const TypeItem* typeItem = TypoidHashTableAccess(HASH_FIND, oid, NULL);
    if (typeItem == NULL) {
        /*
         * Maybe the enum 
         */
        Form_pg_type typeForm;

        HeapTuple tuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(oid));
        TypeItem* result = NULL;
        if (HeapTupleIsValid(tuple)) {
            typeForm = (Form_pg_type)GETSTRUCT(tuple);
            result = (TypeItem*)palloc(sizeof(TypeItem));
            result->og_typname = NameStr(typeForm->typname);
            result->og_type_oid = oid;
            result->flags = 0;
            switch (typeForm->typtype) {
                case 'e':
                    result->dolphin_type_id = DOLPHIN_TYPE_ENUM;
                    break;
                case 's':
                    result->dolphin_type_id = DOLPHIN_TYPE_SET;
                    break;
                default:
                    ereport(WARNING, (errmsg("type \"%s\" cannot find the macro, use string macro instead", result->og_typname)));
                    result->dolphin_type_id = DOLPHIN_TYPE_STRING;
                    break;
            }
            ReleaseSysCache(tuple);
        } else {
            ereport(ERROR, (errmsg("%d is not the object id of type", oid)));
        }
        return result;
    }
    return typeItem;
}

static const TypeItem* TypoidHashTableAccess(HASHACTION action, Oid oid, const TypeItem* item)
{
    HashEntryTypoid2TypeItem* result = NULL;
    bool found = false;
    if (!OidIsValid(oid)) {
        ereport(ERROR, (errmsg("the oid of data type is invalid")));
    }

    result = (HashEntryTypoid2TypeItem*)hash_search(b_typoid2DolphinMarcoHash, &oid, action, &found);
    if (action == HASH_ENTER) {
        Assert(!found);
        result->item = item;
        return item;
    } else if (action == HASH_FIND) {
        if (found) {
            return result->item;
        }
    }
    return NULL;
}

static void InitDolphinMicroHashTable(int size)
{
    HASHCTL info = {0};
    info.keysize = sizeof(Oid);
    info.entrysize = sizeof(HashEntryTypoid2TypeItem);
    info.hash = oid_hash;
    info.hcxt = g_instance.instance_context;
    b_typoid2DolphinMarcoHash = hash_create("Dolphin Micro Type Oid Lookup Table", size, &info,
                                                HASH_ELEM | HASH_FUNCTION | HASH_CONTEXT);
}
