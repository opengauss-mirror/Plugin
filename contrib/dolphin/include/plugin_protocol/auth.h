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
 * auth.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/auth.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_AUTH_H
#define _PROTO_AUTH_H

#include "postgres.h"
#include "libpq/libpq.h"
#include "openssl/ssl.h"
#include "openssl/dh.h"

extern int dophin_conn_handshake(Port* port);
extern void dophin_client_authentication(Port* port);
bool tls_secure_initialize();
int TlsSecureOpen(Port* port);

extern Datum make_scrambled_full_password_sha2(PG_FUNCTION_ARGS);
#define SHA256_DIGEST_LENGTH 32

#define ROUNDS_DEFAULT          5000
#define ROUNDS_MIN              ROUNDS_DEFAULT
#define ROUNDS_MAX              (0xFFF * 1000)
#define MIXCHARS                32
#define CRYPT_SALT_LENGTH       20
#define CRYPT_MAGIC_LENGTH      3
#define CRYPT_PARAM_LENGTH      13
#define SHA256_HASH_LENGTH      43
#define CRYPT_MAX_PASSWORD_SIZE (CRYPT_SALT_LENGTH + SHA256_HASH_LENGTH + CRYPT_MAGIC_LENGTH + CRYPT_PARAM_LENGTH)
#define MAX_PLAINTEXT_LENGTH    256

#define CLIENT_PASSWORD_NATIVE_TOKEN_LEN        (20)
#define CLIENT_PASSWORD_SHA256_TOKEN_LEN        (32)
#define SCRAMBLE_LENGTH                         (20)
#define SHA1_HASH_SIZE                          (20)
#define SHA2_HASH_SIZE                          (32)
#define SCRAMBLED_PASSWORD_CHAR_LENGTH (SCRAMBLE_LENGTH * 2 + 1)

typedef struct UserCachedLinesHash {
    char username[NAMEDATALEN];
    char fastpasswd[CRYPT_MAX_PASSWORD_SIZE];
} UserCachedLinesHash;

UserCachedLinesHash* UserCachedLinesHashTableAccess(HASHACTION action, char* user_name, char* fastpassword);

extern pthread_mutex_t gUserCachedLinesHashLock;
extern SSL_CTX* g_tls_ctx;
extern uint32 Dophin_Flags;

#endif  /* auth.h */