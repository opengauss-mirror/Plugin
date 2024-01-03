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
#include "catalog/pg_proc.h"
#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/proto_com.h"
#include "plugin_postgres.h"

#define DOLPHIN_BLOB_LENGTH 65535

static void printtup_startup(DestReceiver *self, int operation, TupleDesc typeinfo);
static void printtup_shutdown(DestReceiver *self);
static void printtup_destroy(DestReceiver *self);

static void SendRowDescriptionMessage(StringInfo buf, TupleDesc typeinfo, List* targetlist);
static void printtup_prepare_info(DR_printtup *myState, TupleDesc typeinfo, int numAttrs);

static void spi_sql_proc_dest_destroy(DestReceiver *self);
static void spi_sql_proc_dest_shutdown(DestReceiver *self);

static inline bool check_need_free_varchar_output(const char* str)
{
    return ((char*)str == u_sess->utils_cxt.varcharoutput_buffer);
}
static inline bool check_need_free_numeric_output(const char* str)
{
    return ((char*)str == u_sess->utils_cxt.numericoutput_buffer);
}
static inline bool check_need_free_date_output(const char* str)
{
    return ((char*)str == u_sess->utils_cxt.dateoutput_buffer);
}
DestReceiver* CreateSqlProcSpiDestReciver(CommandDest dest)
{
    DestReceiver* resdr = NULL;
    Port* MyProcPort = u_sess->proc_cxt.MyProcPort;
    if (MyProcPort && MyProcPort->protocol_config->fn_printtup_create_DR &&
        MyProcPort->protocol_config->fn_printtup_create_DR == dophin_printtup_create_DR) {
        DR_printtup *dr = (DR_printtup *)palloc0(sizeof(DR_printtup));
        dr->pub.receiveSlot = printtup; /* might get changed later */
        dr->pub.rStartup = printtup_startup;
        dr->pub.rShutdown = printtup_shutdown;
        dr->pub.rDestroy = printtup_destroy;
        dr->pub.mydest = DestRemote;
        dr->pub.finalizeLocalStream = NULL;
        dr->pub.tmpContext = NULL;
        dr->sendDescrip = (dest == DestSqlProcSPI);
        dr->attrinfo = NULL;
        dr->nattrs = 0;
        dr->myinfo = NULL;
        dr->formats = NULL;
        resdr =(DestReceiver*)dr;
    } else {
        DR_Dolphin_proc_printtup *dr = (DR_Dolphin_proc_printtup *)palloc0(sizeof(DR_Dolphin_proc_printtup));
        dr->dest.receiveSlot = spi_sql_proc_dest_printtup; /* might get changed later */
        dr->dest.rStartup = spi_sql_proc_dest_startup;
        dr->dest.rShutdown = spi_sql_proc_dest_shutdown;
        dr->dest.rDestroy = spi_sql_proc_dest_destroy;
        dr->dest.mydest = dest;
        dr->dest.finalizeLocalStream = NULL;
        dr->dest.tmpContext = NULL;
        dr->sendDescrip = (dest == DestSqlProcSPI);
        dr->stmt = NULL;
        dr->attrinfo = NULL;
        dr->nattrs = 0;
        dr->myinfo = NULL;
        resdr =(DestReceiver*)dr;
    }
    return (DestReceiver*)resdr;
}

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
            tle = (TargetEntry *)lfirst(tlist_item);
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

static void send_textproto(TupleTableSlot *slot, DR_printtup *myState, int natts, StringInfo buf)
{
     /*
     * send the attributes of this tuple
     */
    for (int i = 0; i < natts; ++i) {
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        Datum origattr = slot->tts_values[i];
        Datum attr = static_cast<uintptr_t>(0);
        char *outputstr = NULL;

        if (slot->tts_isnull[i] || slot->tts_tupleDescriptor->attrs[i].attisdropped) {
            dq_append_string_lenenc(buf, "");
            continue;
        }

        /*
         * If we have a toasted datum, forcibly detoast it here to avoid
         * memory leakage inside the type's output routine.
         */
        attr = thisState->typisvarlena ? PointerGetDatum(PG_DETOAST_DATUM(origattr)) : origattr;

        Oid typeOid = slot->tts_tupleDescriptor->attrs[i].atttypid;
        if (typeOid == BINARYOID || typeOid == VARBINARYOID ||
            typeOid == TINYBLOBOID || typeOid == MEDIUMBLOBOID ||
            typeOid == LONGBLOBOID || typeOid == BLOBOID ||
            typeOid == RAWOID || typeOid == BYTEAOID) {
            bytea* barg = DatumGetByteaPP(attr);
            dq_append_string_lenenc(buf, VARDATA_ANY(barg), VARSIZE_ANY_EXHDR(barg));
        } else {
            outputstr = OutputFunctionCall(&thisState->finfo, attr);
            dq_append_string_lenenc(buf, outputstr);

            pfree(outputstr);
        }

        /* Clean up detoasted copy, if any */
        if (DatumGetPointer(attr) != DatumGetPointer(origattr)) {
            pfree(DatumGetPointer(attr));
        }
    }
}

static void send_binaryproto(TupleTableSlot *slot, int natts, StringInfo buf)
{
    TupleDesc desc = slot->tts_tupleDescriptor;

    // [0x00] packet header
    dq_append_int1(buf, 0x00);

    // NULL bitmap, length= (column_count + 7 + 2) / 8
    int len = (natts + 7 + 2) / 8;
    bits8 null_bitmap[len] = {0x00};
    for (int j = 0; j < natts; j++) {
        if (slot->tts_isnull[j] || desc->attrs[j].attisdropped) {
            int byte_pos = (j + 2) / 8;
            int bit_pos = (j + 2) % 8;
            null_bitmap[byte_pos] |= 1 << bit_pos;
        }
    }

    for (int k = 0; k < len; k++) {
        dq_append_int1(buf, null_bitmap[k]);
    }

    // values for non-null columns
    for (int i = 0; i < natts; i++) {
        if (slot->tts_isnull[i] || desc->attrs[i].attisdropped) {
            continue;
        }
        Datum binval = slot->tts_values[i];
        const TypeItem *item = GetItemByTypeOid(desc->attrs[i].atttypid);
        append_data_by_dolphin_type(item, binval, buf);
    }
}

void printtup(TupleTableSlot *slot, DestReceiver *self)
{
    TupleDesc typeinfo = slot->tts_tupleDescriptor;
    DR_printtup *myState = (DR_printtup *)self;
    StringInfo buf = &myState->buf;
    int natts = typeinfo->natts;

    /* Set or update my derived attribute info, if needed */
    if (myState->attrinfo != typeinfo || myState->nattrs != natts) {
        printtup_prepare_info(myState, typeinfo, natts);
    }

    /* Make sure the tuple is fully deconstructed */
    tableam_tslot_getallattrs(slot);

    MemoryContext old_context = changeToTmpContext(self);

    resetStringInfo(buf);

    if (!GetSessionContext()->is_binary_proto) {
        send_textproto(slot, myState, natts, buf);
    } else {
        send_binaryproto(slot, natts, buf);
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

void SetSqlProcSpiStmtParams(DestReceiver *self, SPIPlanPtr plan)
{
    if (self->mydest == DestSqlProcSPI && plan) {
        DR_Dolphin_proc_printtup *dr = (DR_Dolphin_proc_printtup *)self;
        List* stmts = plan->stmt_list;
        if (stmts) {
            ListCell* lc = NULL;
            foreach (lc, stmts) {
                Node* stmt = (Node*)lfirst(lc);
                if (IsA(stmt, PlannedStmt)) {
                    if (((PlannedStmt*)stmt)->canSetTag) {
                        dr->stmt = stmt;
                        break;
                    }
                } else if (IsA(stmt, Query)) {
                    if (((Query*)stmt)->canSetTag) {
                        dr->stmt = stmt;
                        break;
                    }
                } else if (list_length(stmts) == 1) {
                    /* Utility stmts are assumed canSetTag if they're the only stmt */
                    dr->stmt = stmt;
                    break;
                }
            }
        } else {
            dr->stmt = NULL;
        }
    }
}

void spi_sql_proc_dest_startup(DestReceiver* self, int operation, TupleDesc typeinfo)
{
    DR_Dolphin_proc_printtup *myState = (DR_Dolphin_proc_printtup *)self;
    Node* stmt = myState->stmt;

    /* create buffer to be used for all messages */
    initStringInfo(&myState->buf);

    if (PG_PROTOCOL_MAJOR(FrontendProtocol) < 3) {
        /*
         * Send portal name to frontend (obsolete cruft, gone in proto 3.0)
         *
         * If portal name not specified, use "blank" portal.
         */
        const char *portalName = "blank";

        pq_puttextmessage('P', portalName);
    }

    /*
     * If we are supposed to emit row descriptions, then send the tuple
     * descriptor of the tuples.
     */
    if (myState->sendDescrip)
        SendRowDescriptionMessage(&myState->buf, typeinfo, FetchStatementTargetList(stmt), NULL);
}

/* ----------------
 *		spi_sql_proc_dest_printtup --- print a tuple in protocol 3.0,when call procedure
 * ----------------
 */

static void spi_proc_printtup_prepare_info(DR_Dolphin_proc_printtup *myState, TupleDesc typeinfo, int numAttrs)
{
    int16 *formats = NULL;
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


void spi_sql_proc_dest_printtup(TupleTableSlot *slot, DestReceiver *self)
{
    TupleDesc typeinfo = slot->tts_tupleDescriptor;
    DR_Dolphin_proc_printtup *myState = (DR_Dolphin_proc_printtup *)self;
    StringInfo buf = &myState->buf;
    int natts = typeinfo->natts;
    int i;
    bool needFree = false;
    bool binary = false;
    /* just as we define in backend/commands/analyze.cpp */
#define WIDTH_THRESHOLD 1024

    /* Set or update my derived attribute info, if needed */
    if (myState->attrinfo != typeinfo || myState->nattrs != natts)
        spi_proc_printtup_prepare_info(myState, typeinfo, natts);

#ifdef PGXC

    /*
     * The datanodes would have sent all attributes in TEXT form. But
     * if the client has asked for any attribute to be sent in a binary format,
     * then we must decode the datarow and send every attribute in the format
     * that the client has asked for. Otherwise its ok to just forward the
     * datarow as it is
     */
    for (i = 0; i < natts; ++i) {
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        if (thisState->format != 0)
            binary = true;
    }

    /*
     * If we are having DataRow-based tuple we do not have to encode attribute
     * values, just send over the DataRow message as we received it from the
     * Datanode
     */
    if (slot->tts_dataRow != NULL && (pg_get_client_encoding() == GetDatabaseEncoding()) && !binary) {
        pq_beginmessage_reuse(buf, 'D');
        appendBinaryStringInfo(buf, slot->tts_dataRow, slot->tts_dataLen);
        pq_endmessage_reuse(buf);
        StreamTimeSerilizeEnd(t_thrd.pgxc_cxt.GlobalNetInstr);
        return;
    }
#endif

    /* Make sure the tuple is fully deconstructed */
    tableam_tslot_getallattrs(slot);

    MemoryContext old_context = changeToTmpContext(self);
    /*
     * Prepare a DataRow message
     */
    pq_beginmessage_reuse(buf, 'D');

    pq_sendint16(buf, natts);

    /*
     * send the attributes of this tuple
     */
    for (i = 0; i < natts; ++i) {
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        Datum attr = slot->tts_values[i];

        /*
         * skip null value attribute,
         * we need to skip the droped columns for analyze global stats.
         */
        if (slot->tts_isnull[i] || typeinfo->attrs[i].attisdropped) {
            pq_sendint32(buf, (uint32)-1);
            continue;
        }

        if (typeinfo->attrs[i].atttypid == ANYARRAYOID && slot->tts_dataRow != NULL) {
            /*
             * For ANYARRAY type, the not null DataRow-based tuple indicates the value in
             * attr had been converted to CSTRING type previously by using anyarray_out.
             * just send over the DataRow message as we received it.
             */
            pq_sendcountedtext_printtup(buf, (char *)attr, strlen((char *)attr), thisState->encoding, (void*)&thisState->convert_finfo);
        } else {
            if (thisState->format == 0) {
                /* Text output */
                char *outputstr = NULL;
#ifndef ENABLE_MULTIPLE_NODES
                t_thrd.xact_cxt.callPrint = true;
#endif
                needFree = false;
                switch (thisState->typoutput) {
                    case F_INT4OUT:
                        outputstr = output_int32_to_cstring(DatumGetInt32(attr));
                        break;
                    case F_INT8OUT:
                        outputstr = output_int64_to_cstring(DatumGetInt64(attr));
                        break;
                    case F_BPCHAROUT:
                        /* support dolphin customizing bpcharout */
                        outputstr = OutputFunctionCall(&thisState->finfo, attr);
                        needFree = true;
                        break;
                    case F_VARCHAROUT:
                        outputstr = output_text_to_cstring((text*)DatumGetPointer(attr));
                        needFree = !check_need_free_varchar_output(outputstr);
                        break;
                    case F_NUMERIC_OUT:
                        outputstr = output_numeric_out(DatumGetNumeric(attr));
                        needFree = !check_need_free_numeric_output(outputstr);
                        break;
                    case F_DATE_OUT:
                        /* support dolphin customizing dateout */
                        outputstr = OutputFunctionCall(&thisState->finfo, attr);
                        needFree = true;
                        break;
                    default:
                        outputstr = OutputFunctionCall(&thisState->finfo, attr);
                        needFree = true;
                        break;
                }
#ifndef ENABLE_MULTIPLE_NODES
                t_thrd.xact_cxt.callPrint = false;
#endif
                pq_sendcountedtext_printtup(buf, outputstr, strlen(outputstr), thisState->encoding, (void*)&thisState->convert_finfo);
                if (needFree) {
                    pfree(outputstr);
                }
            } else {
                /* Binary output */
                bytea *outputbytes = NULL;

                outputbytes = SendFunctionCall(&thisState->finfo, attr);
                pq_sendint32(buf, VARSIZE(outputbytes) - VARHDRSZ);
                pq_sendbytes(buf, VARDATA(outputbytes), VARSIZE(outputbytes) - VARHDRSZ);
                pfree(outputbytes);
            }
        }
    }

    (void)MemoryContextSwitchTo(old_context);

    pq_endmessage_reuse(buf);
}

/* ----------------
 *		spi_sql_proc_dest_destroy
 * ----------------
 */
static void spi_sql_proc_dest_shutdown(DestReceiver *self)
{
    DR_Dolphin_proc_printtup *myState = (DR_Dolphin_proc_printtup *)self;

    if (myState->myinfo != NULL)
        pfree(myState->myinfo);
    myState->myinfo = NULL;

    myState->attrinfo = NULL;
}

/* ----------------
 *		spi_sql_proc_dest_destroy
 * ----------------
 */
static void spi_sql_proc_dest_destroy(DestReceiver *self)
{
    pfree(self);
}
