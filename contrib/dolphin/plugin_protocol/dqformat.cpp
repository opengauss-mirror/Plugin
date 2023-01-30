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
 * dqformat.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/dqformat.cpp
 * -------------------------------------------------------------------------
 */
#include "plugin_protocol/dqformat.h"
#include "knl/knl_session.h"
#include "libpq/libpq.h"

#define AUTH_PLUGIN_DATA_PART_1 8
#define CAPABILITY_UPPER_BITS 16
#define HANDSHAKEV10_RESERVED_BYTES 10
#define ERR_PACKET_SQLSTATE_LEN 5
#define MAX_ERRMSG_LEN 512
#define DEFAULLT_DATA_FIELD_LEN 100

void append_auth_challenge_packet(StringInfo buf, network_mysqld_auth_challenge *shake)
{
    uint i;

    dq_append_int1(buf, 0x0a);
    dq_append_string_null(buf, shake->server_version_str); 
    dq_append_int4(buf, shake->thread_id);
    dq_append_string_len(buf, shake->auth_plugin_data, AUTH_PLUGIN_DATA_PART_1);
    dq_append_int1(buf, 0x00); /* filler */
    
    dq_append_int2(buf, shake->capabilities & 0xffff);
    dq_append_int1(buf, shake->charset);
    dq_append_int2(buf, shake->server_status);
    dq_append_int2(buf, (shake->capabilities >> CAPABILITY_UPPER_BITS) & 0xffff);

    if (shake->capabilities & CLIENT_PLUGIN_AUTH) {
        dq_append_int1(buf, AUTH_PLUGIN_DATA_LEN + 1);
    } else {
        dq_append_int1(buf, 0x00);
    }

    /* add the fillers */
    for (i = 0; i < HANDSHAKEV10_RESERVED_BYTES; i++) {
        dq_append_int1(buf, 0x00);
    }

    if (shake->capabilities & CLIENT_SECURE_CONNECTION) {
        dq_append_string_null(buf, shake->auth_plugin_data + AUTH_PLUGIN_DATA_PART_1);
    }

    if (shake->capabilities & CLIENT_PLUGIN_AUTH) {
        dq_append_string_null(buf, shake->auth_plugin_name); 
    }
}

network_mysqld_ok_packet_t* make_ok_packet(uint64 affected_rows, uint64 insert_id, char *msg)
{
    network_mysqld_ok_packet_t* ok_packet = (network_mysqld_ok_packet_t*)palloc0(sizeof(network_mysqld_ok_packet_t));
    ok_packet->affected_rows = affected_rows;
    ok_packet->insert_id = insert_id;
    ok_packet->warnings = 0;
    ok_packet->msg = msg;
    ok_packet->server_status = SERVER_STATUS_AUTOCOMMIT;
    
    return ok_packet;
}

void send_network_ok_packet(StringInfo buf, network_mysqld_ok_packet_t *ok_packet)
{
    dq_append_int1(buf, 0x00);
    dq_append_int_lenenc(buf, ok_packet->affected_rows);
    dq_append_int_lenenc(buf, ok_packet->insert_id);
    dq_append_int2(buf, ok_packet->server_status);
    dq_append_int2(buf, ok_packet->warnings);
    dq_append_string_eof(buf, ok_packet->msg);

    dq_putmessage(buf->data, buf->len);
}

void send_general_ok_packet()
{
    StringInfo buf = makeStringInfo();
    network_mysqld_ok_packet_t* ok_packet = make_ok_packet(0);
    send_network_ok_packet(buf, ok_packet); 

    pfree(ok_packet);
    DestroyStringInfo(buf);
}

void send_network_eof_packet(StringInfo buf)
{
    resetStringInfo(buf);

    dq_append_int1(buf, 0xfe); 
    dq_append_int2(buf, 0x00);       /* warning count */
    dq_append_int2(buf, 0x02);      /* status flags */
    
    dq_putmessage(buf->data, buf->len);
}

void send_network_err_packet(StringInfo buf, network_mysqld_err_packet_t *err_packet)
{
    int errmsg_len;

    dq_append_int1(buf, 0xff); /* ERR */
    dq_append_int2(buf, err_packet->errcode); /* errorcode */
    if (err_packet->version == NETWORK_MYSQLD_PROTOCOL_VERSION_41) {
        dq_append_string_eof(buf, "#");
        if (err_packet->sqlstate && strlen(err_packet->sqlstate) > 0) {
            dq_append_string_len(buf, err_packet->sqlstate, ERR_PACKET_SQLSTATE_LEN);
        } else {
            dq_append_string_len(buf, C("07000"));
        }
    }

    errmsg_len = err_packet->errmsg ? strlen(err_packet->errmsg) : 0;
    if (errmsg_len >= MAX_ERRMSG_LEN) {
        errmsg_len = MAX_ERRMSG_LEN;
    }
    if (errmsg_len > 0) {
        dq_append_string_len(buf, err_packet->errmsg, errmsg_len);
    }

    dq_putmessage(buf->data, buf->len); 
}

void send_field_count_packet(StringInfo buf, int count)
{
    resetStringInfo(buf);
    dq_append_int1(buf, count);
    dq_putmessage(buf->data, buf->len);
}

dolphin_data_field* make_dolphin_data_field(const char *name, char *tableName)
{
    dolphin_data_field *field = (dolphin_data_field *) palloc0(sizeof(dolphin_data_field));
    // table, org_table (tle->resorigtbl), org_name, character_set, decimals will implement later
    field->name = name; 
    field->type = DOLPHIN_TYPE_VAR_STRING; // map to atttypid
    field->length = DEFAULLT_DATA_FIELD_LEN; 
    field->db =  u_sess->proc_cxt.MyProcPort->database_name;
    field->table = tableName;
    field->org_table = tableName;

    return field;
} 

void send_column_definition41_packet(StringInfo buf, dolphin_data_field *field)
{
    resetStringInfo(buf);

    dq_append_string_lenenc(buf, field->catalog ? field->catalog : "def");
    dq_append_string_lenenc(buf, field->db ? field->db : "");
    dq_append_string_lenenc(buf, field->table ? field->table : "");
    dq_append_string_lenenc(buf, field->org_table ? field->org_table : "");
    dq_append_string_lenenc(buf, field->name ? field->name : "");
    dq_append_string_lenenc(buf, field->org_name ? field->org_name : "");

    /* length of the following block, 12 byte */
    dq_append_int_lenenc(buf, 0x0c);        /* next_length (lenenc_int) */
    dq_append_int2(buf, field->charsetnr);               /* character_set (2) */
    dq_append_int4(buf, field->length);       /* column_length (4) */
    dq_append_int1(buf, field->type);        /* column_type (1) */
    dq_append_int2(buf, field->flags);       /* flags (2) */ 
    dq_append_int1(buf, field->decimals);    /* decimals */
    dq_append_int2(buf, 0x00);               /* filler */

    if (field->default_value) {
        dq_append_string_lenenc(buf, field->default_value);
    }
    
    dq_putmessage(buf->data, buf->len);
}

void send_com_stmt_prepare_ok_packet(StringInfo buf, int statementId, int columns, int params)
{
   resetStringInfo(buf); 

   dq_append_int1(buf, 0x00);
   dq_append_int4(buf, statementId);
   dq_append_int2(buf, columns);
   dq_append_int2(buf, params);
   dq_append_int1(buf, 0x00);
   dq_append_int2(buf, 0x00);

   dq_putmessage(buf->data, buf->len);
}