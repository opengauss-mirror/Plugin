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
#include "access/printtup.h"

extern DestReceiver* dophin_printtup_create_DR(CommandDest dest);
extern void dolphin_set_DR_params(DestReceiver *self, List *target_list);
extern void spi_sql_proc_dest_startup(DestReceiver* self, int operation, TupleDesc typeinfo);
extern void SetSqlProcSpiStmtParams(DestReceiver *self, SPIPlanPtr plan);
extern void spi_sql_proc_dest_printtup(TupleTableSlot *slot, DestReceiver *self);
extern DestReceiver* CreateSqlProcSpiDestReciver(CommandDest dest);

struct DR_Dolphin_proc_printtup {
    DestReceiver dest;
    StringInfoData buf;
    bool sendDescrip;
    Node* stmt;
    TupleDesc attrinfo; /* The attr info we are set up for */
    int nattrs;
    PrinttupAttrInfo* myinfo; /* Cached info about each attr */
};


#ifdef DOLPHIN
extern void printtupStream(TupleTableSlot* slot, DestReceiver* self);
extern void printBatch(VectorBatch* batch, DestReceiver* self);
#endif

#endif  /* printtup.h */