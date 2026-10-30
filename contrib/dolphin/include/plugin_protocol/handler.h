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
 * handler.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/handler.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_HANDLER_H
#define _PROTO_HANDLER_H

#include "postgres.h"
#include "tcop/dest.h"

extern void proc_exit(int code);

extern void dophin_send_ready_for_query(CommandDest dest);
extern void dolphin_send_message(ErrorData *edata);
extern int dophin_read_command(StringInfo buf);
extern void dolphin_end_command(const char *completionTag);
extern int dolphin_process_command(StringInfo buf);
extern void dolphin_comm_reset(void);

#define MYSQL_ACCESS_DENIED_ERROR 1045
#define MYSQL_PARSE_ERROR 1064
#define MYSQL_NO_SUCH_TABLE_ERROR 1146
#define UNSUPPORTED_PS 1295

struct errcode_mapping {
    int kernel_errcode;
    uint16 dolphin_errcode;
    const char *sqlstate;
};

const struct errcode_mapping errcode_mappings[] = {
    {ERRCODE_INVALID_PASSWORD, MYSQL_ACCESS_DENIED_ERROR, "28000"},
    {ERRCODE_SYNTAX_ERROR, MYSQL_PARSE_ERROR, "42000"},
    {ERRCODE_UNDEFINED_TABLE, MYSQL_NO_SUCH_TABLE_ERROR, "42S02"},
    {ERRCODE_INVALID_PSTATEMENT_DEFINITION, UNSUPPORTED_PS, NULL}
};
#define ERRCODE_MAPPINGS_LEN (sizeof(errcode_mappings) / sizeof(struct errcode_mapping))

static inline const struct errcode_mapping *get_dolphin_error_mapping(int errcode)
{
    for (int i = 0; i < (int)ERRCODE_MAPPINGS_LEN; i++) {
        if (errcode_mappings[i].kernel_errcode == errcode) {
            return &errcode_mappings[i];
        }
    }
    return NULL;
}

static inline uint16 get_dolphin_errcode(int errcode)
{
    const struct errcode_mapping *mapping = get_dolphin_error_mapping(errcode);
    return mapping == NULL ? errcode : mapping->dolphin_errcode;
}

static inline const char *get_dolphin_sqlstate(int errcode)
{
    const struct errcode_mapping *mapping = get_dolphin_error_mapping(errcode);
    return mapping == NULL ? NULL : mapping->sqlstate;
}

#endif  /* handler.h */
