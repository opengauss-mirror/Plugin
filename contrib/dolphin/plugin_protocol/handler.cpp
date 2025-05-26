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
 * handler.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/handler.cpp
 * -------------------------------------------------------------------------
 */
#include "access/xact.h"
#include "miscadmin.h"
#include "libpq/pqformat.h"
#include "libpq/libpq.h"
#include "executor/spi_priv.h"
#include "commands/prepare.h"
#include "commands/sequence.h"

#include "plugin_postgres.h"
#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/handler.h"

#define CHAR_SET_RESULTS_LEN 25
#define SELECT_TAG_LEN 6
#define SHOW_TAG_LEN 4
#define EXPLAIN_TAG_LEN 7

static int execute_text_protocol_sql(const char* sql);
static int execute_com_stmt_prepare(StringInfo buf);
static void execute_binary_protocol_req_process_b(StringInfo buf);
static void execute_binary_protocol_req_process_e();
static void remove_cached_stmt_data(uint32 *statement_id);
static void execute_com_stmt_close(StringInfo buf);
static void execute_com_stmt_reset(StringInfo buf);
static void execute_com_field_list(char *tableName);
static bool parse_query_bind_params(uint param_count,
                                    unsigned char **inout_read_pos,
                                    size_t *inout_packet_left,
                                    bool receive_named_params,
                                    bool receive_parameter_set_count);

void dophin_send_ready_for_query(CommandDest dest)
{
    switch (dest) {
        case DestRemote:
        case DestRemoteExecute:
        case DestTupleBroadCast:
        case DestTupleRedistribute:
        case DestBatchBroadCast:
        case DestBatchLocalBroadCast:
        case DestBatchRedistribute:
            /* Flush output at end of cycle in any case. */
            pq_flush();

            break;

        case DestNone:
        case DestDebug:
        case DestSPI:
        case DestSPITupleAnalyze:
        case DestTuplestore:
        case DestIntoRel:
        case DestCopyOut:
        case DestSQLFunction:
        case DestTransientRel:
        case DestTrainModel:
        default:
            break;
    }
}

void dolphin_end_command(const char *completionTag)
{
    StringInfo buf = makeStringInfo();
    if (strncasecmp(completionTag, "SELECT", SELECT_TAG_LEN) == 0 ||
        strncasecmp(completionTag, "SHOW", SHOW_TAG_LEN) == 0 ||
        strncasecmp(completionTag, "EXPLAIN", EXPLAIN_TAG_LEN) == 0 ) {
        // EOF packet
        if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
            send_network_eof_packet(buf);
        } else {
            send_new_eof_packet(buf);
        }
    } else {
        int64 affected_rows = u_sess->statement_cxt.current_row_count;
        uint64 last_insert_id = u_sess->cmd_cxt.last_insert_id;
        network_mysqld_ok_packet_t *ok_packet = make_ok_packet(affected_rows, last_insert_id);
        send_network_ok_packet(buf, ok_packet); 
        pfree(ok_packet);
    }
    DestroyStringInfo(buf);
}

void dolphin_send_message(ErrorData *edata)
{
    if (edata->elevel < ERROR) {
        return;
    }

    network_mysqld_err_packet_t *err_packet =
        (network_mysqld_err_packet_t *)palloc0(sizeof(network_mysqld_err_packet_t));
    err_packet->errcode = get_dolphin_errcode(edata->sqlerrcode);
    err_packet->errmsg = edata->message;
    err_packet->sqlstate = "HY000";   // convert errcode to mysql SQLSTATE later

    StringInfo buf = makeStringInfo();
    send_network_err_packet(buf, err_packet);

    pfree(err_packet);
    DestroyStringInfo(buf);
}

int dophin_read_command(StringInfo buf)
{
    return dq_getmessage(buf, 0);
}

static void free_com_stmt_exec_request(com_stmt_exec_request *req)
{
    pfree_ext(req->parameter_values);
    pfree_ext(req->null_bitmap);
    pfree(req);
}

int dolphin_process_command(StringInfo buf)
{
    GetSessionContext()->enableBCmptMode = true;
    u_sess->attr.attr_common.enable_set_variable_b_format = true;

    uint8 command = pq_getmsgbyte(buf);

    switch (command) {
        case COM_PING: {
            send_general_ok_packet();
            break;
        }
        case COM_QUIT: {
            proc_exit(0);
            break;
        }
        case COM_INIT_DB: {
            char *schemaName = dq_get_string_eof(buf);
            StringInfo sql = makeStringInfo();
            appendStringInfo(sql, "use %s", schemaName);
            execute_text_protocol_sql(sql->data);
            break;
        }
        case COM_QUERY: {
            size_t sql_len =  buf->len - buf->cursor;
            unsigned char* sql = (unsigned char*)dq_get_string_len(buf, buf->len - buf->cursor);
            parse_query_bind_params(/*THD *thd, */0, &sql, &sql_len, TRUE, TRUE);
            if (strcmp((const char*)sql, "select @@version_comment limit 1") == 0) {
                execute_text_protocol_sql("select \'dolphin\'");
            } else {
                execute_simple_query((const char*)sql);
            }
            break;
        }
        case COM_FIELD_LIST: {
            char *tableName = dq_get_string_null(buf);
            execute_com_field_list(tableName);
            break;
        }
        case COM_STMT_PREPARE: {
            execute_com_stmt_prepare(buf);
            break;
        }
        case COM_STMT_EXECUTE: {
            GetSessionContext()->is_binary_proto = true;
            execute_binary_protocol_req_process_b(buf);
            execute_binary_protocol_req_process_e();
            GetSessionContext()->is_binary_proto = false;
            break;
        }
        case COM_STMT_SEND_LONG_DATA: {
            read_send_long_data_request(buf);
            break;
        }
        case COM_STMT_RESET: {
            execute_com_stmt_reset(buf);
            break;
        }
        case COM_STMT_CLOSE: {
            execute_com_stmt_close(buf);
            break;
        }
        default:
            // COM_CREATE_DB, COM_DROP_DB have deprecated by mysql-server
            ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("dolphin server protocol not supported.")));
    }
    return 0;
}

void dolphin_comm_reset(void)
{
    if (temp_Conn_Mysql_Info) {
        pfree(temp_Conn_Mysql_Info);
        temp_Conn_Mysql_Info = NULL;
    }
}

#define CHARSET_251         251
#define CHARSET_252         252
#define CHARSET_253         253
#define CHARSETNUM_0        0
#define CHARSETNUM_1        1
#define CHARSETNUM_2        2
#define CHARSETNUM_3        3
#define CHARSETNUM_4        4
#define CHARSETNUM_8        8
#define CHARSETNUM_9        9
#define CHARSETNUM_16       16
#define PARAMNUM            65535

static uint16 uint2korr(const unsigned char *A)
{
    errno_t rc = 0;
    uint16 ret;
    rc = memcpy_s(&ret, sizeof(ret), A, sizeof(ret));
    securec_check(rc, "\0", "\0");
    return ret;
}

static uint32 uint3korr(const unsigned char *A)
{
    return (uint32)(((uint32)(A[CHARSETNUM_0])) +
           (((uint32)(A[CHARSETNUM_1])) << CHARSETNUM_8) + (((uint32)(A[CHARSETNUM_2])) << CHARSETNUM_16));
}

static uint32 uint4korr(const unsigned char *A)
{
    errno_t rc = 0;
    uint32 ret;
    rc = memcpy_s(&ret, sizeof(ret), A, sizeof(ret));
    securec_check(rc, "\0", "\0");
    return ret;
}

/* Get the length of next field. Change parameter to point at fieldstart */
ulong net_field_length(unsigned char **packet)
{
    const unsigned char *pos = *packet;
    if (*pos < CHARSET_251) {
        (*packet)++;
        return (ulong)*pos;
    }
    if (*pos == CHARSET_251) {
        (*packet)++;
        return ((unsigned long)~0);
    }
    if (*pos == CHARSET_252) {
        (*packet) += CHARSETNUM_3;
        return (ulong)uint2korr(pos + 1);
    }
    if (*pos == CHARSET_253) {
        (*packet) += CHARSETNUM_4;
        return (ulong)uint3korr(pos + 1);
    }
    (*packet) += CHARSETNUM_9; /* Must be 254 when here */
    return (ulong)uint4korr(pos + 1);
}

uint net_field_length_size(const unsigned char *pos)
{
    if (*pos <= CHARSET_251) {
        return CHARSETNUM_1;
    }
    if (*pos == CHARSET_252) {
        return CHARSETNUM_3;
    }
    if (*pos == CHARSET_253) {
        return CHARSETNUM_4;
    }
    return CHARSETNUM_9;
}

static bool parse_query_bind_params(uint param_count,
                                    unsigned char **inout_read_pos,
                                    size_t *inout_packet_left,
                                    bool receive_named_params,
                                    bool receive_parameter_set_count)
{
    unsigned char *read_pos = *inout_read_pos;
    size_t packet_left = *inout_packet_left;

    if (receive_named_params) {
        unsigned long n_params = 0, n_sets;
        if (packet_left < 1 || packet_left < net_field_length_size(read_pos)) {
            return true;
        }
        unsigned char *pre = read_pos;
        /* read the number of params */
        n_params = net_field_length(&read_pos);
        packet_left -= read_pos - pre;

        if (receive_parameter_set_count) {
            if (packet_left < 1 || packet_left < net_field_length_size(read_pos)) {
                return true;
            }
            pre = read_pos;
            n_sets = net_field_length(&read_pos);
            packet_left -= read_pos - pre;
            if (n_sets != 1) {
                return true;
            }
        }

        /* Cap the param count to 64k. Should be enough for everybody! */
        if (n_params > PARAMNUM) {
            return true;
        }
        param_count = n_params;
    }

    *inout_read_pos = read_pos;
    *inout_packet_left = packet_left;
    return false;
}

int execute_text_protocol_sql(const char *sql)
{
    int rc;
    
    start_xact_command();

    if (!u_sess->attr.attr_storage.phony_autocommit) {
        BeginTxnForAutoCommitOff();
    }

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    rc = SPI_execute(sql, false, 0);

    StringInfo buf = makeStringInfo();
    if (SPI_tuptable) {
        sendRowDescriptionPacket(buf, SPI_tuptable);

        // send None or many text Protocol Resultset Row packet
        if (SPI_processed > 0) {
            send_text_protocol_resultset_row(buf, SPI_tuptable);
        }

        // EOF packet
        if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
            send_network_eof_packet(buf);
        } else {
            send_new_eof_packet(buf);
        }
    } else {
        uint64 last_insert_id = u_sess->cmd_cxt.last_insert_id;
        network_mysqld_ok_packet_t *ok_packet = make_ok_packet(SPI_processed, last_insert_id);
        send_network_ok_packet(buf, ok_packet); 
        pfree(ok_packet);
    }

    DestroyStringInfo(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

    return 0;
}

int execute_com_stmt_prepare(StringInfo buf)
{
    OgRecordAutoController _local_opt(SRT6_P);
    char *client_sql = dq_get_string_eof(buf);
    CachedPlanSource *psrc = NULL;
    PreparedStatement *pstmt = NULL;
    int32 statement_id = gs_atomic_add_32(&g_proto_ctx.statement_id, 1);
    char stmt_name[NAMEDATALEN];
    int rc = sprintf_s(stmt_name, NAMEDATALEN, "%s%d", DOLPHIN_PROTOCOL_STMT_NAME_PREFIX, statement_id);
    securec_check_ss(rc, "", "");

    exec_pre_parse_message();
    exec_parse_message(client_sql, stmt_name, NULL, NULL, NULL, 0, false);
    get_prepared_statement(stmt_name, &pstmt, &psrc);
    int column_count = psrc->resultDesc == NULL ? 0 : psrc->resultDesc->natts;
    int param_count = psrc->num_params;

    StringInfo sql = makeStringInfo();
    send_com_stmt_prepare_ok_packet(sql, statement_id, column_count, param_count);

    for (int i = 0; i < param_count; i++) {
        dolphin_column_definition* param_field = make_dolphin_column_definition("?");
        const TypeItem* item = GetItemByTypeOid(psrc->param_types[i]);
        param_field->type = item->dolphin_type_id;
        param_field->charsetnr = item->charset_flag;
        send_column_definition41_packet(sql, param_field);
    }
    if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
        send_network_eof_packet(sql);
    }
    
    for (int i = 0; i < column_count; i++) {
        dolphin_column_definition* column_field = make_dolphin_column_definition("");
        send_column_definition41_packet(sql, column_field);
    }
    if (column_count != 0 && !(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
        send_network_eof_packet(sql);
    }
    DestroyStringInfo(sql);
    pfree_ext(client_sql);
    exec_after_parse_message(stmt_name);
    return 0;
}

void dolphin_get_param_list_info(BindMessage* pqBindMessage, CachedPlanSource* psrc, ParamListInfo* params,
    MemoryContext paramCtx, MemoryContext valueCtx)
{
    int numParams = pqBindMessage->numParams;
    /*
     * Fetch parameters, if any, and store in the portal's memory context.
     */
    if (numParams <= 0) {
        return;
    }

    MemoryContext oldCtx = MemoryContextSwitchTo(paramCtx);
    if (*params == NULL) {
        *params = (ParamListInfo)palloc(offsetof(ParamListInfoData, params) + numParams * sizeof(ParamExternData));
        /* we have static list of params, so no hooks needed */
        (*params)->paramFetch = NULL;
        (*params)->paramFetchArg = NULL;
        (*params)->parserSetup = NULL;
        (*params)->parserSetupArg = NULL;
        (*params)->params_need_process = false;
        (*params)->numParams = numParams;
        (*params)->uParamInfo = DEFUALT_INFO;
        (*params)->params_lazy_bind = false;
    }

    MemoryContextSwitchTo(valueCtx);
    com_stmt_param *param = (com_stmt_param *)pqBindMessage->pvalue;
    for (int i = 0; i < numParams; i++) {
        bool isNull = false;
        Datum pval = 0;
        Oid ptype = psrc->param_types[i];
        Oid sendType = InvalidOid;
        switch (param[i].type) {
            case TYPE_STRING: {
                if (param[i].value.text) {
                    Oid typinput;
                    Oid typioparam;
                    getTypeInputInfo(ptype, &typinput, &typioparam);
                    pval = OidInputFunctionCall(typinput, (char*)param[i].value.text, typioparam, -1);
                } else {
                    isNull = true;
                }
                break;
            }
            case TYPE_INT1: {
                sendType = INT1OID;
                pval = Int8GetDatum(param[i].value.i1);
                break;
            }
            case TYPE_INT2:
            case TYPE_INT4: {
                sendType = INT4OID;
                pval = Int32GetDatum(param[i].value.i4);
                break;
            }
            case TYPE_INT8: {
                sendType = INT8OID;
                pval = Int64GetDatum(param[i].value.i8);
                break;
            }
            case TYPE_FLOAT: {
                sendType = FLOAT4OID;
                pval = Float4GetDatum(param[i].value.f.f4);
                break;
            }
            case TYPE_DOUBLE: {
                sendType = FLOAT8OID;
                pval = Float8GetDatum(param[i].value.d.f8);
                break;
            }
            case TYPE_HEX: {
                sendType = VARBITOID;
                size_t value_len = strlen(param[i].value.text);
                StringInfo sql = makeStringInfo();
                appendStringInfo(sql, "x'");
                for (size_t j = 0; j < value_len; j++) {
                    appendStringInfo(sql, "%x", param[i].value.text[j]);
                }
                appendStringInfo(sql, "'");
                Oid typinput;
                Oid typioparam;
                getTypeInputInfo(VARBITOID, &typinput, &typioparam);
                pval = OidInputFunctionCall(typinput, (char*)sql->data, typioparam, -1);
                DestroyStringInfo(sql);
                break;
            }
            default:
                isNull = true;
                break;
        }

        if (!isNull && sendType != InvalidOid && sendType != ptype) {
            /* maybe we have better way to deal with these data type transform? */
            Oid castFunc;
            /*
             * actually we can use COERCION_EXPLICIT here, but in find_coercion_pathway, COERCION_EXPLICIT will do many
             * other things we don't want, so just give COERCION_EXPLICIT, it's seems ok now.
             */
            CoercionPathType pathtype = find_coercion_pathway(ptype, sendType, COERCION_ASSIGNMENT, &castFunc);
            switch (pathtype) {
                case COERCION_PATH_NONE:
                    ereport(ERROR, (errmsg("could not cast source type: %d to target type: %d.", sendType, ptype)));
                    break;
                case COERCION_PATH_FUNC:
                    pval = OidFunctionCall1(castFunc, pval);
                    break;
                case COERCION_PATH_RELABELTYPE:
                    /* binary equal, noting to do */
                    break;
                case COERCION_PATH_COERCEVIAIO:
                case COERCION_PATH_ARRAYCOERCE: {
                    Oid typinout;
                    Oid typioparam;
                    bool typIsVarlena = false;

                    getTypeOutputInfo(sendType, &typinout, &typIsVarlena);
                    char* result = OidOutputFunctionCall(typinout, pval);

                    getTypeInputInfo(ptype, &typinout, &typioparam);
                    pval = OidInputFunctionCall(typinout, result, typioparam, -1);
                    pfree_ext(result);
                    break;
                }
                default:
                    /* should not happen */
                    ereport(ERROR, (errmsg("Unknown coerce path type: %d.", pathtype)));
                    break;
            }
        }
        (*params)->params[i].value = pval;
        (*params)->params[i].isnull = isNull;

        /*
         * We mark the params as CONST.  This ensures that any custom plan
         * makes full use of the parameter values.
         */
        (*params)->params[i].pflags = PARAM_FLAG_CONST;
        (*params)->params[i].ptype = ptype;
        (*params)->params[i].tabInfo = NULL;
    }
    MemoryContextSwitchTo(oldCtx);
}

static void execute_binary_protocol_req_process_b(StringInfo buf)
{
    OgRecordAutoController _local_opt_b(SRT7_B);

    exec_pre_bind_message();

    com_stmt_exec_request *req = read_com_stmt_exec_request(buf);
    char stmt_name[NAMEDATALEN];
    int rc = sprintf_s(stmt_name, NAMEDATALEN, "%s%d", DOLPHIN_PROTOCOL_STMT_NAME_PREFIX, req->statement_id);
    securec_check_ss(rc, "", "");
    BindMessage pqBindMessage;
    pqBindMessage.portalName = "";
    pqBindMessage.stmtName = stmt_name;
    pqBindMessage.numPFormats = 0;
    pqBindMessage.pformats = NULL;
    pqBindMessage.numParams = req->param_count;
    pqBindMessage.plength = NULL;
    pqBindMessage.pvalue = (const char**)req->parameter_values;
    pqBindMessage.numRFormats = 0;
    pqBindMessage.rformats = NULL;

    exec_bind_message(&pqBindMessage, NULL, NULL, false, dolphin_get_param_list_info);
    free_com_stmt_exec_request(req);
}

static void execute_binary_protocol_req_process_e()
{
    OgRecordAutoController _local_opt_e(SRT8_E);
    /* set use_parame to true, so in opfusion state it will send row desc, check OpFusion::setReceiver */
    u_sess->param_cxt.use_parame = true;
    if (exec_pre_execute_message("", FETCH_ALL, false)) {
        u_sess->param_cxt.use_parame = false;
        return;
    }

    exec_execute_message("", FETCH_ALL, false);
    u_sess->param_cxt.use_parame = false;
}

void remove_cached_stmt_data(uint32 *statement_id)
{
    if (GetSessionContext()->b_sendBlobHash) {
        HashEntryBlob *entry = (HashEntryBlob *)hash_search(GetSessionContext()->b_sendBlobHash, (void*)statement_id,
                                                            HASH_REMOVE, NULL);
        if (entry != NULL) {
            for (int i = 0; i < entry->value->count; i++) {
                pfree_ext(entry->value->data[i]);
            }
            pfree_ext(entry->value->data);
            pfree_ext(entry->value);
        }
    }
    if (GetSessionContext()->b_stmtInputTypeHash) {
        HashEntryStmtParamType *entry = (HashEntryStmtParamType *)hash_search(GetSessionContext()->b_stmtInputTypeHash,
            (void*)statement_id, HASH_REMOVE, NULL);
        if (entry != NULL) {
            pfree_ext(entry->value->itypes);
            pfree_ext(entry->value);
        }
    }
}

void execute_com_stmt_reset(StringInfo buf)
{
    uint32 statement_id;
    dq_get_int4(buf, &statement_id);
    remove_cached_stmt_data(&statement_id);
    send_general_ok_packet();
}

void execute_com_stmt_close(StringInfo buf)
{
    uint32 statement_id;
    char stmt_name[NAMEDATALEN];
    dq_get_int4(buf, &statement_id);
    int rc = sprintf_s(stmt_name, NAMEDATALEN, "%s%d", DOLPHIN_PROTOCOL_STMT_NAME_PREFIX, statement_id);
    securec_check_ss(rc, "\0", "\0");

    DropPreparedStatement(stmt_name, false);

    remove_cached_stmt_data(&statement_id);
}

void execute_com_field_list(char *tableName)
{
    int rc;
    
    start_xact_command();

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    StringInfo sql = makeStringInfo();
    appendStringInfo(sql, "show columns from %s", tableName);

    rc = SPI_execute(sql->data, false, 0);

    StringInfo buf = makeStringInfo();
    if (rc == SPI_OK_SELECT && SPI_processed > 0) {
        TupleDesc spi_tupdesc = SPI_tuptable->tupdesc;
        for (uint32 i = 0; i < SPI_processed; i++) {
            HeapTuple spi_tuple = SPI_tuptable->vals[i];
            char *name = SPI_getvalue(spi_tuple, spi_tupdesc, SPI_fnumber(spi_tupdesc, "Field"));
            char *default_value = SPI_getvalue(spi_tuple, spi_tupdesc, SPI_fnumber(spi_tupdesc, "Default"));
            dolphin_column_definition *field = make_dolphin_column_definition(name, tableName);
            field->default_value = default_value;
            send_column_definition41_packet(buf, field);

            pfree(field);
        }

        /* EOF packet at end of all rows*/
        if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
            send_network_eof_packet(buf);
        } else {
            send_new_eof_packet(buf);
        }
    }
    
    DestroyStringInfo(sql);
    DestroyStringInfo(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();

    finish_xact_command();
}
