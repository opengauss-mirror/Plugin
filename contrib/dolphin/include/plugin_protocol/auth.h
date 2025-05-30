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

extern int dolphin_conn_handshake(Port* port);
extern void dolphin_client_authentication(Port* port);
bool tls_secure_initialize();
int TlsSecureOpen(Port* port);

extern Datum make_scrambled_full_password_sha2(PG_FUNCTION_ARGS);
extern void get_parent_directory(char* path);
#define SHA256_DIGEST_LENGTH 32

#define DOLPHIN_ROUNDS_DEFAULT          5000
#define DOLPHIN_ROUNDS_MIN              DOLPHIN_ROUNDS_DEFAULT
#define DOLPHIN_ROUNDS_MAX              (0xFFF * 1000)
#define DOLPHIN_MIXCHARS                32
#define DOLPHIN_CRYPT_SALT_LENGTH       20
#define DOLPHIN_CRYPT_MAGIC_LENGTH      3
#define DOLPHIN_CRYPT_PARAM_LENGTH      13
#define DOLPHIN_SHA256_HASH_LENGTH      43
#define DOLPHIN_CRYPT_MAX_PASSWORD_SIZE                          \
    (DOLPHIN_CRYPT_SALT_LENGTH + DOLPHIN_SHA256_HASH_LENGTH +    \
    DOLPHIN_CRYPT_MAGIC_LENGTH + DOLPHIN_CRYPT_PARAM_LENGTH)
#define MAX_PLAINTEXT_LENGTH    256

#define CLIENT_PASSWORD_NATIVE_TOKEN_LEN        (20)
#define CLIENT_PASSWORD_SHA256_TOKEN_LEN        (32)
#define SCRAMBLE_LENGTH                         (20)
#define SHA1_HASH_SIZE                          (20)
#define SHA2_HASH_SIZE                          (32)
#define SCRAMBLED_PASSWORD_CHAR_LENGTH (SCRAMBLE_LENGTH * 2 + 1)

typedef struct UserCachedLinesHash {
    char username[NAMEDATALEN];
    char fastpasswd[DOLPHIN_CRYPT_MAX_PASSWORD_SIZE];
} UserCachedLinesHash;

UserCachedLinesHash* UserCachedLinesHashTableAccess(HASHACTION action, char* user_name, char* fastpassword);

extern pthread_mutex_t gUserCachedLinesHashLock;
extern SSL_CTX* g_tls_ctx;
extern uint32 Dophin_Flags;

#endif  /* auth.h */