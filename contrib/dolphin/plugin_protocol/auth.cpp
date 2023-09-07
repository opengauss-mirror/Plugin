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

#define HANDSHAKE_RESPONSE_RESERVED_BYTES 23
#define PRINTABLE_CHARS_COUNT 62 
#define CLIENT_PASSWORD_TOKEN_LEN 20

static char PRINTABLE_CHARS[PRINTABLE_CHARS_COUNT + 1] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static THR_LOCAL char scramble[AUTH_PLUGIN_DATA_LEN + 1]; 
static THR_LOCAL char token[CLIENT_PASSWORD_TOKEN_LEN + 1];

static void get_login_request(StringInfo packet, network_mysqld_auth_request *auth);
static void dophin_hba_authentication(Port *prot);
static bool exec_native_password_auth(Port *port);

int dophin_conn_handshake(Port* port)
{
    network_mysqld_auth_challenge *challenge = (network_mysqld_auth_challenge*) palloc0(sizeof(network_mysqld_auth_challenge));
    challenge->capabilities = DOPHIN_DEFAULT_FLAGS;
    challenge->auth_plugin_name = pstrdup("mysql_native_password");
    challenge->server_version_str = pstrdup("5.7.38-dophin-server");
    challenge->charset = 0x21;  /* utf8_general_ci */
    challenge->server_status = SERVER_STATUS_AUTOCOMMIT; 
    challenge->thread_id = gs_atomic_add_32(&g_proto_ctx.connect_id, 1);

    // generate a random challenge
    int retval = RAND_priv_bytes((unsigned char*)challenge->auth_plugin_data, AUTH_PLUGIN_DATA_LEN);
    if (retval != 1) {
        ereport(ERROR, (errmsg("Failed to Generate the random number,errcode:%d", retval)));
    }

    // translate random string to printable chars 
    int index;
    for (int i = 0; i < AUTH_PLUGIN_DATA_LEN; i++) {
        index = (unsigned char)challenge->auth_plugin_data[i] % PRINTABLE_CHARS_COUNT; 
        challenge->auth_plugin_data[i] = PRINTABLE_CHARS[index]; 
    }
    challenge->auth_plugin_data[AUTH_PLUGIN_DATA_LEN] = 0x00; 

    errno_t rc = memcpy_s(scramble, AUTH_PLUGIN_DATA_LEN + 1, challenge->auth_plugin_data, AUTH_PLUGIN_DATA_LEN);
    securec_check(rc, "\0", "\0");

    // build handshakev10 packet
    StringInfo packet = makeStringInfo();
    append_auth_challenge_packet(packet, challenge);

    // send handshakev10 packet
    next_seqid = 0;
    dq_putmessage(packet->data, packet->len);
    pq_flush();

    // read user login request packet
    if (dq_getmessage(packet, 0) != STATUS_OK) {
        return EOF;
    } // authreq packet
    network_mysqld_auth_request *authreq = (network_mysqld_auth_request*) palloc0(sizeof(network_mysqld_auth_request)); 
    get_login_request(packet, authreq);

    port->user_name = authreq->username;
    // support database.schema format later
    port->database_name = pstrdup(g_proto_ctx.database_name.data);

    rc = memcpy_s(token, CLIENT_PASSWORD_TOKEN_LEN + 1, authreq->auth_response, CLIENT_PASSWORD_TOKEN_LEN);
    securec_check(rc, "\0", "\0");
    token[CLIENT_PASSWORD_TOKEN_LEN] = 0x00; 

    if (authreq->schema) {
        StringInfo search_path = makeStringInfo();
        appendStringInfo(search_path, "\"$user\",%s", authreq->schema);
        u_sess->attr.attr_common.namespace_search_path = search_path->data; 
    } 

    pfree(challenge);
    pfree(authreq);
    DestroyStringInfo(packet);

    return STATUS_OK;
}

void get_login_request(StringInfo packet, network_mysqld_auth_request *auth)
{
    dq_get_int4(packet, &auth->client_capabilities);
    dq_get_int4(packet, &auth->max_packet_size);
    dq_get_int1(packet, &auth->charset);

    dq_skip_bytes(packet, HANDSHAKE_RESPONSE_RESERVED_BYTES);
    auth->username = dq_get_string_null(packet);

    if (auth->client_capabilities & CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
        auth->auth_response = dq_get_string_lenenc(packet);
    } else if (auth->client_capabilities & CLIENT_SECURE_CONNECTION) {
        uint8 len;
        dq_get_int1(packet, &len);
        auth->auth_response = dq_get_string_len(packet, len);
    } else {
        auth->auth_response = dq_get_string_null(packet);
    }

    if ((DOPHIN_DEFAULT_FLAGS & CLIENT_CONNECT_WITH_DB) &&
        (auth->client_capabilities & CLIENT_CONNECT_WITH_DB)) {
       auth->schema = dq_get_string_null(packet); 
    }

    if ((DOPHIN_DEFAULT_FLAGS & CLIENT_PLUGIN_AUTH) && (auth->client_capabilities & CLIENT_PLUGIN_AUTH)) {
        auth->auth_plugin_name = dq_get_string_null(packet); 
    }
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

    // 2. compute SHA1(scramble + rolpasswordext) as PASSWORD
    rc = memcpy_s(scramble_password, AUTH_PLUGIN_DATA_LEN, scramble, AUTH_PLUGIN_DATA_LEN);
    securec_check(rc, "", "");
    rc = memcpy_s(scramble_password + AUTH_PLUGIN_DATA_LEN, SHA_DIGEST_LENGTH, sha1_hex_to_bytes(stored_password), AUTH_PLUGIN_DATA_LEN);
    securec_check(rc, "", "");
    scramble_password[AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH] = 0x00; 

    sha1_scramble_password = TextDatumGetCString(DirectFunctionCall1(sha1, CStringGetTextDatum(scramble_password)));

    // 3. compute token XOR PASSWORD as stage1_hash
    XOR_between_password(token, sha1_hex_to_bytes(sha1_scramble_password), stage1_hash, SHA_DIGEST_LENGTH);
    stage1_hash[SHA_DIGEST_LENGTH] = 0x00;

    // 4. compare SHA1(statge1_hash) and rolpasswordext
    stage2_hash = TextDatumGetCString(DirectFunctionCall1(sha1, CStringGetTextDatum(stage1_hash)));
    if (!strcasecmp(stored_password, stage2_hash)) {
        ret = true;
    } else {
        ereport(WARNING, (errmsg("failed in compare password")));
    }
    
    release:
        ReleaseSysCache(roleTup);
        return ret;
}