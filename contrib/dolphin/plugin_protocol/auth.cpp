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
#include "cipher.h"

#include "catalog/pg_authid.h"
#include "utils/builtins.h"

#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/password.h"
#include "plugin_protocol/auth.h"

extern int SSL_set_fd_ex_wrap(Port* port, int fd);

static char fast_auth_success = '\3';
static char perform_full_authentication = '\4';

uint32 Dophin_Flags = 0;
THR_LOCAL conn_mysql_infoP_t temp_Conn_Mysql_Info = NULL;

static const char mandatory_p1[] = { "ECDHE-ECDSA-AES128-GCM-SHA256:"
                                     "ECDHE-ECDSA-AES256-GCM-SHA384:"
                                     "ECDHE-RSA-AES128-GCM-SHA256:"
                                     "ECDHE-ECDSA-AES128-SHA256:"
                                     "ECDHE-RSA-AES128-SHA256" };

static const char optional_a1[] = { "ECDHE-RSA-AES256-GCM-SHA384:"
                                    "ECDHE-ECDSA-AES256-SHA384:"
                                    "ECDHE-RSA-AES256-SHA384:"
                                    "DHE-RSA-AES128-GCM-SHA256:"
                                    "DHE-DSS-AES128-GCM-SHA256:"
                                    "DHE-RSA-AES128-SHA256:"
                                    "DHE-DSS-AES128-SHA256:"
                                    "DHE-DSS-AES256-GCM-SHA384:"
                                    "DHE-RSA-AES256-SHA256:"
                                    "DHE-DSS-AES256-SHA256:"
                                    "DHE-RSA-AES256-GCM-SHA384" };

static const char optional_a2[] = { "DH-DSS-AES128-GCM-SHA256:"
                                    "ECDH-ECDSA-AES128-GCM-SHA256:"
                                    "DH-DSS-AES256-GCM-SHA384:"
                                    "ECDH-ECDSA-AES256-GCM-SHA384:"
                                    "DH-DSS-AES128-SHA256:"
                                    "ECDH-ECDSA-AES128-SHA256:"
                                    "DH-DSS-AES256-SHA256:"
                                    "ECDH-ECDSA-AES256-SHA384:"
                                    "DH-RSA-AES128-GCM-SHA256:"
                                    "ECDH-RSA-AES128-GCM-SHA256:"
                                    "DH-RSA-AES256-GCM-SHA384:"
                                    "ECDH-RSA-AES256-GCM-SHA384:"
                                    "DH-RSA-AES128-SHA256:"
                                    "ECDH-RSA-AES128-SHA256:"
                                    "DH-RSA-AES256-SHA256:"
                                    "ECDH-RSA-AES256-SHA384" };

static const char optional_d1[] = { "ECDHE-RSA-AES128-SHA:"
                                    "ECDHE-ECDSA-AES128-SHA:"
                                    "ECDHE-RSA-AES256-SHA:"
                                    "ECDHE-ECDSA-AES256-SHA:"
                                    "DHE-DSS-AES128-SHA:"
                                    "DHE-RSA-AES128-SHA:"
                                    "DHE-DSS-AES256-SHA:"
                                    "DHE-RSA-AES256-SHA:"
                                    "DH-DSS-AES128-SHA:"
                                    "ECDH-ECDSA-AES128-SHA:"
                                    "AES256-SHA:"
                                    "DH-DSS-AES256-SHA:"
                                    "ECDH-ECDSA-AES256-SHA:"
                                    "DH-RSA-AES128-SHA:"
                                    "ECDH-RSA-AES128-SHA:"
                                    "DH-RSA-AES256-SHA:"
                                    "ECDH-RSA-AES256-SHA:"
                                    "CAMELLIA256-SHA:"
                                    "CAMELLIA128-SHA:"
                                    "AES128-GCM-SHA256:"
                                    "AES256-GCM-SHA384:"
                                    "AES128-SHA256:"
                                    "AES256-SHA256:"
                                    "AES128-SHA" };

static const char tls_cipher_blocked[] = { "!aNULL:"
                                           "!eNULL:"
                                           "!EXPORT:"
                                           "!LOW:"
                                           "!MD5:"
                                           "!DES:"
                                           "!RC2:"
                                           "!RC4:"
                                           "!PSK:"
                                           "!DES-CBC3-SHA:"
                                           "!DHE-DSS-DES-CBC3-SHA:"
                                           "!DHE-RSA-DES-CBC3-SHA:"
                                           "!ECDH-RSA-DES-CBC3-SHA:"
                                           "!ECDH-ECDSA-DES-CBC3-SHA:"
                                           "!ECDHE-RSA-DES-CBC3-SHA:"
                                           "!ECDHE-ECDSA-DES-CBC3-SHA:"
                                           "!DH-RSA-DES-CBC3-SHA:"
                                           "!DH-DSS-DES-CBC3-SHA" };

static void dophin_hba_authentication(Port *prot);
static bool exec_mysql_password_auth(Port *port);

int dolphin_conn_handshake(Port* port)
{
    if (t_thrd.postmaster_cxt.HaShmData->current_mode == STANDBY_MODE &&
        (!g_instance.attr.attr_network.dolphin_hot_standby || !g_instance.attr.attr_storage.EnableHotStandby)) {
        ereport(ERROR, (errcode(ERRCODE_CANNOT_CONNECT_NOW),
                errmsg("can not accept connection if dolphin hot standby off")));
    }

    StringInfo buf = makeStringInfo();
    u_sess->proc_cxt.nextSeqid = 0;
    temp_Conn_Mysql_Info = (conn_mysql_infoP_t)palloc(sizeof(conn_mysql_info_t));

    // send handshakev10 packet
    network_mysqld_auth_challenge *challenge = make_mysqld_handshakev10_packet(temp_Conn_Mysql_Info->conn_scramble);
    send_auth_challenge_packet(buf, challenge);

    pfree(challenge->auth_plugin_name);
    pfree(challenge->server_version_str);
    pfree(challenge);
    
    // flush send buffer data to client
    pq_flush();

    // read user login request packet
    if (dq_special_getmessage(buf) != STATUS_OK) {
        DestroyStringInfo(buf);
        return EOF;
    }
    
    // read authreq packet
    network_mysqld_auth_request *authreq = read_login_request(buf, port);

    DestroyStringInfo(buf);

    if (authreq == NULL) {
        return EOF;
    }
    temp_Conn_Mysql_Info->client_capabilities = authreq->client_capabilities;
    temp_Conn_Mysql_Info->charset = authreq->charset;
    temp_Conn_Mysql_Info->max_packet_size = authreq->max_packet_size;
    
    port->user_name = authreq->username;
    // support database.schema format later
    port->database_name = pstrdup(g_proto_ctx.database_name.data);

    if (authreq->auth_response) {
        int rc = memcpy_s(temp_Conn_Mysql_Info->conn_sha256_token, CLIENT_PASSWORD_SHA256_TOKEN_LEN + 1,
                          authreq->auth_response, CLIENT_PASSWORD_SHA256_TOKEN_LEN);
        securec_check(rc, "\0", "\0");
        temp_Conn_Mysql_Info->conn_sha256_token[CLIENT_PASSWORD_SHA256_TOKEN_LEN] = 0x00;
    }

    if (authreq->schema) {
        StringInfo search_path = makeStringInfo();
        appendStringInfo(search_path, "\"$user\",%s", authreq->schema);
        SetConfigOption("search_path", search_path->data, PGC_SUSET, PGC_S_OVERRIDE);
        DestroyStringInfo(search_path);
    }

    pfree(authreq);
    return STATUS_OK;
}

void dolphin_client_authentication(Port *port)
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
            if (!exec_mysql_password_auth(port)) {
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

static const char dig_vec_upper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define CHARSETID_10            10
#define CHAROFFSET_4            4
#define PARAMNUM_1              1
#define PARAMNUM_2              2
#define PARAMNUM_3              3
#define DEFAULTMULTIPLE         1000

static char* octet2hex(char* to, const char* str, uint len)
{
    const char* str_end = str + len;
    for (; str != str_end; ++str) {
        *to++ = dig_vec_upper[((unsigned char)*str) >> CHAROFFSET_4];
        *to++ = dig_vec_upper[((unsigned char)*str) & 0x0F];
    }
    *to = '\0';
    return to;
}

static inline uint8 char_val(uint8 X)
{
    return (uint)(X >= '0' && X <= '9' ? X - '0' : X >= 'A' && X <= 'Z' ? X - 'A' +
                  CHARSETID_10 : X - 'a' + CHARSETID_10);
}

static void hex2octet(uint8 *to, const char *str, uint len)
{
    const char *str_end = str + len;
    while (str < str_end) {
        const char tmp = char_val(*str++);
        *to++ = (tmp << CHAROFFSET_4) | char_val(*str++);
    }
}

static void compute_sha2_hash(uint8* digest, const char* buf, size_t len)
{
    EVP_MD_CTX* sha2_context = EVP_MD_CTX_create();
    EVP_DigestInit_ex(sha2_context, EVP_sha256(), NULL);
    EVP_DigestUpdate(sha2_context, buf, len);
    EVP_DigestFinal_ex(sha2_context, digest, NULL);
    EVP_MD_CTX_destroy(sha2_context);
    sha2_context = NULL;
}

inline static void compute_two_stage_sha2_hash(const char* password,
                                               size_t      pass_len,
                                               uint8*      hash_stage1,
                                               uint8*      hash_stage2)
{
    /* Stage 1: hash password */
    compute_sha2_hash(hash_stage1, password, pass_len);

    /* Stage 2 : hash first stage's output. */
    compute_sha2_hash(hash_stage2, (const char*)hash_stage1, SHA2_HASH_SIZE);
}

void make_scrambled_fast_password_sha2(char* to, const char* password, size_t pass_len)
{
    uint8 hash_stage2[SHA2_HASH_SIZE];

    /* Two stage SHA2 hash of the password. */
    compute_two_stage_sha2_hash(password, pass_len, (uint8*)to, hash_stage2);

    /* convert hash_stage2 to hex string */
    octet2hex(to, (const char*)hash_stage2, SHA2_HASH_SIZE);
}

static bool fast_authenticate_mysql_caching_sha2(const char* stored_password, const char* scramble, const char* token)
{
    uint8 scramble_stage1[SHA2_HASH_SIZE] = {0};
    uint8 digest_stage1[SHA2_HASH_SIZE] = {0};
    uint8 digest_stage2[SHA2_HASH_SIZE] = {0};
    uint8 stored_password1[SHA2_HASH_SIZE] = {0};
    uint8 sz_token[SHA2_HASH_SIZE] = {0};
    uint8 sz_scramble[SCRAMBLE_LENGTH] = {0};
    errno_t rc;

    hex2octet(stored_password1, stored_password, SHA2_HASH_SIZE + SHA2_HASH_SIZE);
    rc = memcpy_s(sz_scramble, SCRAMBLE_LENGTH, scramble, SCRAMBLE_LENGTH);
    securec_check(rc, "\0", "\0");
    rc = memcpy_s(sz_token, SHA2_HASH_SIZE, token, SHA2_HASH_SIZE);
    securec_check(rc, "\0", "\0");
    /* SHA2(known, sz_scramble) => scramble_stage1 */
    EVP_MD_CTX* sha2_context_1 = EVP_MD_CTX_create();
    EVP_DigestInit_ex(sha2_context_1, EVP_sha256(), NULL);
    EVP_DigestUpdate(sha2_context_1, stored_password1, SHA2_HASH_SIZE);
    EVP_DigestUpdate(sha2_context_1, sz_scramble, SCRAMBLE_LENGTH);
    EVP_DigestFinal_ex(sha2_context_1, scramble_stage1, NULL);
    EVP_MD_CTX_destroy(sha2_context_1);
    sha2_context_1 = NULL;

    /* XOR(token, scramble_stage1) => digest_stage1 */
    for (unsigned int i = 0; i < SHA2_HASH_SIZE; ++i) {
        digest_stage1[i] = (sz_token[i] ^ scramble_stage1[i]);
    }
    /* SHA2(digest_stage1) => digest_stage2 */
    EVP_MD_CTX* sha2_context_2 = EVP_MD_CTX_create();
    EVP_DigestInit_ex(sha2_context_2, EVP_sha256(), NULL);
    EVP_DigestUpdate(sha2_context_2, digest_stage1, SHA2_HASH_SIZE);
    EVP_DigestFinal_ex(sha2_context_2, digest_stage2, NULL);
    EVP_MD_CTX_destroy(sha2_context_2);
    sha2_context_2 = NULL;
    return (memcmp(stored_password1, (const uint8 *)digest_stage2, SHA2_HASH_SIZE) == 0);
}

static bool search_cached(char *user, char **cachedpassword)
{
    AutoMutexLock UserCachedLinesHashLock(&gUserCachedLinesHashLock);
    UserCachedLinesHashLock.lock();
    UserCachedLinesHash* result =  UserCachedLinesHashTableAccess(HASH_FIND, user, NULL);
    if (result) {
        *cachedpassword = result->fastpasswd;
        UserCachedLinesHashLock.unLock();
        return TRUE;
    }
    UserCachedLinesHashLock.unLock();
    return FALSE;
}

static bool deserialize(const char*serialized_string, char*salt, char*digest, unsigned long *iterations)
{
    char parts[PARAMNUM_3][DOLPHIN_CRYPT_MAX_PASSWORD_SIZE + 1] = { 0 };
    int num_parts = 0;
    errno_t rc = 0;
    char *str_text = (char *)palloc(strlen(serialized_string) + 1);
    rc = strcpy_s(str_text, strlen(serialized_string) + 1, serialized_string);
    securec_check(rc, "\0", "\0");
    char *token = strtok(str_text, "$");
    while (token != NULL && num_parts < PARAMNUM_3) {
        rc = strncpy_s(parts[num_parts], DOLPHIN_CRYPT_MAX_PASSWORD_SIZE, token, strlen(token));
        securec_check(rc, "\0", "\0");
        num_parts++;
        token = strtok(NULL, "$");
    }  
    *iterations = atol(parts[PARAMNUM_1]) * DEFAULTMULTIPLE;
    rc = strncpy_s(salt, SHA1_HASH_SIZE + 1, parts[PARAMNUM_2], DOLPHIN_CRYPT_SALT_LENGTH);
    securec_check(rc, "\0", "\0");
    rc = strncpy_s(digest, DOLPHIN_CRYPT_MAX_PASSWORD_SIZE + 1,
                   (parts[PARAMNUM_2] + DOLPHIN_CRYPT_SALT_LENGTH), DOLPHIN_SHA256_HASH_LENGTH);
    securec_check(rc, "\0", "\0");
    int salt_len = strlen(salt);
    int digest_len = strlen(digest);
    pfree(str_text);
    if (salt_len == DOLPHIN_CRYPT_SALT_LENGTH && digest_len == DOLPHIN_SHA256_HASH_LENGTH) {
        return TRUE;
    }
    return FALSE;
}

static bool full_authenticate_mysql_caching_sha2(const char* stored_password, Port *port)
{
    char        *fast_passwd = (char*)palloc(SHA2_HASH_SIZE * 2 + 1);
    char         salt[SHA1_HASH_SIZE + 1] = { 0 };
    char         digest[DOLPHIN_CRYPT_MAX_PASSWORD_SIZE + 1] = { 0 };
    char         store_digest[DOLPHIN_CRYPT_MAX_PASSWORD_SIZE + 1] = { 0 };
    unsigned long iterations = 0;
    char         *plaintext_passwd = NULL;
    StringInfo buf = makeStringInfo();
    errno_t rc;

    if (!deserialize(stored_password, salt, store_digest, &iterations)) {
        DestroyStringInfo(buf);
        return FALSE;
    }
    dq_append_int1(buf, '\1');
    dq_append_int1(buf, perform_full_authentication);
    dq_putmessage(buf->data, buf->len);
    pq_flush();

    if (dq_getmessage(buf, 0) != STATUS_OK) {
        DestroyStringInfo(buf);
        return FALSE;
    }   
    
    plaintext_passwd = dq_get_string_null(buf);
    char *full_passwd = TextDatumGetCString(DirectFunctionCall2(make_scrambled_full_password_sha2,
        PointerGetDatum(plaintext_passwd), PointerGetDatum(salt)));
    rc = strcpy_s(digest, DOLPHIN_CRYPT_MAX_PASSWORD_SIZE + 1,
                  full_passwd +
                  (DOLPHIN_CRYPT_MAGIC_LENGTH + DOLPHIN_CRYPT_MAGIC_LENGTH + 1 + DOLPHIN_CRYPT_SALT_LENGTH));
    securec_check(rc, "\0", "\0");
    if (memcmp(store_digest, (const uint8 *)digest, strlen(digest)) == 0) {
        //fast authentication
        make_scrambled_fast_password_sha2(fast_passwd, plaintext_passwd, strlen(plaintext_passwd));
        AutoMutexLock UserCachedLinesHashLock(&gUserCachedLinesHashLock);
        UserCachedLinesHashLock.lock();
        UserCachedLinesHash* result = UserCachedLinesHashTableAccess(HASH_ENTER, port->user_name, fast_passwd);
        if (strcmp(result->fastpasswd, fast_passwd)) {
            UserCachedLinesHashLock.unLock();
            return FALSE;
        }
        UserCachedLinesHashLock.unLock();
        pfree(fast_passwd);
        DestroyStringInfo(buf);
        return TRUE;
    } else {
        pfree(fast_passwd);
        DestroyStringInfo(buf);
        return FALSE;
    }
}

bool exec_mysql_password_auth(Port *port)
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
        ereport(WARNING, (errmsg("user '%s' has not set native password or caching sha2 password, please execute "
            "'select set_native_password('%s', 'new_password', 'old_password');' first. || "
            "'select set_caching_sha2_password('%s', 'new_password', 'old_password');' first.",
            port->user_name, port->user_name, port->user_name)));
        goto release;
    }
    stored_password = TextDatumGetCString(datum); 
    // execute corresponding authentication
    if (stored_password[0] == '$') {
        char *cachedpassword = NULL;
        if (search_cached(port->user_name, &cachedpassword)) {
            //fast authentication
            if (!fast_authenticate_mysql_caching_sha2(cachedpassword,
                                                      temp_Conn_Mysql_Info->conn_scramble,
                                                      temp_Conn_Mysql_Info->conn_sha256_token)) {
                ret = false;
                ereport(WARNING, (errmsg("fast authenticate failed!")));
                goto release;
            }
            StringInfo fast_OK = makeStringInfo();
            dq_append_int1(fast_OK, '\1');
            dq_append_int1(fast_OK, fast_auth_success);
            dq_putmessage(fast_OK->data, fast_OK->len);
            pq_flush();
            DestroyStringInfo(fast_OK);
        } else {
            //full authentication
            if (!full_authenticate_mysql_caching_sha2(stored_password, port)) {
                ret = false;
                ereport(WARNING, (errmsg("full authenticate failed!")));
                goto release;
            }
        }
        ret = true;
    } else {
        if ((Dophin_Flags & CLIENT_PLUGIN_AUTH) && (temp_Conn_Mysql_Info->client_capabilities & CLIENT_PLUGIN_AUTH)) {
            StringInfo buf = makeStringInfo();
            char *auth_response = NULL;
            send_auth_switch_packet(buf, temp_Conn_Mysql_Info->conn_scramble);
            // flush send buffer data to client
            pq_flush();
            // read user login request packet
            if (dq_getmessage(buf, 0) != STATUS_OK) {
                return EOF;
            }
            auth_response = read_switch_response(buf);
            rc = memcpy_s(temp_Conn_Mysql_Info->conn_native_token, CLIENT_PASSWORD_NATIVE_TOKEN_LEN + 1,
                auth_response, CLIENT_PASSWORD_NATIVE_TOKEN_LEN);
            securec_check(rc, "\0", "\0");
            temp_Conn_Mysql_Info->conn_native_token[CLIENT_PASSWORD_NATIVE_TOKEN_LEN] = 0x00;
            pfree(auth_response);
            DestroyStringInfo(buf);
        } else {
            rc = memcpy_s(temp_Conn_Mysql_Info->conn_native_token, CLIENT_PASSWORD_NATIVE_TOKEN_LEN + 1,
                temp_Conn_Mysql_Info->conn_sha256_token, CLIENT_PASSWORD_NATIVE_TOKEN_LEN);
            securec_check(rc, "\0", "\0");
            temp_Conn_Mysql_Info->conn_native_token[CLIENT_PASSWORD_NATIVE_TOKEN_LEN] = 0x00;
        }

        char stored_password_bytes[SHA_DIGEST_LENGTH + 1];
        sha1_hex_to_bytes(stored_password, stored_password_bytes);

        // 2. compute SHA1(scramble + rolpasswordext) as PASSWORD
        rc = memcpy_s(scramble_password, AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH + 1,
                      temp_Conn_Mysql_Info->conn_scramble, SHA1_HASH_SIZE + 1);
        securec_check(rc, "", "");
        rc = memcpy_s(scramble_password + AUTH_PLUGIN_DATA_LEN, SHA_DIGEST_LENGTH + 1,
                      stored_password_bytes, SHA_DIGEST_LENGTH);
        securec_check(rc, "", "");
        scramble_password[AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH] = 0x00; 

        sha1_scramble_password = TextDatumGetCString(DirectFunctionCall1(sha1,
            CStringGetByteaDatum(scramble_password, AUTH_PLUGIN_DATA_LEN + SHA_DIGEST_LENGTH)));

        // 3. compute token XOR PASSWORD as stage1_hash
        char sha1_scramble_password_bytes[SHA_DIGEST_LENGTH + 1];
        sha1_hex_to_bytes(sha1_scramble_password, sha1_scramble_password_bytes);
        XOR_between_password(temp_Conn_Mysql_Info->conn_native_token,
                             sha1_scramble_password_bytes, stage1_hash, SHA_DIGEST_LENGTH);
        stage1_hash[SHA_DIGEST_LENGTH] = 0x00;

        // 4. compare SHA1(statge1_hash) and rolpasswordext
        stage2_hash = TextDatumGetCString(DirectFunctionCall1(sha1,
                                                              CStringGetByteaDatum(stage1_hash, SHA_DIGEST_LENGTH)));
        if (!strcmp(stored_password, stage2_hash)) {
            ret = true;
        } else {
            ereport(WARNING, (errmsg("failed in compare password")));
        }
    }
    release:
        ReleaseSysCache(roleTup);
        return ret;
}

/*
  Diffie-Hellman key.
  Generated using: >openssl dhparam -5 -C 2048

  -----BEGIN DH PARAMETERS-----
  MIIBCAKCAQEAil36wGZ2TmH6ysA3V1xtP4MKofXx5n88xq/aiybmGnReZMviCPEJ
  46+7VCktl/RZ5iaDH1XNG1dVQmznt9pu2G3usU+k1/VB4bQL4ZgW4u0Wzxh9PyXD
  glm99I9Xyj4Z5PVE4MyAsxCRGA1kWQpD9/zKAegUBPLNqSo886Uqg9hmn8ksyU9E
  BV5eAEciCuawh6V0O+Sj/C3cSfLhgA0GcXp3OqlmcDu6jS5gWjn3LdP1U0duVxMB
  h/neTSCSvtce4CAMYMjKNVh9P1nu+2d9ZH2Od2xhRIqMTfAS1KTqF3VmSWzPFCjG
  mjxx/bg6bOOjpgZapvB6ABWlWmRmAAWFtwIBBQ==
  -----END DH PARAMETERS-----
 */
static unsigned char dh2048_p[] = {
    0x8A, 0x5D, 0xFA, 0xC0, 0x66, 0x76, 0x4E, 0x61, 0xFA, 0xCA, 0xC0, 0x37, 0x57, 0x5C, 0x6D, 0x3F, 0x83, 0x0A, 0xA1,
    0xF5, 0xF1, 0xE6, 0x7F, 0x3C, 0xC6, 0xAF, 0xDA, 0x8B, 0x26, 0xE6, 0x1A, 0x74, 0x5E, 0x64, 0xCB, 0xE2, 0x08, 0xF1,
    0x09, 0xE3, 0xAF, 0xBB, 0x54, 0x29, 0x2D, 0x97, 0xF4, 0x59, 0xE6, 0x26, 0x83, 0x1F, 0x55, 0xCD, 0x1B, 0x57, 0x55,
    0x42, 0x6C, 0xE7, 0xB7, 0xDA, 0x6E, 0xD8, 0x6D, 0xEE, 0xB1, 0x4F, 0xA4, 0xD7, 0xF5, 0x41, 0xE1, 0xB4, 0x0B, 0xE1,
    0x98, 0x16, 0xE2, 0xED, 0x16, 0xCF, 0x18, 0x7D, 0x3F, 0x25, 0xC3, 0x82, 0x59, 0xBD, 0xF4, 0x8F, 0x57, 0xCA, 0x3E,
    0x19, 0xE4, 0xF5, 0x44, 0xE0, 0xCC, 0x80, 0xB3, 0x10, 0x91, 0x18, 0x0D, 0x64, 0x59, 0x0A, 0x43, 0xF7, 0xFC, 0xCA,
    0x01, 0xE8, 0x14, 0x04, 0xF2, 0xCD, 0xA9, 0x2A, 0x3C, 0xF3, 0xA5, 0x2A, 0x83, 0xD8, 0x66, 0x9F, 0xC9, 0x2C, 0xC9,
    0x4F, 0x44, 0x05, 0x5E, 0x5E, 0x00, 0x47, 0x22, 0x0A, 0xE6, 0xB0, 0x87, 0xA5, 0x74, 0x3B, 0xE4, 0xA3, 0xFC, 0x2D,
    0xDC, 0x49, 0xF2, 0xE1, 0x80, 0x0D, 0x06, 0x71, 0x7A, 0x77, 0x3A, 0xA9, 0x66, 0x70, 0x3B, 0xBA, 0x8D, 0x2E, 0x60,
    0x5A, 0x39, 0xF7, 0x2D, 0xD3, 0xF5, 0x53, 0x47, 0x6E, 0x57, 0x13, 0x01, 0x87, 0xF9, 0xDE, 0x4D, 0x20, 0x92, 0xBE,
    0xD7, 0x1E, 0xE0, 0x20, 0x0C, 0x60, 0xC8, 0xCA, 0x35, 0x58, 0x7D, 0x3F, 0x59, 0xEE, 0xFB, 0x67, 0x7D, 0x64, 0x7D,
    0x8E, 0x77, 0x6C, 0x61, 0x44, 0x8A, 0x8C, 0x4D, 0xF0, 0x12, 0xD4, 0xA4, 0xEA, 0x17, 0x75, 0x66, 0x49, 0x6C, 0xCF,
    0x14, 0x28, 0xC6, 0x9A, 0x3C, 0x71, 0xFD, 0xB8, 0x3A, 0x6C, 0xE3, 0xA3, 0xA6, 0x06, 0x5A, 0xA6, 0xF0, 0x7A, 0x00,
    0x15, 0xA5, 0x5A, 0x64, 0x66, 0x00, 0x05, 0x85, 0xB7,
};

static unsigned char dh2048_g[] = {
    0x05,
};

static DH* get_dh2048(void)
{
    DH* dh;
    if ((dh = DH_new())) {
        BIGNUM* p = BN_bin2bn(dh2048_p, sizeof(dh2048_p), NULL);
        BIGNUM* g = BN_bin2bn(dh2048_g, sizeof(dh2048_g), NULL);
        if (!p || !g || !DH_set0_pqg(dh, p, NULL, g)) {
            DH_free(dh);
            return NULL;
        }
    }
    return (dh);
}

/*
 *  Close SSL connection.
 */
static void close_SSL(Port* port)
{
    if (port->ssl != NULL) {
        SSL_shutdown(port->ssl);
        SSL_free(port->ssl);
        port->ssl = NULL;
    }

    if (port->peer != NULL) {
        X509_free(port->peer);
        port->peer = NULL;
    }

    if (port->peer_cn != NULL) {
        pfree(port->peer_cn);
        port->peer_cn = NULL;
    }
}
static const char* SSLerrmessage(void)
{
    unsigned long errcode;
    const char* errreason = NULL;
    static THR_LOCAL char errbuf[32];

    errcode = ERR_get_error();
    if (errcode == 0) {
        return _("no SSL error reported");
    }
    errreason = ERR_reason_error_string(errcode);
    if (errreason != NULL) {
        return errreason;
    }
    int rcs = snprintf_s(errbuf, sizeof(errbuf), sizeof(errbuf) - 1, _("SSL error code %lu"), errcode);
    securec_check_ss(rcs, "\0", "\0");
    return errbuf;
}

static bool analyze_mysql_certs(Port* port)
{
    void* x509_name      = NULL;
    bool analyze_status = FALSE;
    char subject_name[1024] = { 0 };
    char issuer_name[1024] = { 0 };
    char ciphers[1024] = { 0 };
    if ((port->peer = SSL_get_peer_certificate(port->ssl))) {
        ereport(NOTICE, (errmsg("Obtain client Peer certificate information")));
        X509_NAME_oneline(X509_get_subject_name(port->peer), subject_name, sizeof(subject_name));
        ereport(NOTICE, (errmsg("Obtain client subject information: %s", subject_name)));
        X509_NAME_oneline(X509_get_issuer_name(port->peer), issuer_name, sizeof(issuer_name));
        ereport(NOTICE, (errmsg("Obtain client issuer information: %s", issuer_name)));
        X509_free(port->peer);
        analyze_status = TRUE;
    } else {
        ereport(NOTICE, (errmsg("Peer without certificate, perform encryption suite authentication")));
    }
    if (SSL_get_shared_ciphers(port->ssl, ciphers, sizeof(ciphers))) {
        ereport(NOTICE, (errmsg("Get client shared_ciphers information: %s", ciphers)));
        analyze_status = TRUE;
    } else {
        ereport(NOTICE, (errmsg("No shared password! Authentication failed")));
    }

    return analyze_status;
}

#define SSLTIMEOUT 100
int TlsSecureOpen(Port* port)
{
    int r;
    int err;
    Assert(port->ssl == NULL);
    Assert(port->peer == NULL);
    port->ssl = SSL_new(g_tls_ctx);
    if (!port->ssl) {
        ereport(COMMERROR,
            (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("could not initialize SSL connection: %s", SSLerrmessage())));
        close_SSL(port);
        return -1;
    }
    SSL_clear(port->ssl);
    SSL_SESSION_set_timeout(SSL_get_session(port->ssl), SSLTIMEOUT);
    if (!SSL_set_fd_ex_wrap(port, (int)((intptr_t)(port->sock)))) {
        ereport(
            COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("could not set SSL socket: %s", SSLerrmessage())));
        close_SSL(port);
        return -1;
    }
aloop:
    ERR_clear_error();
    if ((r = SSL_accept(port->ssl)) <= 0) {
        err = SSL_get_error(port->ssl, r);
        switch (err) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
#ifdef WIN32
                pgwin32_waitforsinglesocket(SSL_get_fd(port->ssl),
                    (err == SSL_ERROR_WANT_READ) ? (FD_READ | FD_CLOSE | FD_ACCEPT) : (FD_WRITE | FD_CLOSE),
                    INFINITE);
#endif
                goto aloop;
            case SSL_ERROR_SYSCALL:
                if (r < 0)
                    ereport(COMMERROR, (errcode_for_socket_access(), errmsg("could not accept SSL connection: %m")));
                else
                    ereport(COMMERROR,
                        (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("could not accept SSL connection: EOF detected")));
                break;
            case SSL_ERROR_SSL:
                ereport(COMMERROR,
                    (errcode(ERRCODE_PROTOCOL_VIOLATION),
                        errmsg("could not accept SSL connection: %s", SSLerrmessage())));
                break;
            case SSL_ERROR_ZERO_RETURN:
                ereport(COMMERROR,
                    (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("could not accept SSL connection: EOF detected")));
                break;
            default:
                ereport(
                    COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("unrecognized SSL error code: %d", err)));
                break;
        }
        close_SSL(port);
        return -1;
    }
#ifdef _DEBUG
    PRINTF("ssl communication cipher: %s\n", SSL_get_cipher(port->ssl));
#endif
    if (!analyze_mysql_certs(port)) {
        return -1;
    }
    return 0;
}

#define SSLCACHESIZE 128

static bool tls_secure_ctx(SSL_CTX** ssl_ctx)
{
    errno_t rc = 0;
    struct stat buf;
    DH*        dh;
    GS_UCHAR serverkey[NAMEDATALEN] = { 0 };
    KeyMode keymode = SERVER_MODE;
    StringInfo mtsql_cioher_list = makeStringInfo();
    long       ssl_ctx_options   = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1;
    ssl_ctx_options              = (ssl_ctx_options | 0)
                      & (SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2
                         | SSL_OP_NO_TLSv1_3 | SSL_OP_NO_TICKET);

    int verify = SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE;
    char file_path[MAXPGPATH] = {0};
    char* data_path = NULL;

    appendBinaryStringInfo(mtsql_cioher_list, tls_cipher_blocked, strlen(tls_cipher_blocked));
    appendBinaryStringInfo(mtsql_cioher_list, ":", strlen(":"));
    appendBinaryStringInfo(mtsql_cioher_list, mandatory_p1, strlen(mandatory_p1));
    appendBinaryStringInfo(mtsql_cioher_list, ":", strlen(":"));
    appendBinaryStringInfo(mtsql_cioher_list, optional_a1, strlen(optional_a1));
    appendBinaryStringInfo(mtsql_cioher_list, ":", strlen(":"));
    appendBinaryStringInfo(mtsql_cioher_list, optional_a2, strlen(optional_a2));
    appendBinaryStringInfo(mtsql_cioher_list, ":", strlen(":"));
    appendBinaryStringInfo(mtsql_cioher_list, optional_d1, strlen(optional_d1));
    do {
        if (!(*ssl_ctx = SSL_CTX_new(TLS_server_method()))) {
            ereport(WARNING, (errmsg("SSL environment initialization failed,%s", SSLerrmessage())));
            break;
        }
        if (is_absolute_path(g_proto_ctx.mysql_server_key)) {
            data_path = pstrdup(g_proto_ctx.mysql_server_key);
            get_parent_directory(data_path);
        } else {
            rc = sprintf_s(file_path, MAXPGPATH, "%s/%s", t_thrd.proc_cxt.DataDir, g_proto_ctx.mysql_server_key);
            securec_check_ss(rc, "\0", "\0");
            data_path = pstrdup(file_path);
            get_parent_directory(data_path);
        }
        rc = sprintf_s(file_path, MAXPGPATH, "%s/server.key.cipher", data_path);
        securec_check_ss(rc, "\0", "\0");
        if (stat(file_path, &buf) < 0) {
            ereport(WARNING, (errmsg("could not open file \"%s\": No such file or directory", file_path)));
        } else {
            decode_cipher_files(keymode, NULL, data_path, serverkey);
            SSL_CTX_set_default_passwd_cb_userdata(*ssl_ctx, (char*)serverkey);
        }
        /* Specify encryption algorithm */
        if (SSL_CTX_set_cipher_list(*ssl_ctx, mtsql_cioher_list->data) == 0) {
            ereport(WARNING, (errmsg("Failed to specify encryption algorithm,%s", SSLerrmessage())));
            break;
        }
        /* Load CA certificate */
        if (stat(g_proto_ctx.mysql_ca, &buf) != 0) {
            ereport(WARNING, (errmsg("could not open file \"%s\": No such file or directory", file_path)));
            break;
        }
        if (SSL_CTX_load_verify_locations(*ssl_ctx, g_proto_ctx.mysql_ca, NULL) <= 0) {
            ereport(WARNING, (errmsg("Failed to load CA certificate,%s", SSLerrmessage())));
            break;
        }
        /* Load server certificate */
        if (stat(g_proto_ctx.mysql_server_cert, &buf) != 0) {
            ereport(WARNING, (errmsg("could not open file \"%s\": No such file or directory", file_path)));
            break;
        }
        if (SSL_CTX_use_certificate_file(*ssl_ctx, g_proto_ctx.mysql_server_cert, SSL_FILETYPE_PEM) <= 0) {
            ereport(WARNING, (errmsg("Failed to load server certificate,%s", SSLerrmessage())));
            break;
        }
        if (stat(g_proto_ctx.mysql_server_key, &buf) != 0) {
            ereport(WARNING, (errmsg("could not open file \"%s\": No such file or directory", file_path)));
            break;
        }
        if (SSL_CTX_use_PrivateKey_file(*ssl_ctx, g_proto_ctx.mysql_server_key, SSL_FILETYPE_PEM) <= 0) {
            ereport(WARNING, (errmsg("Failed to load server private key,%s", SSLerrmessage())));
            break;
        }
        /* Check if the server's certificate and private key are paired */
        if (SSL_CTX_check_private_key(*ssl_ctx) == 0) {
            ereport(WARNING, (errmsg("Server certificate private key verification failed,%s", SSLerrmessage())));
            break;
        }
        /* DH stuff */
        dh = get_dh2048();
        if (SSL_CTX_set_tmp_dh(*ssl_ctx, dh) == 0) {
            DH_free(dh);
            ereport(WARNING, (errmsg("%s", SSLerrmessage())));
            break;
        }
        DH_free(dh);
        if (SSL_CTX_set_ecdh_auto(*ssl_ctx, 1) == 0) {
            ereport(WARNING, (errmsg("Failed to set elliptic curve parameters,%s", SSLerrmessage())));
            break;
        }
        SSL_CTX_set_options(*ssl_ctx, ssl_ctx_options);
        if (SSL_CTX_sess_set_cache_size(*ssl_ctx, SSLCACHESIZE) == 0) {
            ereport(WARNING, (errmsg("Failed to set the size of SSL session cache,%s", SSLerrmessage())));
            break;
        }
        SSL_CTX_set_verify(*ssl_ctx, verify, NULL);
        DestroyStringInfo(mtsql_cioher_list);
        Dophin_Flags = DOPHIN_DEFAULT_FLAGS | CLIENT_SSL;
        ereport(LOG, (errmsg("Dolphin plugin successfully initialized SSL information !")));
        return TRUE;
    } while (0);

    Dophin_Flags = DOPHIN_DEFAULT_FLAGS;
    DestroyStringInfo(mtsql_cioher_list);
    return FALSE;
}

bool tls_secure_initialize()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    if (!tls_secure_ctx(&g_tls_ctx)) {
        return FALSE;
    }
    return TRUE;
}