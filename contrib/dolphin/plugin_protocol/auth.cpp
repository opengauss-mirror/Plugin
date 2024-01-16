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
 * auth.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/auth.cpp
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "libpq/ip.h"
#include "gs_policy/policy_common.h"
#include "auditfuncs.h"
#include "miscadmin.h"

#include "catalog/pg_authid.h"
#include "utils/builtins.h"

#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/password.h"
#include "plugin_protocol/auth.h"

#define CLIENT_PASSWORD_TOKEN_LEN 20

static THR_LOCAL char scramble[AUTH_PLUGIN_DATA_LEN + 1]; 
static THR_LOCAL char token[CLIENT_PASSWORD_TOKEN_LEN + 1];

static void dophin_hba_authentication(Port *prot);
static bool exec_native_password_auth(Port *port);

int dophin_conn_handshake(Port* port)
{
    if (t_thrd.postmaster_cxt.HaShmData->current_mode == STANDBY_MODE &&
        (!g_instance.attr.attr_network.dolphin_hot_standby || !g_instance.attr.attr_storage.EnableHotStandby)) {
        ereport(ERROR, (errcode(ERRCODE_CANNOT_CONNECT_NOW),
                errmsg("can not accept connection if dolphin hot standby off")));
    }

    StringInfo buf = makeStringInfo();
    next_seqid = 0;

    // send handshakev10 packet
    network_mysqld_auth_challenge *challenge = make_mysqld_handshakev10_packet(scramble);
    send_auth_challenge_packet(buf, challenge);
    
    // flush send buffer data to client
    pq_flush();

    // read user login request packet
    if (dq_getmessage(buf, 0) != STATUS_OK) {
        return EOF;
    }
    
    // read authreq packet
    network_mysqld_auth_request *authreq = read_login_request(buf);
    
    port->user_name = authreq->username;
    // support database.schema format later
    port->database_name = pstrdup(g_proto_ctx.database_name.data);

    if (authreq->auth_response) {
        int rc = memcpy_s(token, CLIENT_PASSWORD_TOKEN_LEN + 1, authreq->auth_response, CLIENT_PASSWORD_TOKEN_LEN);
        securec_check(rc, "\0", "\0");
        token[CLIENT_PASSWORD_TOKEN_LEN] = 0x00;
    } 

    if (authreq->schema) {
        StringInfo search_path = makeStringInfo();
        appendStringInfo(search_path, "\"$user\",%s", authreq->schema);
        u_sess->attr.attr_common.namespace_search_path = search_path->data; 
    } 

    pfree(challenge->auth_plugin_name);
    pfree(challenge->server_version_str);
    pfree(challenge);
    pfree(authreq);
    DestroyStringInfo(buf);

    return STATUS_OK;
}

void dophin_client_authentication(Port *port)
{
    dophin_hba_authentication(port);

    network_mysqld_ok_packet_t *ok_packet = make_ok_packet();
    StringInfo buf = makeStringInfo();
    send_network_ok_packet(buf, ok_packet); 
    pq_flush();

    pfree(ok_packet);
    DestroyStringInfo(buf); 
}

void dophin_hba_authentication(Port *port)
{
    int status = STATUS_ERROR;
    /* Database Security: Support password complexity */
    char details[PGAUDIT_MAXLENGTH] = {0};
    errno_t rc = EOK;

    hba_getauthmethod(port);

    /*
     * Now proceed to do the actual authentication check
     */
    switch (port->hba->auth_method) {
        case uaReject: {
            char hostinfo[NI_MAXHOST];
            /* 
             * Audit user login
             * it's unsafe to deal with plugins hooks as dynamic lib may be released 
             */
            if (!(g_instance.status > NoShutdown) && user_login_hook) {
                user_login_hook(port->database_name, port->user_name, false, true);
            }
            rc = snprintf_s(details,
                PGAUDIT_MAXLENGTH,
                PGAUDIT_MAXLENGTH - 1,
                "login db(%s)failed,pg_hba.conf rejects connection for user(%s)",
                port->database_name,
                port->user_name);
            securec_check_ss(rc, "\0", "\0");
            pgaudit_user_login(FALSE, port->database_name, details);

            (void)pg_getnameinfo_all(
                &port->raddr.addr, port->raddr.salen, hostinfo, sizeof(hostinfo), NULL, 0, NI_NUMERICHOST);
#ifdef USE_SSL
                ereport(FATAL,
                    (errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
                        errmsg("pg_hba.conf rejects connection for host \"%s\", user \"%s\", database \"%s\", %s",
                            hostinfo,
                            port->user_name,
                            port->database_name,
                            port->ssl ? _("SSL on") : _("SSL off"))));
#else
                ereport(FATAL,
                    (errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
                        errmsg("pg_hba.conf rejects connection for host \"%s\", user \"%s\", database \"%s\"",
                            hostinfo,
                            port->user_name,
                            port->database_name)));
#endif
                }
            break;
        case uaImplicitReject: {
            /*
             * No matching entry, so tell the user we fell through.
             *
             * NOTE: the extra info reported here is not a security breach,
             * because all that info is known at the frontend and must be
             * assumed known to bad guys.  We're merely helping out the less
             * clueful good guys.
             */
            char hostinfo[NI_MAXHOST];

            (void)pg_getnameinfo_all(
                &port->raddr.addr, port->raddr.salen, hostinfo, sizeof(hostinfo), NULL, 0, NI_NUMERICHOST);

#define HOSTNAME_LOOKUP_DETAIL(port)                                                                                   \
    ((port)->remote_hostname                                                                                           \
            ? (((port)->remote_hostname_resolv == +1)                                                                  \
                      ? errdetail_log(                                                                                 \
                            "Client IP address resolved to \"%s\", forward lookup matches.", (port)->remote_hostname)  \
                      : (((port)->remote_hostname_resolv == 0)                                                         \
                                ? errdetail_log("Client IP address resolved to \"%s\", forward lookup not checked.",   \
                                      (port)->remote_hostname)                                                         \
                                : (((port)->remote_hostname_resolv == -1)                                              \
                                          ? errdetail_log("Client IP address resolved to \"%s\", forward lookup does " \
                                                          "not match.",                                                \
                                                (port)->remote_hostname)                                               \
                                          : 0)))                                                                       \
            : 0)

#ifdef USE_SSL
            ereport(FATAL,
                (errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
                    errmsg("no pg_hba.conf entry for host \"%s\", user \"%s\", database \"%s\", %s",
                        hostinfo,
                        port->user_name,
                        port->database_name,
                        port->ssl ? _("SSL on") : _("SSL off")),
                    HOSTNAME_LOOKUP_DETAIL(port)));
#else
            ereport(FATAL,
                (errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
                    errmsg("no pg_hba.conf entry for host \"%s\", user \"%s\", database \"%s\"",
                        hostinfo,
                        port->user_name,
                        port->database_name),
                    HOSTNAME_LOOKUP_DETAIL(port)));
#endif
            break;
        }

        case uaMD5:
        case uaSHA256:
        case uaSM3: {
            if (!exec_native_password_auth(port)) {
                ereport(ERROR, (errcode(ERRCODE_INVALID_PASSWORD), errmsg("failed in auth check, role:%s", port->user_name))); 
            }
            break;
        }

        case uaKrb5:
        case uaGSS:
        case uaSSPI:
        case uaPeer:
        case uaIdent:
        case uaPAM:
        case uaCert:
        case uaIAM: {
            /* the above authentication methods are not supported for dolphin */
            char    hostinfo[NI_MAXHOST];

            pg_getnameinfo_all(&port->raddr.addr, port->raddr.salen,
                    hostinfo, sizeof(hostinfo),
                    NULL, 0,
                    NI_NUMERICHOST);

            ereport(FATAL,
                (errcode(ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION),
                    errmsg("pg_hba.conf entry specifies unsupported dolphin authentication for host \"%s\", user \"%s\", database \"%s\"",
                    hostinfo, port->user_name, port->database_name),
                    errhint("Supported methods are trust")));
            break;
        }
        case uaTrust:
            status = STATUS_OK;
            break;
        default:
            // default is rdundance.
            break;
    }
}

bool exec_native_password_auth(Port *port)
{
    bool ret = false;
    errno_t rc;
    char *stored_password;
    char scramble_password[AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH + 1];
    char *sha1_scramble_password;
    char stage1_hash[SHA_DIGEST_LENGTH + 1];
    char *stage2_hash;

    // 1.get rollpasswordext from pg_authid
    
    /* Get role info from pg_authid */
    HeapTuple roleTup = SearchSysCache1(AUTHNAME, PointerGetDatum(port->user_name));
    if (!HeapTupleIsValid(roleTup)) {
        return false;
    }

    bool isNull;
    Datum datum = SysCacheGetAttr(AUTHNAME, roleTup, Anum_pg_authid_rolpasswordext, &(isNull));
    if (isNull) {
        ret = false; /*user has no password */
        ereport(WARNING, (errmsg("user '%s' has not set native password, please execute "
            "'select set_native_password('%s', 'password');' first.", port->user_name, port->user_name)));
        goto release;
    }

    stored_password = TextDatumGetCString(datum); 
    char stored_password_bytes[SHA_DIGEST_LENGTH + 1];
    sha1_hex_to_bytes(stored_password, stored_password_bytes);

    // 2. compute SHA1(scramble + rolpasswordext) as PASSWORD
    rc = memcpy_s(scramble_password, AUTH_PLUGIN_DATA_LEN, scramble, AUTH_PLUGIN_DATA_LEN);
    securec_check(rc, "", "");
    rc = memcpy_s(scramble_password + AUTH_PLUGIN_DATA_LEN, SHA_DIGEST_LENGTH, stored_password_bytes, AUTH_PLUGIN_DATA_LEN);
    securec_check(rc, "", "");
    scramble_password[AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH] = 0x00; 

    sha1_scramble_password = TextDatumGetCString(
        DirectFunctionCall1(sha1, CStringGetByteaDatum(scramble_password, AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH)));

    // 3. compute token XOR PASSWORD as stage1_hash
    char sha1_scramble_password_bytes[SHA_DIGEST_LENGTH + 1];
    sha1_hex_to_bytes(sha1_scramble_password, sha1_scramble_password_bytes);
    XOR_between_password(token, sha1_scramble_password_bytes, stage1_hash, SHA_DIGEST_LENGTH);
    stage1_hash[SHA_DIGEST_LENGTH] = 0x00;

    // 4. compare SHA1(statge1_hash) and rolpasswordext
    stage2_hash = TextDatumGetCString(DirectFunctionCall1(sha1, CStringGetByteaDatum(stage1_hash, SHA_DIGEST_LENGTH)));
    if (!strcasecmp(stored_password, stage2_hash)) {
        ret = true;
    } else {
        ereport(WARNING, (errmsg("failed in compare password")));
    }
    
    release:
        ReleaseSysCache(roleTup);
        return ret;
}