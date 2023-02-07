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
#include "utils/builtins.h"

#define PRINTABLE_CHARS_COUNT 62 
#define HANDSHAKE_RESPONSE_RESERVED_BYTES 23

#define AUTH_PLUGIN_DATA_PART_1 8
#define CAPABILITY_UPPER_BITS 16
#define HANDSHAKEV10_RESERVED_BYTES 10
#define ERR_PACKET_SQLSTATE_LEN 5
#define MAX_ERRMSG_LEN 512
#define DEFAULLT_DATA_FIELD_LEN 100
#define DOLPHIN_BLOB_LENGTH 65535

static char PRINTABLE_CHARS[PRINTABLE_CHARS_COUNT + 1] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

network_mysqld_auth_challenge* make_mysqld_handshakev10_packet(char *scramble)
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

    return challenge;
}

void send_auth_challenge_packet(StringInfo buf, network_mysqld_auth_challenge *shake)
{
    uint i;

    resetStringInfo(buf);

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

    dq_putmessage(buf->data, buf->len);
}

network_mysqld_auth_request* read_login_request(StringInfo buf)
{
    network_mysqld_auth_request *auth = (network_mysqld_auth_request*) palloc0(sizeof(network_mysqld_auth_request)); 

    dq_get_int4(buf, &auth->client_capabilities);
    dq_get_int4(buf, &auth->max_packet_size);
    dq_get_int1(buf, &auth->charset);

    dq_skip_bytes(buf, HANDSHAKE_RESPONSE_RESERVED_BYTES);
    auth->username = dq_get_string_null(buf);

    if (auth->client_capabilities & CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) {
        auth->auth_response = dq_get_string_lenenc(buf);
    } else if (auth->client_capabilities & CLIENT_SECURE_CONNECTION) {
        uint8 len;
        dq_get_int1(buf, &len);
        auth->auth_response = dq_get_string_len(buf, len);
    } else {
        auth->auth_response = dq_get_string_null(buf);
    }

    if ((DOPHIN_DEFAULT_FLAGS & CLIENT_CONNECT_WITH_DB) &&
        (auth->client_capabilities & CLIENT_CONNECT_WITH_DB)) {
       auth->schema = dq_get_string_null(buf); 
    }

    if ((DOPHIN_DEFAULT_FLAGS & CLIENT_PLUGIN_AUTH) && (auth->client_capabilities & CLIENT_PLUGIN_AUTH)) {
        auth->auth_plugin_name = dq_get_string_null(buf); 
    }

    return auth; 
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

dolphin_column_definition* make_dolphin_column_definition(const char *name, char *tableName)
{
    dolphin_column_definition *field = (dolphin_column_definition *) palloc0(sizeof(dolphin_column_definition));
    // table, org_table (tle->resorigtbl), org_name, character_set, decimals will implement later
    field->name = name; 
    field->type = DOLPHIN_TYPE_VAR_STRING; // map to atttypid
    field->length = DEFAULLT_DATA_FIELD_LEN; 
    field->db = u_sess->proc_cxt.MyProcPort->database_name;
    field->table = tableName;
    field->org_table = tableName;
    field->charsetnr = 0x2d;

    return field;
} 

dolphin_column_definition* make_dolphin_column_definition(Form_pg_attribute attr, char *tableName)
{
    // FIELD packet
    dolphin_column_definition *field = (dolphin_column_definition *) palloc0(sizeof(dolphin_column_definition));

    // db, table, org_table (tle->resorigtbl), org_name, character_set, decimals will implement later
    field->name = attr->attname.data;
    const TypeItem* item = GetItemByTypeOid(attr->atttypid);
    field->type = item->dolphin_type_id; // map to atttypid
    field->flags = item->flags;
    field->charsetnr = item->charset_flag;
    if (attr->atttypid != BLOBOID) {
        field->length = attr->attlen;
    } else {
        field->length = DOLPHIN_BLOB_LENGTH;
    }

    return field;
}

void send_column_definition41_packet(StringInfo buf, dolphin_column_definition *field)
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

com_stmt_exec_request* read_com_stmt_exec_request(StringInfo buf)
{
    char stmt_name[NAMEDATALEN];

    com_stmt_exec_request *req = (com_stmt_exec_request *)palloc0(sizeof(com_stmt_exec_request));
    dq_get_int4(buf, &req->statement_id);
    dq_get_int1(buf, &req->flags);
    dq_get_int4(buf, &req->iteration_count);

    // if parameter count > 0
    int rc = snprintf_s(stmt_name, NAMEDATALEN + 1, NAMEDATALEN, "p%d", req->statement_id);
    securec_check_ss(rc, "\0", "\0");

    PreparedStatement *pstmt = FetchPreparedStatement(stmt_name, true, true);
    Query *query = (Query *)linitial(pstmt->plansource->query_list);
    int column_count = list_length(query->targetList);
    int param_count = pstmt->plansource->num_params; 

    if (param_count > 0) {
        int len = (column_count + 7) / 8;
        req->null_bitmap = dq_get_string_len(buf, len); 
        dq_get_int1(buf, &req->new_params_bind_flag);
    }

    if (param_count > 0) {
        com_stmt_param *parameters = (com_stmt_param *)palloc0(sizeof(com_stmt_param) * param_count);
        if (req->new_params_bind_flag) {
            // TODO reset parameter typte
            for (int i = 0; i < param_count; i++) {
                dq_get_int2(buf, &parameters[i].type);
            }
        }
        for (int i = 0; i < param_count; i++) {
            if (!param_isnull(i, req->null_bitmap)) {
                // should get value by type
                parameters[i].value = dq_get_string_lenenc(buf);
            } 
        }
        req->parameter_values = parameters;
        req->param_count = param_count;
    }    

    return req;
}

void send_binary_protocol_resultset_row(StringInfo buf, SPITupleTable *SPI_tuptable)
{
    TupleDesc spi_tupdesc = SPI_tuptable->tupdesc;
    Form_pg_attribute *attrs = spi_tupdesc->attrs;

    for (uint32 i = 0; i < SPI_processed; i++) {
        resetStringInfo(buf);

        // [0x00] packet header 
        dq_append_int1(buf, 0x00);

        HeapTuple spi_tuple = SPI_tuptable->vals[i];

        // NULL bitmap, length= (column_count + 7 + 2) / 8
        bits8 *null_bitmap = spi_tuple->t_data->t_bits;
        for (int j = 0; j < BITMAPLEN(spi_tupdesc->natts); j++) {
            dq_append_int1(buf, null_bitmap[j]);
        }

        // values for non-null columns
        for (int k = 0; k < spi_tupdesc->natts; k++) {
            bool isnull = false;
            Datum binval = SPI_getbinval(spi_tuple, spi_tupdesc, (int)k + 1, &isnull);
            if (isnull) continue;
            // it's better to reuse dolphin_column_definition here
            const TypeItem* item = GetItemByTypeOid(attrs[k]->atttypid);
            switch (item->dolphin_type_id) {
                case DOLPHIN_TYPE_LONG:
                case DOLPHIN_TYPE_INT24: {
                    int32 val = DatumGetInt32(binval);
                    dq_append_int4(buf, val);
                    break;
                }
                case DOLPHIN_TYPE_LONGLONG: {
                    int64 val = DatumGetInt64(binval);
                    dq_append_int8(buf, val);
                    break;
                }
                case DOLPHIN_TYPE_SHORT:
                case DOLPHIN_TYPE_YEAR: {
                    int16 val = DatumGetInt16(binval);
                    dq_append_int8(buf, val);
                    break;
                }
                case DOLPHIN_TYPE_TINY: {
                    int8 val = DatumGetInt8(binval);
                    dq_append_int1(buf, val);
                    break;
                }
                case DOLPHIN_TYPE_DOUBLE: {
                    int64 num = DatumGetInt64(binval);
                    dq_append_int8(buf, num);
                    // TODO
                    break;
                }
                case DOLPHIN_TYPE_FLOAT: {
                    int32 num = DatumGetInt32(binval);
                    dq_append_int4(buf, num);
                    // TODO 
                }
                case DOLPHIN_TYPE_STRING:
                case DOLPHIN_TYPE_VARCHAR:
                case DOLPHIN_TYPE_VAR_STRING:
                case DOLPHIN_TYPE_ENUM:
                case DOLPHIN_TYPE_SET:
                case DOLPHIN_TYPE_LONG_BLOB:
                case DOLPHIN_TYPE_MEDIUM_BLOB:
                case DOLPHIN_TYPE_BLOB:
                case DOLPHIN_TYPE_TINY_BLOB:
                case DOLPHIN_TYPE_GEOMETRY:
                case DOLPHIN_TYPE_BIT:
                case DOLPHIN_TYPE_DECIMAL:
                case DOLPHIN_TYPE_NEWDECIMAL: {
                    char *val = TextDatumGetCString(binval);
                    dq_append_string_lenenc(buf, val);
                    break;
                }
                case DOLPHIN_TYPE_DATE:
                case DOLPHIN_TYPE_DATETIME:
                case DOLPHIN_TYPE_TIMESTAMP: {
                    Timestamp val = DatumGetTimestamp(binval);
                    struct pg_tm tt, *tm = &tt;
                    fsec_t fsec;
                    timestamp2tm(val, NULL, tm, &fsec, NULL, NULL);
                    // send date packet 
                    break;
                }
                case DOLPHIN_TYPE_TIME: {
                    break;
                }
                case DOLPHIN_TYPE_NULL: {
                    break;
                }
                default:
                    break;
            }
        }
        dq_putmessage(buf->data, buf->len);
    }
}