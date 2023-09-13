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
 * password.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/password.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_PASSWORD_H
#define _PROTO_PASSWORD_H

#include "openssl/sha.h"

void sha1_hex_to_bytes(const char b[SHA_DIGEST_LENGTH * 2], char t[SHA_DIGEST_LENGTH]);

#endif