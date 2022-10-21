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
 * dqformat.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/dqformat.h
 * -------------------------------------------------------------------------
 */
#ifndef DQ_FORMAT_H
#define DQ_FORMAT_H

#include "plugin_protocol/proto_com.h"
#include "plugin_protocol/bytestream.h"

#define C(x) x, sizeof(x) - 1

typedef enum {
    NETWORK_MYSQLD_PROTOCOL_VERSION_41
} network_mysqld_protocol_t;

typedef struct network_mysqld_auth_challenge {
    uint8 protocol_version;
    char *server_version_str;
    uint32 server_version;
    uint32 thread_id;
    char auth_plugin_data[AUTH_PLUGIN_DATA_LEN + 1];
    uint32 capabilities;
    uint8 charset;
    uint16 server_status;
    char *auth_plugin_name;
} network_mysqld_auth_challenge;

typedef struct network_mysqld_auth_request {
    uint32 client_capabilities;
    uint32 server_capabilities;
    uint32 max_packet_size;
    uint8 charset;
    char *username;
    char *auth_response;
    char *schema;
    char *auth_plugin_name;
    bool ssl_request;
} network_mysqld_auth_request;

typedef struct {
    uint64 affected_rows;
    uint64 insert_id;
    uint16 server_status;
    uint16 warnings;
    char *msg;
} network_mysqld_ok_packet_t;

typedef struct {
    const char *errmsg;
    const char *sqlstate;
    uint16 errcode;
    network_mysqld_protocol_t version;
} network_mysqld_err_packet_t;

network_mysqld_ok_packet_t* make_ok_packet(uint64 affected_rows = 0, uint64 insert_id = 0, char *msg = "");

void append_auth_challenge_packet(StringInfo buf, network_mysqld_auth_challenge *shake);

void send_network_ok_packet(StringInfo buf, network_mysqld_ok_packet_t *ok_packet);

void send_general_ok_packet();

void send_network_eof_packet(StringInfo buf);

void send_network_err_packet(StringInfo buf, network_mysqld_err_packet_t *err_packet);

void send_field_count_packet(StringInfo buf, int count);

dolphin_data_field* make_dolphin_data_field(const char *name, char *tableName = NULL);

void send_column_definition41_packet(StringInfo buf, dolphin_data_field *field);

#endif /* DQ_FORMAT_H */