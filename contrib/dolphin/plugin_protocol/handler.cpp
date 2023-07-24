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

static int execute_text_protocol_sql(const char* sql);
static int execute_com_stmt_prepare(const char *client_sql);
static int execute_binary_protocol_req(com_stmt_exec_request *request);
static void remove_cached_stmt_data(uint32 *statement_id);
static void execute_com_stmt_close(StringInfo buf);
static void execute_com_stmt_reset(StringInfo buf);
static void execute_com_field_list(char *tableName);

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
        strncasecmp(completionTag, "SHOW", SHOW_TAG_LEN) == 0) {
        // EOF packet
        send_network_eof_packet(buf);
    } else {
        int64 affected_rows = u_sess->statement_cxt.current_row_count;
        uint64 last_insert_id = u_sess->cmd_cxt.last_insert_id;
        network_mysqld_ok_packet_t *ok_packet = make_ok_packet(affected_rows, last_insert_id);
        send_network_ok_packet(buf, ok_packet); 
        pfree(ok_packet);
    }
    DestroyStringInfo(buf);
}

void dophin_send_message(ErrorData *edata)
{
    if (edata->elevel < ERROR) return;
    
    network_mysqld_err_packet_t *err_packet = (network_mysqld_err_packet_t *)palloc0(sizeof(network_mysqld_err_packet_t));
    err_packet->errcode = edata->sqlerrcode;
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
            const char *sql = dq_get_string_eof(buf);

            if (strcmp(sql, "select @@version_comment limit 1") == 0) {
                execute_text_protocol_sql("select \'dolphin\'");
            } else {
                execute_simple_query(sql);
            }
            break;
        }
        case COM_FIELD_LIST: {
            char *tableName = dq_get_string_null(buf);
            execute_com_field_list(tableName);
            break;
        }
        case COM_STMT_PREPARE: {
            char *sql = dq_get_string_eof(buf);
            execute_com_stmt_prepare(sql);
            break;
        }
        case COM_STMT_EXECUTE: {
            GetSessionContext()->is_binary_proto = true;
            com_stmt_exec_request *req = read_com_stmt_exec_request(buf);
            execute_binary_protocol_req(req);
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
        send_network_eof_packet(buf);
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

int execute_com_stmt_prepare(const char *client_sql)
{
    int rc;
    char stmt_name[NAMEDATALEN];
    
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

    StringInfo sql = makeStringInfo();
    int32 statement_id = gs_atomic_add_32(&g_proto_ctx.statement_id, 1);
    rc = snprintf_s(stmt_name, NAMEDATALEN + 1, NAMEDATALEN, "p%d", statement_id);
    securec_check_ss(rc, "\0", "\0");

    appendStringInfo(sql, "prepare %s from '%s'", stmt_name, client_sql);

    rc = SPI_execute(sql->data, false, 0);

    PreparedStatement *pstmt = FetchPreparedStatement(stmt_name, true, true);
    Query *query = (Query *)linitial(pstmt->plansource->query_list);
    int column_count = list_length(query->targetList);
    int param_count = pstmt->plansource->num_params;

    StringInfo buf = makeStringInfo();
    send_com_stmt_prepare_ok_packet(buf, statement_id, column_count, param_count);

    for (int i = 0; i < param_count; i++) {
        dolphin_column_definition* param_field = make_dolphin_column_definition("?");
        const TypeItem* item = GetItemByTypeOid(pstmt->plansource->param_types[i]);
        param_field->type = item->dolphin_type_id;
        param_field->charsetnr = item->charset_flag;
        send_column_definition41_packet(buf, param_field);
    }
    send_network_eof_packet(buf);
    
    for (int i = 0; i < column_count; i++) {
        dolphin_column_definition* column_field = make_dolphin_column_definition("");
        send_column_definition41_packet(buf, column_field);
    }
    send_network_eof_packet(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

    return 0;
}

int execute_binary_protocol_req(com_stmt_exec_request *request)
{
    StringInfo sql = makeStringInfo();
    appendStringInfo(sql, "execute p%d using ", request->statement_id);

    for (uint i = 0; i < request->param_count; i++) {
        switch (request->parameter_values[i].type) {
            case TYPE_STRING: {
                if (request->parameter_values[i].value.text) {
                    appendStringInfo(sql, "'%s'", request->parameter_values[i].value.text);
                } else {
                    appendStringInfo(sql, "(null)");
                }
                
                break;
            }
            case TYPE_INT1: {
                appendStringInfo(sql, "%d", request->parameter_values[i].value.i1);
                break;
            }
            case TYPE_INT2:
            case TYPE_INT4: {
                appendStringInfo(sql, "%d", request->parameter_values[i].value.i4);
                break;
            }
            case TYPE_INT8: {
                appendStringInfo(sql, "%lu", request->parameter_values[i].value.i8);
                break;
            }
            case TYPE_FLOAT: {
                appendStringInfo(sql, "%f", request->parameter_values[i].value.f.f4);
                break;
            }
            case TYPE_DOUBLE: {
                appendStringInfo(sql, "%lf", request->parameter_values[i].value.d.f8);
                break;
            }
            case TYPE_HEX: {
                appendStringInfo(sql, "x'");
                for (uint j = 0; j < strlen(request->parameter_values[i].value.text); j++) {
                    appendStringInfo(sql, "%x", request->parameter_values[i].value.text[j]);
                }
                appendStringInfo(sql, "'");
                break;
            }
            default:
                break;
        }
        
        if (i < request->param_count - 1) {
            appendStringInfoString(sql, ",");
        }
    }

    execute_simple_query(sql->data);

    return 0;
}

void remove_cached_stmt_data(uint32 *statement_id)
{
    if (GetSessionContext()->b_sendBlobHash) {
        (void)hash_search(GetSessionContext()->b_sendBlobHash, (void*)statement_id, HASH_REMOVE, NULL);
    }
    if (GetSessionContext()->b_stmtInputTypeHash) {
        (void)hash_search(GetSessionContext()->b_stmtInputTypeHash, (void*)statement_id, HASH_REMOVE, NULL);
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
    int rc = snprintf_s(stmt_name, NAMEDATALEN + 1, NAMEDATALEN, "p%d", statement_id);
    securec_check_ss(rc, "\0", "\0");

    StringInfo sql = makeStringInfo();
    appendStringInfo(sql, "DEALLOCATE %s", stmt_name);

    start_xact_command();

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect()) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    rc = SPI_execute(sql->data, false, 0);

    DestroyStringInfo(sql);
    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

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
        send_network_eof_packet(buf);
    }
    
    DestroyStringInfo(sql);
    DestroyStringInfo(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();

    finish_xact_command();
}
