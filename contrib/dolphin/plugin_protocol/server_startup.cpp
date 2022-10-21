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
#include "libpq/libpq.h"

#include "plugin_protocol/proto_com.h"
#include "plugin_protocol/handler.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/auth.h"
#include "plugin_protocol/startup.h"
#include "plugin_postgres.h"

#define DOLPHINE_DEFAULT_SERVER_NAME "Dolphin-Server"

/* Global variables */
dolphin_proto_ctx g_proto_ctx;

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
    DefineCustomIntVariable(
                "dolphin.port",
                gettext_noop("Sets the dolphin TCP port the server listens on."),
                NULL,
                &g_proto_ctx.port,
                3307, 1024, 65535,
                PGC_SIGHUP,
                GUC_NOT_IN_SAMPLE,
                NULL, NULL, NULL);
    DefineCustomStringVariable(
                "dolphin.default_database_name",
                gettext_noop("Predefined dolphin database name"),
                NULL,
                &GetSessionContext()->default_database_name,
                pstrdup(g_proto_ctx.database_name.data),
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
                    g_proto_ctx.port,
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
                    g_proto_ctx.port,
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
                        g_proto_ctx.port)));
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
