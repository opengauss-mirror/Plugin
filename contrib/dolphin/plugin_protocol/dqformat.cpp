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
#include "utils/date.h"
#include "utils/geo_decls.h"
#include "utils/varbit.h"
#include "utils/bytea.h"
#include "access/printtup.h"

#include "plugin_utils/year.h"
#include "plugin_utils/date.h"

#define PRINTABLE_CHARS_COUNT 62
#define HANDSHAKE_RESPONSE_RESERVED_BYTES 23

#define AUTH_PLUGIN_DATA_PART_1 8
#define CAPABILITY_UPPER_BITS 16
#define HANDSHAKEV10_RESERVED_BYTES 10
#define ERR_PACKET_SQLSTATE_LEN 5
#define MAX_ERRMSG_LEN 512
#define DEFAULLT_DATA_FIELD_LEN 100
#define DOLPHIN_BLOB_LENGTH 65535

#define PROTO_DATE_LEN 4
#define PROTO_DATETIME_LEN 7
#define PROTO_DATETIMESTAMP_LEN 11
#define PROTO_TIME_LEN 8
#define PROTE_TIMESTATMP_LEN 12

#define PACKETOFFSET_4 4
#define PACKETOFFSET_8 8

static com_stmt_param* make_stmt_parameters_bytype(int param_count, PreparedStatement *pstmt,
                                                   com_stmt_exec_request *req, StringInfo buf);
static void fill_null_bitmap(HeapTuple spi_tuple, TupleDesc spi_tupdesc, bits8 *null_bitmap);

static char PRINTABLE_CHARS[PRINTABLE_CHARS_COUNT + 1] =
                                "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

network_mysqld_auth_challenge* make_mysqld_handshakev10_packet(char *scramble)
{
    network_mysqld_auth_challenge *challenge =
                                    (network_mysqld_auth_challenge*) palloc0(sizeof(network_mysqld_auth_challenge));
    challenge->capabilities = Dophin_Flags;
    challenge->auth_plugin_name = pstrdup("caching_sha2_password");
    challenge->server_version_str = pstrdup("8.0.28-dophin-server");
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
    scramble[AUTH_PLUGIN_DATA_LEN] = 0x00;

    return challenge;
}

void send_auth_switch_packet(StringInfo buf, char *scramble)
{
    dq_append_int1(buf, 0xfe);
    dq_append_string_null(buf, "mysql_native_password");
    dq_append_string_eof(buf, scramble);
    dq_append_int1(buf, 0x00);
    dq_putmessage(buf->data, buf->len);
}

char *read_switch_response(StringInfo buf)
{
    char *response_data = (char *)palloc0(sizeof(20));
    response_data = dq_get_string_eof(buf);
    return response_data;
}

void send_auth_challenge_packet(StringInfo buf, network_mysqld_auth_challenge *shake)
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

    dq_putmessage(buf->data, buf->len);
}

network_mysqld_auth_request* read_login_request(StringInfo buf, Port* port)
{
    network_mysqld_auth_request *auth = (network_mysqld_auth_request*) palloc0(sizeof(network_mysqld_auth_request));
    dq_get_int4(buf, &auth->client_capabilities);
    if (buf->len != PACKETOFFSET_4) {
        dq_get_int4(buf, &auth->max_packet_size);
        dq_get_int1(buf, &auth->charset);
    } else {
        auth->max_packet_size = 0xfffff;
    }
    if ((auth->client_capabilities & CLIENT_SSL)) {
        uint8 ssl_charset_code = 0;
        if (TlsSecureOpen(port)) {
            return NULL;
        }
        resetStringInfo(buf);
        //Read data after SSL communication
        if (dq_getmessage(buf, 0) != STATUS_OK) {
            DestroyStringInfo(buf);
            return NULL;
        }
        dq_skip_bytes(buf, PACKETOFFSET_8);
        dq_get_int1(buf, &ssl_charset_code);
    }

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

    if ((Dophin_Flags & CLIENT_CONNECT_WITH_DB) &&
        (auth->client_capabilities & CLIENT_CONNECT_WITH_DB)) {
        auth->schema = dq_get_string_null(buf);
    }

    if ((Dophin_Flags & CLIENT_PLUGIN_AUTH) && (auth->client_capabilities & CLIENT_PLUGIN_AUTH)) {
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

/* status flags * int<2>	status_flags	SERVER_STATUS_flags_enum*/
static void sendServerStatus(StringInfo buf)
{
    if (u_sess->proc_cxt.nextQuery) {
        dq_append_int2(buf, SERVER_STATUS_AUTOCOMMIT | SERVER_MORE_RESULTS_EXISTS);
    } else {
        dq_append_int2(buf, SERVER_STATUS_AUTOCOMMIT);
    }
}

void send_network_eof_packet(StringInfo buf)
{
    resetStringInfo(buf);

    dq_append_int1(buf, 0xfe); 
    dq_append_int2(buf, 0x00);       /* warning count */
    sendServerStatus(buf);
    
    dq_putmessage(buf->data, buf->len);
}

void send_new_eof_packet(StringInfo buf)
{
    resetStringInfo(buf);
    if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF) {
        dq_append_int1(buf, 0xfe);
    } else {
        dq_append_int1(buf, 0x00);
    }
    dq_append_int_lenenc(buf, 0);   //affected rows
    dq_append_int_lenenc(buf, 0);   //last insert-id
    if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_PROTOCOL_41) {
        sendServerStatus(buf);
        dq_append_int2(buf, 0x00);       /* warning count *int<2>	warnings	number of warnings*/
    } else if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_TRANSACTIONS) {
        sendServerStatus(buf);
    }
    if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_SESSION_TRACK) {
        dq_append_string_lenenc(buf, "", -1);        // string<lenenc>	info	human readable status information
    }
    if (SERVER_SESSION_STATE_CHANGED) {
        dq_append_string_lenenc(buf, "", -1);  //string<lenenc>	session state info	Session State Information
    } else {
        dq_append_string_eof(buf, "");        //string<EOF>	info	human readable status information
    }
    dq_putmessage(buf->data, buf->len);
}

void send_network_fetch_packet(StringInfo buf)
{
    resetStringInfo(buf);

    dq_append_int1(buf, 0xfe);
    dq_append_int2(buf, 0x00);       /* warning count */
    dq_append_int2(buf, 0x2a);      /* status flags */
    
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

dolphin_column_definition* make_dolphin_column_definition(FormData_pg_attribute *attr, char *tableName)
{
    // FIELD packet
    dolphin_column_definition *field = (dolphin_column_definition *) palloc0(sizeof(dolphin_column_definition));

    // db, table, org_table (tle->resorigtbl), org_name, character_set, decimals will implement later
    field->name = attr->attname.data;
    const TypeItem* item = GetItemByTypeOid(attr->atttypid);
    switch (item->dolphin_type_id) {
        // since mysql-jdbc 5.1.47 don't support enum and set.
        case DOLPHIN_TYPE_ENUM:
        case DOLPHIN_TYPE_SET: {
            field->type = DOLPHIN_TYPE_STRING;
            break;
        }
        default: {
            field->type = item->dolphin_type_id;
            break;
        }
    }
    field->flags = item->flags;
    field->charsetnr = item->charset_flag;
    if (attr->atttypid != BLOBOID) {
        field->length = attr->attalign;
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
    dq_append_int2(buf, field->charsetnr);  /* character_set (2) */
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

static com_stmt_param* make_stmt_parameters_bytype(int param_count, PreparedStatement *pstmt,
                                                   com_stmt_exec_request *req, StringInfo buf)
{
    com_stmt_param *parameters = (com_stmt_param *)palloc0(sizeof(com_stmt_param) * param_count);
    const InputStmtParam *stmt_param = GetCachedInputStmtParamTypes(req->statement_id);
    for (int i = 0; i < param_count; i++) {
        if (param_isnull(i, req->null_bitmap)) continue;
        switch (stmt_param->itypes[i]) {
            case DOLPHIN_TYPE_LONG:
            case DOLPHIN_TYPE_INT24: {
                parameters[i].type = TYPE_INT4;
                dq_get_int4(buf, &parameters[i].value.i4);
                break;
            }
            case DOLPHIN_TYPE_LONGLONG: {
                parameters[i].type = TYPE_INT8;
                dq_get_int8(buf, &parameters[i].value.i8);
                break;
            }
            case DOLPHIN_TYPE_SHORT:
            case DOLPHIN_TYPE_YEAR: {
                parameters[i].type = TYPE_INT2;
                dq_get_int2(buf, &parameters[i].value.i4);
                break;
            }
            case DOLPHIN_TYPE_TINY: {
                parameters[i].type = TYPE_INT1;
                dq_get_int1(buf, &parameters[i].value.i1);
                break;
            }
            case DOLPHIN_TYPE_DOUBLE: {
                parameters[i].type = TYPE_DOUBLE;
                dq_get_int8(buf, &parameters[i].value.d.i8);
                break;
            }
            case DOLPHIN_TYPE_FLOAT: {
                parameters[i].type = TYPE_FLOAT;
                dq_get_int4(buf, &parameters[i].value.f.i4);
                break;
            }
            case DOLPHIN_TYPE_LONG_BLOB:
            case DOLPHIN_TYPE_MEDIUM_BLOB:
            case DOLPHIN_TYPE_BLOB:
            case DOLPHIN_TYPE_TINY_BLOB: {
                parameters[i].value.text = GetCachedParamBlob(req->statement_id);
                parameters[i].type = TYPE_STRING;
                break;
            }
            case DOLPHIN_TYPE_STRING:
            case DOLPHIN_TYPE_VARCHAR:
            case DOLPHIN_TYPE_VAR_STRING:
            case DOLPHIN_TYPE_ENUM:
            case DOLPHIN_TYPE_SET:
            case DOLPHIN_TYPE_GEOMETRY:
            case DOLPHIN_TYPE_BIT:
            case DOLPHIN_TYPE_DECIMAL:
            case DOLPHIN_TYPE_NEWDECIMAL: {
                const TypeItem* item = GetItemByTypeOid(pstmt->plansource->param_types[i]);
                switch (item->dolphin_type_id) {
                    case DOLPHIN_TYPE_BIT: {
                        parameters[i].type = TYPE_HEX;
                        break;
                    }
                    default: {
                        parameters[i].type = TYPE_STRING;
                        break;
                    }
                }
                parameters[i].value.text = dq_get_string_lenenc(buf);
                break;
            }
            case DOLPHIN_TYPE_DATE:
            case DOLPHIN_TYPE_TIMESTAMP:
            case DOLPHIN_TYPE_DATETIME: {
                parameters[i].type = TYPE_STRING;
                uint8 len;
                proto_tm tm;
                dq_get_int1(buf, &len);
                StringInfo text = makeStringInfo();
                if (len == PROTO_DATE_LEN) {
                    dq_get_int2(buf, &tm.year);
                    dq_get_int1(buf, &tm.month);
                    dq_get_int1(buf, &tm.day);
                    appendStringInfo(text, "%d-%d-%d", tm.year, tm.month, tm.day);
                    parameters[i].value.text = text->data;
                } else if (len == PROTO_DATETIME_LEN) {
                    dq_get_int2(buf, &tm.year);
                    dq_get_int1(buf, &tm.month);
                    dq_get_int1(buf, &tm.day);
                    dq_get_int1(buf, &tm.hour);
                    dq_get_int1(buf, &tm.minute);
                    dq_get_int1(buf, &tm.second);
                    appendStringInfo(text, "%d-%d-%d %d:%d:%d",
                                     tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
                    parameters[i].value.text = text->data;
                } else if (len == PROTO_DATETIMESTAMP_LEN) {
                    dq_get_int2(buf, &tm.year);
                    dq_get_int1(buf, &tm.month);
                    dq_get_int1(buf, &tm.day);
                    dq_get_int1(buf, &tm.hour);
                    dq_get_int1(buf, &tm.minute);
                    dq_get_int1(buf, &tm.second);
                    dq_get_int4(buf, &tm.microsecond);
                    appendStringInfo(text, "%d-%d-%d %d:%d:%d.%u",
                                     tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, tm.microsecond);
                    parameters[i].value.text =  text->data;
                }
                break;
            }
            case DOLPHIN_TYPE_TIME: {
                parameters[i].type = TYPE_STRING;
                uint8 len;
                proto_tm tm;
                dq_get_int1(buf, &len);
                StringInfo text = makeStringInfo();
                if (len == PROTO_TIME_LEN) {
                    dq_get_int1(buf, &tm.is_negative);
                    dq_get_int4(buf, &tm.days);
                    dq_get_int1(buf, &tm.hour);
                    dq_get_int1(buf, &tm.minute);
                    dq_get_int1(buf, &tm.second);
                    const char* fmt = tm.is_negative ? "-%d:%d:%d" : "%d:%d:%d";
                    appendStringInfo(text, fmt, tm.hour, tm.minute, tm.second);
                    parameters[i].value.text = text->data;
                } else if (len == PROTE_TIMESTATMP_LEN) {
                    dq_get_int1(buf, &tm.is_negative);
                    dq_get_int4(buf, &tm.days);
                    dq_get_int1(buf, &tm.hour);
                    dq_get_int1(buf, &tm.minute);
                    dq_get_int1(buf, &tm.second);
                    dq_get_int4(buf, &tm.microsecond);
                    const char* fmt = tm.is_negative ? "-%d:%d:%d.%u" : "%d:%d:%d.%u";
                    appendStringInfo(text, fmt, tm.hour, tm.minute, tm.second, tm.microsecond);
                    parameters[i].value.text = text->data;
                }
                break;
            }
            default:
                break;
        }
    }
    return parameters;
}

com_stmt_exec_request* read_com_stmt_exec_request(StringInfo buf)
{
    char stmt_name[NAMEDATALEN];
    com_stmt_exec_request *req = (com_stmt_exec_request *)palloc0(sizeof(com_stmt_exec_request));
    dq_get_int4(buf, &req->statement_id);
    dq_get_int1(buf, &req->flags);
    dq_get_int4(buf, &req->iteration_count);
    int rc = snprintf_s(stmt_name, NAMEDATALEN + 1, NAMEDATALEN, "p%d", req->statement_id);
    securec_check_ss(rc, "\0", "\0");
    PreparedStatement *pstmt = FetchPreparedStatement(stmt_name, true, true);
    int param_count = 0;
    if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_QUERY_ATTRIBUTES) {
        uint64 len;
        param_count = dq_get_int_lenenc(buf, (void *)&len);
    } else {
        param_count = pstmt->plansource->num_params;
    }
    if (param_count > 0) {
        int len = (param_count + 7) / 8;
        req->null_bitmap = dq_get_string_len(buf, len);
        dq_get_int1(buf, &req->new_params_bind_flag);
        if (req->new_params_bind_flag) {
            /* malloc private data using u_sess->cache_mem_cxt */
            MemoryContext oldcontext = MemoryContextSwitchTo(u_sess->cache_mem_cxt);
            InputStmtParam *parameter_types = (InputStmtParam *)palloc0(sizeof(InputStmtParam));
            parameter_types->count = param_count;
            parameter_types->itypes = (uint32 *)palloc0(sizeof(uint32) * param_count);
            for (int i = 0; i < param_count; i++) {
                dq_get_int2(buf, &parameter_types->itypes[i]);
                if (GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_QUERY_ATTRIBUTES) {
                    /*At present, there is no stored parameter name, only read and not used.
                    It will be automatically released after completion*/
                    char* parameter_name = dq_get_string_lenenc(buf);
                }
            }
            SaveCachedInputStmtParamTypes(req->statement_id, parameter_types);
            (void)MemoryContextSwitchTo(oldcontext);
        }
        com_stmt_param *parameters = make_stmt_parameters_bytype(param_count, pstmt, req, buf);
        req->parameter_values = parameters;
        req->param_count = param_count;
    }    
    return req;
}

static void fill_null_bitmap(HeapTuple spi_tuple, TupleDesc spi_tupdesc, bits8 *null_bitmap)
{
    if (HeapTupleHasNulls(spi_tuple)) {
        for (int j = 0; j < spi_tupdesc->natts; j++) {
            if (att_isnull(j, spi_tuple->t_data->t_bits)) {
                int byte_pos = (j + 2) / 8;
                int bit_pos = (j + 2) % 8;
                null_bitmap[byte_pos] |= 1 << bit_pos;
            }
        }
    }    
}

void append_data_by_dolphin_type(const TypeItem *item, Datum binval, StringInfo buf, PrinttupAttrInfo *thisState)
{
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
        case DOLPHIN_TYPE_SHORT: {
            int16 val = DatumGetInt16(binval);
            dq_append_int2(buf, val);
            break;
        }
        case DOLPHIN_TYPE_YEAR: {
            int16 year = DatumGetInt16(binval);
            int16 val = year >= 0 ? YearADT_to_Year(year) : YearADT_to_Year(-year) % 100;
            dq_append_int2(buf, val);
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
            break;
        }
        case DOLPHIN_TYPE_FLOAT: {
            int32 num = DatumGetInt32(binval);
            dq_append_int4(buf, num);
            break;
        }
        case DOLPHIN_TYPE_STRING: {
            /* since all MySQL unknown type are map to DOLPHIN_TYPE_STRING, we use type out func here */
            if (thisState == NULL) {
                /* should not happen */
                ereport(ERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION),
                    errmsg("There is no output function for type: %s(%u)", item->og_typname, item->og_type_oid)));
            }
            char *val = OutputFunctionCall(&thisState->finfo, binval);
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_VARCHAR:
        case DOLPHIN_TYPE_VAR_STRING:
        case DOLPHIN_TYPE_JSON: {
            char *val = TextDatumGetCString(binval);
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_LONG_BLOB:
        case DOLPHIN_TYPE_MEDIUM_BLOB:
        case DOLPHIN_TYPE_BLOB:
        case DOLPHIN_TYPE_TINY_BLOB: {
            u_sess->attr.attr_common.bytea_output = BYTEA_OUTPUT_ESCAPE;
            char *val = DatumGetCString(DirectFunctionCall1(byteaout, binval));
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_BIT: {
            char *val = DatumGetCString(DirectFunctionCall1(bit_out, binval));
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_GEOMETRY: {
            char *val;
            if (item->og_type_oid == POINTOID) {
                val = DatumGetCString(DirectFunctionCall1(point_out, binval));
            } else if (item->og_type_oid == LINEOID) {
                val = DatumGetCString(DirectFunctionCall1(line_out, binval));
            } else if (item->og_type_oid == POLYGONOID) {
                val = DatumGetCString(DirectFunctionCall1(poly_out, binval));
            }
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_DECIMAL:
        case DOLPHIN_TYPE_NEWDECIMAL: {
            char *val = DatumGetCString(DirectFunctionCall1(numeric_out, binval));
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_ENUM: {
            char *val = DatumGetCString(DirectFunctionCall1(enum_out, binval));
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_SET: {
            char *val = DatumGetCString(DirectFunctionCall1(set_out, binval));
            dq_append_string_lenenc(buf, val);
            pfree_ext(val);
            break;
        }
        case DOLPHIN_TYPE_DATE: {
            struct pg_tm tt, *tm = &tt;
            DateADT date = DatumGetDateADT(binval);
            j2date(date + POSTGRES_EPOCH_JDATE, &(tm->tm_year), &(tm->tm_mon), &(tm->tm_mday));
            dq_append_int1(buf, 0x04);
            dq_append_int2(buf, tm->tm_year);
            dq_append_int1(buf, tm->tm_mon);
            dq_append_int1(buf, tm->tm_mday);
            break;
        }

        case DOLPHIN_TYPE_TIMESTAMP: {
            TimestampTz val = DatumGetTimestampTz(binval);
            int tz;
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            const char* tzn = NULL;
            timestamp2tm(val, &tz, tm, &fsec, &tzn, NULL);

            dq_append_int1(buf, 0x0b);
            dq_append_int2(buf, tm->tm_year);
            dq_append_int1(buf, tm->tm_mon);
            dq_append_int1(buf, tm->tm_mday);
            dq_append_int1(buf, tm->tm_hour);
            dq_append_int1(buf, tm->tm_min);
            dq_append_int1(buf, tm->tm_sec);
            dq_append_int4(buf, fsec);
            break;
        }
        case DOLPHIN_TYPE_DATETIME: {
            Timestamp val = DatumGetTimestamp(binval);
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            timestamp2tm(val, NULL, tm, &fsec, NULL, NULL);
            dq_append_int1(buf, 0x0b);
            dq_append_int2(buf, tm->tm_year);
            dq_append_int1(buf, tm->tm_mon);
            dq_append_int1(buf, tm->tm_mday);
            dq_append_int1(buf, tm->tm_hour);
            dq_append_int1(buf, tm->tm_min);
            dq_append_int1(buf, tm->tm_sec);
            dq_append_int4(buf, fsec);
            break;
        }
        case DOLPHIN_TYPE_TIME: {
            TimeADT val = DatumGetTimeADT(binval);
            struct pg_tm tt, *tm = &tt;
            fsec_t fsec;
            time2tm(val, tm, &fsec);

            dq_append_int1(buf, 0x0c);
            val > 0 ? dq_append_int1(buf, 0x00) : dq_append_int1(buf, 0x01);
            dq_append_int4(buf, 0x00);
            dq_append_int1(buf, tm->tm_hour);
            dq_append_int1(buf, tm->tm_min);
            dq_append_int1(buf, tm->tm_sec);
            dq_append_int4(buf, fsec);

            break;
        }
        case DOLPHIN_TYPE_NULL: {
            break;
        }
        default:
            break;
    }
}

void send_binary_protocol_resultset_row(StringInfo buf, SPITupleTable *SPI_tuptable)
{
    TupleDesc spi_tupdesc = SPI_tuptable->tupdesc;
    Form_pg_attribute attrs = spi_tupdesc->attrs;

    for (uint32 i = 0; i < SPI_processed; i++) {
        resetStringInfo(buf);

        // [0x00] packet header
        dq_append_int1(buf, 0x00);

        HeapTuple spi_tuple = SPI_tuptable->vals[i];

        // NULL bitmap, length= (column_count + 7 + 2) / 8
        int len = (spi_tupdesc->natts + 7 + 2) / 8;
        bits8 null_bitmap[len] = {0x00};
        fill_null_bitmap(spi_tuple, spi_tupdesc, null_bitmap);
        
        for (int k = 0; k < len; k++) {
            dq_append_int1(buf, null_bitmap[k]);
        }

        // values for non-null columns
        for (int m = 0; m < spi_tupdesc->natts; m++) {
            bool isnull = false;
            Datum binval = SPI_getbinval(spi_tuple, spi_tupdesc, (int)m + 1, &isnull);
            if (isnull) continue;
            // it's better to reuse dolphin_column_definition here
            const TypeItem *item = GetItemByTypeOid(attrs[m].atttypid);
            append_data_by_dolphin_type(item, binval, buf);
        }
        dq_putmessage(buf->data, buf->len);
    }
}

void read_send_long_data_request(StringInfo buf)
{
    uint32 statement_id;
    uint32 param_id;

    dq_get_int4(buf, &statement_id);
    dq_get_int2(buf, &param_id);
    char *payload = dq_get_string_eof(buf);
    SaveCachedParamBlob(statement_id, payload);
}

void sendRowDescriptionPacket(StringInfo buf, SPITupleTable  *SPI_tuptable)
{
    TupleDesc spi_tupdesc = SPI_tuptable->tupdesc;
    int natts = spi_tupdesc->natts;
    Form_pg_attribute attrs = spi_tupdesc->attrs;

    // FIELD_COUNT packet
    send_field_count_packet(buf, natts);

    // send column_count * column_definition packet
    for (int i = 0; i < natts; ++i) {
        // FIELD packet
        dolphin_column_definition *field = make_dolphin_column_definition(&attrs[i]);
        send_column_definition41_packet(buf, field);
        pfree(field);
    }

    // EOF packet
    if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
        send_network_eof_packet(buf);
    }
}

void send_text_protocol_resultset_row(StringInfo buf, SPITupleTable *SPI_tuptable)
{
    TupleDesc spi_tupdesc = SPI_tuptable->tupdesc;
    int natts = spi_tupdesc->natts;
    Form_pg_attribute attrs = spi_tupdesc->attrs;

    PrinttupAttrInfo *myinfo = (PrinttupAttrInfo *)palloc0(natts * sizeof(PrinttupAttrInfo));

    for (int i = 0; i < natts; i++) {
        PrinttupAttrInfo *thisState = myinfo + i;

        if (attrs[i].attisdropped) {
            attrs[i].atttypid = UNKNOWNOID;
        }
        getTypeOutputInfo(attrs[i].atttypid, &thisState->typoutput, &thisState->typisvarlena);
        fmgr_info(thisState->typoutput, &thisState->finfo);
    }

    for (uint32 i = 0; i < SPI_processed; i++) {
        resetStringInfo(buf);
        HeapTuple spi_tuple = SPI_tuptable->vals[i];

        for (int j = 0; j < natts; ++j) {
            bool isnull = false;
            Datum origattr = SPI_getbinval(spi_tuple, spi_tupdesc, (int)j + 1, &isnull);
            Datum attr = (Datum)0;
            char *outputstr = NULL;

            if (isnull) {
                dq_append_string_lenenc(buf, "");
                continue;
            }

            PrinttupAttrInfo *thisState = myinfo + j;
            attr = thisState->typisvarlena ? PointerGetDatum(PG_DETOAST_DATUM(origattr)) : origattr;

            outputstr = OutputFunctionCall(&thisState->finfo, attr);
            dq_append_string_lenenc(buf, outputstr);

            pfree(outputstr);

            /* Clean up detoasted copy, if any */
            if (DatumGetPointer(attr) != DatumGetPointer(origattr)) {
                pfree(DatumGetPointer(attr));
            }
        }
        dq_putmessage(buf->data, buf->len);
    }
}
