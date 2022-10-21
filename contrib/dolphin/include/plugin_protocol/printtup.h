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
 * printtup.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/printtup.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_PRINTTUP_H
#define _PROTO_PRINTTUP_H

#include "postgres.h"
#include "tcop/dest.h"

extern DestReceiver* dophin_printtup_create_DR(CommandDest dest);
extern void dolphin_set_DR_params(DestReceiver *self, List *target_list);

#endif  /* printtup.h */