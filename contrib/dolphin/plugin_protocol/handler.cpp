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

#include "plugin_postgres.h"
#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/handler.h"

static int execute_sql(const char* sql);
static int execute_prepare_sql(const char *sql);
static int execute_prepare_plan(com_stmt_exec_request *request);
static void execute_show_variables();
static int execute_show_columns_from(char *tableName);
static void execute_fetch_server_config();

SPIPlanPtr plan;

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
    uint8 command = pq_getmsgbyte(buf);

    switch (command) {
        case COM_PING: {
            send_general_ok_packet();
            break;
        }
        case COM_QUIT: {
            proc_exit(0);
            // Port *port = u_sess->proc_cxt.MyProcPort;
            // if (port->sock >= 0) {
            //     StreamClose(port->sock);
            // }
            // ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("response of COM_QUIT packet."))); 
            break;
        }
        case COM_INIT_DB: {
            char *schemaName = dq_get_string_eof(buf);
            StringInfo sql = makeStringInfo();
            appendStringInfo(sql, "use %s", schemaName);
            execute_sql(sql->data);
            break;
        }
        case COM_QUERY: {
            const char *sql = dq_get_string_eof(buf);
            /**
             * jdbc driver will send sql like this, dolphin does not support it yet.
             * SELECT  @@session.auto_increment_increment AS auto_increment_increment, @@character_set_client AS character_set_client, 
             *  @@character_set_connection AS character_set_connection, @@character_set_results AS character_set_results, 
             *  @@character_set_server AS character_set_server, @@collation_server AS collation_server, @@collation_connection AS collation_connection, 
             *  @@init_connect AS init_connect, @@interactive_timeout AS interactive_timeout, @@license AS license, @@lower_case_table_names AS lower_case_table_names, 
             *  @@max_allowed_packet AS max_allowed_packet, @@net_buffer_length AS net_buffer_length, @@net_write_timeout AS net_write_timeout, 
             *  @@query_cache_size AS query_cache_size, @@query_cache_type AS query_cache_type, @@sql_mode AS sql_mode, @@system_time_zone AS system_time_zone, 
             *  @@time_zone AS time_zone, @@transaction_isolation AS transaction_isolation, @@wait_timeout AS wait_timeout 
             */
            if (strcasestr(sql, "@@session.auto_increment_increment") && strcasestr(sql, "@@character_set_client")) {
                execute_fetch_server_config();
            } else if (strcasestr(sql, "show variables")) {
                execute_show_variables();
            } else if (strncasecmp(sql, "SET", 3) == 0) {
                send_general_ok_packet();
            } else if (strcmp(sql, "select @@version_comment limit 1") == 0) {
                execute_sql("select \'dolphin\'");
            } else {
                execute_sql(sql);
            }
            break;
        }
        // COM_FIELD_LIST response will trigger client-server [ FIN,ACK] error, just comment it here.
        // maybe desc & show columns sql result is the root cause.
        case COM_FIELD_LIST: {
            char *tableName = dq_get_string_null(buf);
            execute_show_columns_from(tableName);
            break;
        }
        case COM_STMT_PREPARE: {
            GetSessionContext()->enableBCmptMode = true;
            char *sql = dq_get_string_eof(buf);
            execute_prepare_sql(sql);
            break;
        }
        case COM_STMT_EXECUTE: {
            com_stmt_exec_request *req = (com_stmt_exec_request *)palloc0(sizeof(com_stmt_exec_request));
            dq_get_int4(buf, &req->statement_id);
            dq_get_int1(buf, &req->flags);
            dq_get_int4(buf, &req->iteration_count);

            // if parameter count > 0
            int parameter_count = 2;
            int len = (parameter_count + 7) / 8;
            req->null_bitmap = dq_get_string_len(buf, len);
            
            dq_get_int1(buf, &req->new_params_bind_flag);

            if (req->new_params_bind_flag) {
                com_stmt_param *parameters = (com_stmt_param *)palloc0(sizeof(com_stmt_param) * parameter_count);
                for (int i = 0; i < parameter_count; i++) {
                    dq_get_int2(buf, &parameters[i].type);
                }
                 for (int i = 0; i < parameter_count; i++) {
                     // should get value by type
                     parameters[i].value = dq_get_string_lenenc(buf);
                }
                req->parameter_values = parameters;
            }

            execute_prepare_plan(req);

            // execute cache plan
            
            break;
        }
        default:
            // COM_CREATE_DB, COM_DROP_DB have deprecated by mysql-server
           ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("dolphin server protocol not supported."))); 
    }
    return 0;
}

int execute_sql(const char *sql)
{
    int rc;
    
    start_xact_command();

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect(DestRemote)) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    rc = SPI_execute(sql, false, 0);

    StringInfo buf = makeStringInfo();
    if (rc == SPI_OK_SELECT) {
        /* EOF packet at end of all rows*/
        send_network_eof_packet(buf);
    } else {
        network_mysqld_ok_packet_t *ok_packet = make_ok_packet(SPI_processed);
        send_network_ok_packet(buf, ok_packet); 
        pfree(ok_packet);
    }

    DestroyStringInfo(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

    return 0;
}

int execute_prepare_sql(const char *sql)
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

    Oid argtypes[2];
    // SPIPlanPtr plan;

    argtypes[0] = VARCHAROID;
    argtypes[1] = VARCHAROID;

    plan = SPI_prepare(sql, 2, argtypes);
    // SPI_keepplan(plan);

    // todo send response packet
    StringInfo buf = makeStringInfo();
    send_com_stmt_prepare_ok_packet(buf, 1, 2, 2);

    for (int i = 0; i < 2; i++) {
        dolphin_data_field* param_field = make_dolphin_data_field("param");
        send_column_definition41_packet(buf, param_field);
    }
    send_network_eof_packet(buf);
    
    for (int i = 0; i < 2; i++) {
        dolphin_data_field* column_field = make_dolphin_data_field("column");
        send_column_definition41_packet(buf, column_field);
    }
    send_network_eof_packet(buf);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

    return 0;
}

int execute_prepare_plan(com_stmt_exec_request *request)
{
    int rc;
    
    start_xact_command();

    SPI_STACK_LOG("connect", NULL, NULL);
    if ((rc = SPI_connect(DestRemote)) != SPI_OK_CONNECT) {
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("dolphin SPI_connect failed: %s", SPI_result_code_string(rc)),
            errdetail("SPI_connect failed"),
            errcause("System error."),
            erraction("Check whether the snapshot retry is successful")));
    }

    Datum args[2];
    char nulls[2];
    int spirc;

    args[0] = PointerGetDatum(request->parameter_values[0].value);
    args[1] = PointerGetDatum(request->parameter_values[1].value); 
    nulls[0] = ' ';
    nulls[1] = ' ';

    spirc = SPI_execute_plan(plan, args, nulls, false, 1);

    SPI_STACK_LOG("finish", NULL, NULL);
    SPI_finish();
    finish_xact_command();

    return 0;
}


int execute_show_columns_from(char *tableName)
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
            dolphin_data_field *field = make_dolphin_data_field(name, tableName);
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

    return 0;
}

void execute_show_variables()
{
    StringInfo buf = makeStringInfo();
    send_field_count_packet(buf, 2);

    dolphin_data_field *name_field = make_dolphin_data_field("Variable_name");
    dolphin_data_field *value_field = make_dolphin_data_field("Value");
    send_column_definition41_packet(buf, name_field);
    send_column_definition41_packet(buf, value_field);
    send_network_eof_packet(buf);

    resetStringInfo(buf);
    dq_append_string_lenenc(buf, "max_allowed_packet");
    dq_append_string_lenenc(buf, "4194304");
    dq_putmessage(buf->data, buf->len);

    send_network_eof_packet(buf); 

    pfree(name_field);
    pfree(value_field);
    DestroyStringInfo(buf);
}

void execute_fetch_server_config()
{
    StringInfo buf = makeStringInfo();
    send_field_count_packet(buf, 21);

    // prepare data field
    dolphin_data_field *auto_increment_increment = make_dolphin_data_field("auto_increment_increment");
    dolphin_data_field *character_set_client = make_dolphin_data_field("character_set_client");
    dolphin_data_field *character_set_connection = make_dolphin_data_field("character_set_connection");
    dolphin_data_field *character_set_results = make_dolphin_data_field("character_set_results");
    dolphin_data_field *character_set_server = make_dolphin_data_field("character_set_server");
    dolphin_data_field *collation_server = make_dolphin_data_field("collation_server");
    dolphin_data_field *collation_connection = make_dolphin_data_field("collation_connection");
    dolphin_data_field *init_connect = make_dolphin_data_field("init_connect");
    dolphin_data_field *interactive_timeout = make_dolphin_data_field("interactive_timeout");
    dolphin_data_field *license = make_dolphin_data_field("license");
    dolphin_data_field *lower_case_table_names = make_dolphin_data_field("lower_case_table_names");
    dolphin_data_field *max_allowed_packet = make_dolphin_data_field("max_allowed_packet");
    dolphin_data_field *net_buffer_length = make_dolphin_data_field("net_buffer_length");
    dolphin_data_field *net_write_timeout = make_dolphin_data_field("net_write_timeout");
    dolphin_data_field *query_cache_size = make_dolphin_data_field("query_cache_size");
    dolphin_data_field *query_cache_type = make_dolphin_data_field("query_cache_type");
    dolphin_data_field *sql_mode = make_dolphin_data_field("sql_mode");
    dolphin_data_field *system_time_zone = make_dolphin_data_field("system_time_zone");
    dolphin_data_field *time_zone = make_dolphin_data_field("time_zone");
    dolphin_data_field *transaction_isolation = make_dolphin_data_field("transaction_isolation");
    dolphin_data_field *wait_timeout = make_dolphin_data_field("wait_timeout");
    
    // send column definition41 packet
    send_column_definition41_packet(buf, auto_increment_increment);
    send_column_definition41_packet(buf, character_set_client);
    send_column_definition41_packet(buf, character_set_connection);
    send_column_definition41_packet(buf, character_set_results);
    send_column_definition41_packet(buf, character_set_server);
    send_column_definition41_packet(buf, collation_server);
    send_column_definition41_packet(buf, collation_connection);
    send_column_definition41_packet(buf, init_connect);
    send_column_definition41_packet(buf, interactive_timeout);
    send_column_definition41_packet(buf, license);
    send_column_definition41_packet(buf, lower_case_table_names);
    send_column_definition41_packet(buf, max_allowed_packet);
    send_column_definition41_packet(buf, net_buffer_length);
    send_column_definition41_packet(buf, net_write_timeout);
    send_column_definition41_packet(buf, query_cache_size);
    send_column_definition41_packet(buf, query_cache_type);
    send_column_definition41_packet(buf, sql_mode);
    send_column_definition41_packet(buf, system_time_zone);
    send_column_definition41_packet(buf, time_zone);
    send_column_definition41_packet(buf, transaction_isolation);
    send_column_definition41_packet(buf, wait_timeout);
    send_network_eof_packet(buf);

    // send server config content 
    resetStringInfo(buf);
    dq_append_string_lenenc(buf, "1");
    dq_append_string_lenenc(buf, "utf8");
    dq_append_string_lenenc(buf, "utf8");
    dq_append_string_lenenc(buf, "utf8");
    dq_append_string_lenenc(buf, "utf8");
    dq_append_string_lenenc(buf, "utf8_general_ci");
    dq_append_string_lenenc(buf, "utf8_general_ci");
    dq_append_string_lenenc(buf, "");
    dq_append_string_lenenc(buf, "28800");
    dq_append_string_lenenc(buf, "Mulan PSL v2");
    dq_append_string_lenenc(buf, "2");
    dq_append_string_lenenc(buf, "4194304");
    dq_append_string_lenenc(buf, "16384");
    dq_append_string_lenenc(buf, "60");
    dq_append_string_lenenc(buf, "1048576");
    dq_append_string_lenenc(buf, "OFF");
    dq_append_string_lenenc(buf, "ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION");
    dq_append_string_lenenc(buf, "CST");
    dq_append_string_lenenc(buf, "SYSTEM");
    dq_append_string_lenenc(buf, "REPEATABLE-READ");
    dq_append_string_lenenc(buf, "28800");

    dq_putmessage(buf->data, buf->len);
    send_network_eof_packet(buf); 

    pfree(auto_increment_increment);
    pfree(character_set_client);
    pfree(character_set_connection);
    pfree(character_set_results);
    pfree(character_set_server);
    pfree(collation_server);
    pfree(collation_connection);
    pfree(init_connect);
    pfree(interactive_timeout);
    pfree(license);
    pfree(lower_case_table_names);
    pfree(max_allowed_packet);
    pfree(net_buffer_length);
    pfree(net_write_timeout);
    pfree(query_cache_size);
    pfree(query_cache_type);
    pfree(sql_mode);
    pfree(system_time_zone);
    pfree(time_zone);
    pfree(transaction_isolation);
    pfree(wait_timeout);
    DestroyStringInfo(buf);
}