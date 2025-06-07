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

#define UNSUPPORTED_PS 1295

struct errcode_mapping {
    int kernel_errcode;
    uint16 dolphin_errcode;
};

const struct errcode_mapping errcode_mappings[] = {
    {ERRCODE_INVALID_PSTATEMENT_DEFINITION, UNSUPPORTED_PS}
};
#define ERRCODE_MAPPINGS_LEN (sizeof(errcode_mappings) / sizeof(struct errcode_mapping))

static inline uint16 get_dolphin_errcode(int errcode)
{
    for (int i = 0; i < (int)ERRCODE_MAPPINGS_LEN; i++) {
        if (errcode_mappings[i].kernel_errcode == errcode) {
            return errcode_mappings[i].dolphin_errcode;
        }
    }
    return errcode;
}

#endif  /* handler.h */