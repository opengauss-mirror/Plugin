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
 * printtup.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/printtup.cpp
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include "access/printtup.h"
#include "access/transam.h"
#include "access/tableam.h"
#include "libpq/libpq.h"
#include "libpq/pqformat.h"
#include "tcop/pquery.h"
#include "utils/lsyscache.h"
#include "mb/pg_wchar.h"
#include "miscadmin.h"
#include "access/heapam.h"

#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/proto_com.h"

#define DOLPHIN_BLOB_LENGTH 65535

static void printtup_startup(DestReceiver *self, int operation, TupleDesc typeinfo);
static void printtup_shutdown(DestReceiver *self);
static void printtup_destroy(DestReceiver *self);

static void SendRowDescriptionMessage(StringInfo buf, TupleDesc typeinfo, List* targetlist);
static void printtup_prepare_info(DR_printtup *myState, TupleDesc typeinfo, int numAttrs);

DestReceiver* dophin_printtup_create_DR(CommandDest dest)
{
    DR_printtup *self = (DR_printtup *)palloc0(sizeof(DR_printtup));

    self->pub.receiveSlot = printtup;
    self->pub.sendBatch = printBatch;
    self->pub.rStartup = printtup_startup;
    self->pub.rShutdown = printtup_shutdown;
    self->pub.rDestroy = printtup_destroy;
    self->pub.finalizeLocalStream = NULL;
    self->pub.mydest = dest;
    self->pub.tmpContext = NULL;

    /*
     * Send T message automatically if DestRemote, but not if
     * DestRemoteExecute
     */
    self->sendDescrip = (dest == DestRemote);

    self->attrinfo = NULL;
    self->nattrs = 0;
    self->myinfo = NULL;
    self->formats = NULL;

    return (DestReceiver *)self;
}

/*
 * Set parameters for a DestRemote (or DestRemoteExecute) receiver
 */
void dolphin_set_DR_params(DestReceiver *self, List* target_list)
{
    DR_printtup *myState = (DR_printtup *)self;

    if (myState->pub.mydest == DestRemote) {
        myState->target_list = target_list;
    }
}

static void printtup_startup(DestReceiver *self, int operation, TupleDesc typeinfo)
{
    DR_printtup *myState = (DR_printtup *)self;

    /* create buffer to be used for all messages */
    initStringInfo(&myState->buf);

    /*
     * If we are supposed to emit row descriptions, then send the tuple
     * descriptor of the tuples.
     */
    if (myState->sendDescrip) {
        SendRowDescriptionMessage(&myState->buf, typeinfo, myState->target_list);
    }
}

static void SendRowDescriptionMessage(StringInfo buf, TupleDesc typeinfo, List *targetlist)
{
    FormData_pg_attribute *attrs = typeinfo->attrs;
    int natts = typeinfo->natts;
    int i;
    ListCell *tlist_item = list_head(targetlist);
    TargetEntry *tle;

    // FIELD_COUNT packet
    send_field_count_packet(buf, natts);
    
    if (tlist_item != NULL) {
        tle = (TargetEntry *)lfirst(tlist_item);
    }

    for (i = 0; i < natts; ++i) {
        // FIELD packet
        dolphin_column_definition *field = make_dolphin_column_definition(&attrs[i]);

        while (tlist_item && ((TargetEntry *)lfirst(tlist_item))->resjunk) {
            tlist_item = lnext(tlist_item);
        }
    
        if (tlist_item != NULL) {
            TargetEntry *tle = (TargetEntry *)lfirst(tlist_item);
            tlist_item = lnext(tlist_item);
        } 
        
        send_column_definition41_packet(buf, field);
        pfree(field);
    }
    
    // EOF packet
    send_network_eof_packet(buf);
}

inline MemoryContext changeToTmpContext(DestReceiver *self)
{
    MemoryContext old_context = CurrentMemoryContext;
    if (self->tmpContext != NULL) {
        old_context = MemoryContextSwitchTo(self->tmpContext);
    }
    return old_context;
}

void printtup(TupleTableSlot *slot, DestReceiver *self)
{

    TupleDesc typeinfo = slot->tts_tupleDescriptor;
    DR_printtup *myState = (DR_printtup *)self;
    StringInfo buf = &myState->buf;
    int natts = typeinfo->natts;
    int i;

    /* Set or update my derived attribute info, if needed */
    if (myState->attrinfo != typeinfo || myState->nattrs != natts) {
        printtup_prepare_info(myState, typeinfo, natts);
    }

    /* Make sure the tuple is fully deconstructed */
    tableam_tslot_getallattrs(slot);

    MemoryContext old_context = changeToTmpContext(self);

    resetStringInfo(buf);

    /*
     * send the attributes of this tuple
     */
    for (i = 0; i < natts; ++i) {
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        Datum origattr = slot->tts_values[i];
        Datum attr = static_cast<uintptr_t>(0);
        char *outputstr = NULL;

        if (slot->tts_isnull[i]) {
            dq_append_string_lenenc(buf, "");
            continue;
        }

        /*
         * If we have a toasted datum, forcibly detoast it here to avoid
         * memory leakage inside the type's output routine.
         */
        attr = thisState->typisvarlena ? PointerGetDatum(PG_DETOAST_DATUM(origattr)) : origattr;

        outputstr = OutputFunctionCall(&thisState->finfo, attr);
        dq_append_string_lenenc(buf, outputstr);

        pfree(outputstr);

        /* Clean up detoasted copy, if any */
        if (DatumGetPointer(attr) != DatumGetPointer(origattr)) {
            pfree(DatumGetPointer(attr));
        }
    }

    (void)MemoryContextSwitchTo(old_context);

    dq_putmessage(buf->data, buf->len);
}

/*
 * Get the lookup info that printtup() needs
 */
static void printtup_prepare_info(DR_printtup *myState, TupleDesc typeinfo, int numAttrs)
{

    int16 *formats = myState->portal != NULL ? myState->portal->formats : myState->formats;
    int i;

    /* get rid of any old data */
    if (myState->myinfo != NULL) {
        pfree(myState->myinfo);
    }
    myState->myinfo = NULL;

    myState->attrinfo = typeinfo;
    myState->nattrs = numAttrs;
    if (numAttrs <= 0) {
        return;
    }


    if (myState->portal != NULL && myState->portal->tupDesc != NULL) {
#ifdef USE_ASSERT_CHECKING
        Assert(numAttrs <= myState->portal->tupDesc->natts);
#else
        if (numAttrs > myState->portal->tupDesc->natts) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("num attrs from DN is %d, mismatch num attrs %d in portal", numAttrs, myState->portal->tupDesc->natts)));
        }
#endif
    }

    myState->myinfo = (PrinttupAttrInfo *)palloc0(numAttrs * sizeof(PrinttupAttrInfo));

    for (i = 0; i < numAttrs; i++) {
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        int16 format = (formats ? formats[i] : 0);

        /*
         * for analyze global stats, because DN will send sample rows to CN,
         * if we encounter droped columns, we should send it to CN. but atttypid of dropped column
         * is invalid in pg_attribute, it will generate error, so we should do special process for the reason.
         */
        if (typeinfo->attrs[i].attisdropped) {
            typeinfo->attrs[i].atttypid = UNKNOWNOID;
        }

        thisState->format = format;
        if (format == 0) {
            getTypeOutputInfo(typeinfo->attrs[i].atttypid, &thisState->typoutput, &thisState->typisvarlena);
            fmgr_info(thisState->typoutput, &thisState->finfo);
        } else if (format == 1) {
            getTypeBinaryOutputInfo(typeinfo->attrs[i].atttypid, &thisState->typsend, &thisState->typisvarlena);
            fmgr_info(thisState->typsend, &thisState->finfo);
        } else {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("unsupported format code: %d", format)));
        }
    }
}

void printBatch(VectorBatch *batch, DestReceiver *self){
}

/* ----------------
 *		printtup_shutdown
 * ----------------
 */static void printtup_shutdown(DestReceiver *self)
{
    DR_printtup *myState = (DR_printtup *)self;

    if (myState->myinfo != NULL)
        pfree(myState->myinfo);
    myState->myinfo = NULL;

    myState->attrinfo = NULL;
}

/* ----------------
 *		printtup_destroy
 * ----------------
 */
static void printtup_destroy(DestReceiver *self)
{
    pfree(self);
}
