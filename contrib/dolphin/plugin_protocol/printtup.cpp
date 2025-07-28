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

#include <cstring>

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
#include "access/datavec/vector.h"
#include "plugin_utils/float.h"
#include "plugin_protocol/dqformat.h"
#include "plugin_protocol/printtup.h"
#include "plugin_protocol/proto_com.h"
#include "plugin_postgres.h"
#ifdef DOLPHIN
#include "plugin_commands/mysqlmode.h"
#endif

#define DOLPHIN_BLOB_LENGTH 65535
#define BITS_PER_BYTE 8

static void printtup_startup(DestReceiver *self, int operation, TupleDesc typeinfo);
static void printtup_shutdown(DestReceiver *self);
static void printtup_destroy(DestReceiver *self);

static void DolphinSendRowDescriptionMessage(StringInfo buf, TupleDesc typeinfo, List *targetlist, int16 *formats);
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
    self->pub.sendRowDesc = DolphinSendRowDescriptionMessage;
    self->pub.finalizeLocalStream = NULL;
    self->pub.mydest = dest;
    self->pub.tmpContext = NULL;

    /*
     * Send T message automatically if DestRemote or DestRemoteExecute
     */
    self->sendDescrip = (dest == DestRemote || dest == DestRemoteExecute);

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
        List *target_list = myState->portal != NULL ? FetchPortalTargetList(myState->portal) : myState->target_list;
        myState->pub.sendRowDesc(&myState->buf, typeinfo, target_list, NULL);
    }
}

static void DolphinSendRowDescriptionMessage(StringInfo buf, TupleDesc typeinfo, List *targetlist, int16 *formats)
{
    FormData_pg_attribute *attrs = typeinfo->attrs;
    int natts = typeinfo->natts;
    int i;
    ListCell *tlist_item = list_head(targetlist);
    TargetEntry *tle = NULL;

    // FIELD_COUNT packet
    send_field_count_packet(buf, natts);
    
    if (tlist_item != NULL) {
        tle = (TargetEntry *)lfirst(tlist_item);
    }

    for (i = 0; i < natts; ++i) {
        while (tlist_item && ((TargetEntry *)lfirst(tlist_item))->resjunk) {
            tlist_item = lnext(tlist_item);
        }
    
        if (tlist_item != NULL) {
            tle = (TargetEntry *)lfirst(tlist_item);
            tlist_item = lnext(tlist_item);
        }

        char* oriColName = NULL;
        if (tle != NULL && strcmp(tle->resname, attrs[i].attname.data) == 0 &&
            OidIsValid(tle->resorigtbl) && tle->resorigcol > 0) {
            HeapTuple tuple;
            Form_pg_attribute attForm;
            tuple = SearchSysCache2(ATTNUM, ObjectIdGetDatum(tle->resorigtbl), Int16GetDatum(tle->resorigcol));
            if (HeapTupleIsValid(tuple)) {
                attForm = (Form_pg_attribute)GETSTRUCT(tuple);
                oriColName = pstrdup(attForm->attname.data);
                ReleaseSysCache(tuple);
            }
        }
        
        // FIELD packet
        dolphin_column_definition field;
        make_dolphin_column_definition(&attrs[i], NULL, oriColName, &field);
        send_column_definition41_packet(buf, &field);
        pfree_ext(oriColName);
    }
    
    // EOF packet
    if (!(GetSessionContext()->Conn_Mysql_Info->client_capabilities & CLIENT_DEPRECATE_EOF)) {
        send_network_eof_packet(buf);
    }
}

inline MemoryContext changeToTmpContext(DestReceiver *self)
{
    MemoryContext old_context = CurrentMemoryContext;
    if (self->tmpContext != NULL) {
        old_context = MemoryContextSwitchTo(self->tmpContext);
    }
    return old_context;
}

static void convertBitsToBytes(char* bitStr, StringInfo buf)
{
    int bitlen = strlen(bitStr);
    int bytelen = (bitlen + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
    uint8 bytes[bytelen];
    errno_t rc = memset_sp(bytes, bytelen, 0, bytelen);
    securec_check(rc, "\0", "\0");
    
    int bitindex = 0;
    int byteindex = bytelen - 1;
    for (int i = bitlen - 1; i >= 0; --i) {
        if (bitStr[i] == '1') {
            bytes[byteindex] |= (1 << bitindex);
        }
        bitindex++;
        if (byteindex > 0 && bitindex == BITS_PER_BYTE) {
            bitindex = 0;
            byteindex--;
        }
    }
    dq_append_int_lenenc(buf, bytelen);
    for (int i = 0; i < bytelen; ++i) {
        dq_append_int1(buf, bytes[i]);
    }
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
            dq_append_string_lenenc(buf, NULL);
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
            if (typeOid == BITOID) {
                convertBitsToBytes(outputstr, buf);
            } else {
                dq_append_string_lenenc(buf, outputstr);
            }

            pfree(outputstr);
        }

        /* Clean up detoasted copy, if any */
        if (DatumGetPointer(attr) != DatumGetPointer(origattr)) {
            pfree(DatumGetPointer(attr));
        }
    }
}

static void send_binaryproto(TupleTableSlot *slot, DR_printtup *myState, int natts, StringInfo buf)
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
        PrinttupAttrInfo *thisState = myState->myinfo + i;
        Datum binval = slot->tts_values[i];
        const TypeItem *item = GetItemByTypeOid(desc->attrs[i].atttypid);
        append_data_by_dolphin_type(item, binval, buf, thisState);
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
        send_binaryproto(slot, myState, natts, buf);
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
            if (typeinfo->attrs[i].attcollation == BINARY_COLLATION_OID && ENABLE_MULTI_CHARSET) {
                thisState->encoding = GetDatabaseEncoding();
             } else {
                thisState->encoding = get_valid_charset_by_collation(typeinfo->attrs[i].attcollation);
            }
            construct_conversion_fmgr_info(
                thisState->encoding, u_sess->mb_cxt.ClientEncoding->encoding, (void*)&thisState->convert_finfo);
        } else if (format == 1) {
            getTypeBinaryOutputInfo(typeinfo->attrs[i].atttypid, &thisState->typsend, &thisState->typisvarlena);
            fmgr_info(thisState->typsend, &thisState->finfo);
            thisState->encoding = PG_INVALID_ENCODING; // just initialize, should not be used
            thisState->convert_finfo.fn_oid = InvalidOid;
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
        if (thisState->format != 0) {
            binary = true;
            break;
        }
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
                    case F_INT4OUT: {
                        outputstr = u_sess->utils_cxt.int4output_buffer;
                        int length = 0;
                        pg_ltoa(DatumGetInt32(attr), outputstr, &length);
#ifndef ENABLE_MULTIPLE_NODES
                        t_thrd.xact_cxt.callPrint = false;
#endif
                        pq_sendcountedtext_printtup(buf, outputstr, length, thisState->encoding,
                                                    (void *)&thisState->convert_finfo);
                        continue;
                    }
                    case F_INT8OUT: {
                        outputstr = u_sess->utils_cxt.int8output_buffer;
                        int length = 0;
                        pg_lltoa(DatumGetInt64(attr), outputstr, &length);
#ifndef ENABLE_MULTIPLE_NODES
                        t_thrd.xact_cxt.callPrint = false;
#endif
                        pq_sendcountedtext_printtup(buf, outputstr, length, thisState->encoding,
                                                    (void *)&thisState->convert_finfo);
                        continue;
                    }
                    case F_FLOAT4OUT: {
                        outputstr = u_sess->utils_cxt.float4output_buffer;
                        pg_ftoa<MAXFLOATWIDTH>(DatumGetFloat4(attr), outputstr);
                        pq_sendcountedtext_printtup(buf, outputstr, std::strlen(outputstr), thisState->encoding,
                                                    (void*)&thisState->convert_finfo);
                        continue;
                    }
                    case F_FLOAT8OUT: {
                        outputstr = u_sess->utils_cxt.float8output_buffer;
                        dolphin_dtoa<MAXDOUBLEWIDTH>(DatumGetFloat8(attr), outputstr);
                        pq_sendcountedtext_printtup(buf, outputstr, std::strlen(outputstr), thisState->encoding,
                                                    (void *)&thisState->convert_finfo);
                        continue;
                    }
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

inline void AddCheckInfo(StringInfo buf)
{
    StringInfoData buf_check;
    bool is_check_added = false;

    /* add check info  for datanode and coordinator */
    if (IS_SPQ_EXECUTOR || IsConnFromCoord()) {
#ifdef USE_ASSERT_CHECKING
        initStringInfo(&buf_check);
        AddCheckMessage(&buf_check, buf, false);
        is_check_added = true;
#else
        if (anls_opt_is_on(ANLS_STREAM_DATA_CHECK)) {
            initStringInfo(&buf_check);
            AddCheckMessage(&buf_check, buf, false);
            is_check_added = true;
        }
#endif

        if (unlikely(is_check_added)) {
            pfree(buf->data);
            buf->len = buf_check.len;
            buf->maxlen = buf_check.maxlen;
            buf->data = buf_check.data;
        }
    }
}

#ifdef DOLPHIN
bool is_req_from_gsql()
{
    return strcmp(u_sess->attr.attr_common.application_name, "gsql") == 0;
}

bool inline is_type_support_not_escape_zero(Oid type)
{
    return BINARYOID == type;
}


void dolphin_pq_sendcountedtext_binary_printtup(StringInfo buf, const char* str, int slen, int src_encoding, void* convert_finfo)
{
    char* p = (char*)str;
    char* p2 = NULL;

    if (unlikely(src_encoding != u_sess->mb_cxt.ClientEncoding->encoding)) {
        int check_len = strlen(str);
        p = pg_any_to_client(str, check_len, src_encoding, convert_finfo);
        if (unlikely(p != str)) {
            p2 = (char*)palloc(slen + 1);
            errno_t rc = memset_sp(p2, slen + 1, 0, slen + 1);
            securec_check(rc, "\0", "\0");
            rc = memcpy_sp(p2, (size_t)slen, p, (size_t)strlen(p));
            securec_check(rc, "\0", "\0");
            pfree(p);
            p = p2;
        }
    }
    if (unlikely(p != str)) { /* actual conversion has been done? */
        enlargeStringInfo(buf, slen + sizeof(uint32));
        pq_writeint32(buf, (uint32)slen);
        errno_t rc = memcpy_sp(buf->data + buf->len, (size_t)(buf->maxlen - buf->len), p, (size_t)slen);
        securec_check(rc, "\0", "\0");
        buf->len += slen;
        buf->data[buf->len] = '\0';
        pfree(p);
        p = NULL;
    } else {
        enlargeStringInfo(buf, slen + sizeof(uint32));
        pq_writeint32(buf, (uint32)slen);
        errno_t rc = memcpy_sp(buf->data + buf->len, (size_t)(buf->maxlen - buf->len), str, (size_t)slen);
        securec_check(rc, "\0", "\0");
        buf->len += slen;
        buf->data[buf->len] = '\0';
    }
}


void dolphin_default_printtup(TupleTableSlot *slot, DestReceiver *self)
{
    TupleDesc typeinfo = slot->tts_tupleDescriptor;
    DR_printtup *myState = (DR_printtup *)self;
    StringInfo buf = &myState->buf;
    int natts = typeinfo->natts;
    int i;
    bool need_free = false;
    bool binary = false;
    /* just as we define in backend/commands/analyze.cpp */
#define WIDTH_THRESHOLD 1024

    /* Set or update my derived attribute info, if needed */
    if (myState->attrinfo != typeinfo || myState->nattrs != natts)
        printtup_prepare_info(myState, typeinfo, natts);

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
        if (thisState->format != 0) {
            binary = true;
            break;
        }
    }

    /*
     * If we are having DataRow-based tuple we do not have to encode attribute
     * values, just send over the DataRow message as we received it from the
     * Datanode
     */
    if (slot->tts_dataRow != NULL && (pg_get_client_encoding() == GetDatabaseEncoding()) && !binary) {
        pq_beginmessage_reuse(buf, 'D');
        appendBinaryStringInfo(buf, slot->tts_dataRow, slot->tts_dataLen);
        AddCheckInfo(buf);
        pq_endmessage_reuse(buf);
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
                char *zero_without_escape_output = NULL;
                int actual_len = 0;
#ifndef ENABLE_MULTIPLE_NODES
                t_thrd.xact_cxt.callPrint = true;
#endif
                need_free = false;
                switch (thisState->typoutput) {
                    case F_INT4OUT: {
                        outputstr = u_sess->utils_cxt.int4output_buffer;
                        int length = 0;
                        pg_ltoa(DatumGetInt32(attr), outputstr, &length);
#ifndef ENABLE_MULTIPLE_NODES
                        t_thrd.xact_cxt.callPrint = false;
#endif
                        pq_sendcountedtext_printtup(buf, outputstr, length, thisState->encoding,
                                                    (void *)&thisState->convert_finfo);
                        continue;
                    }
                    case F_INT8OUT: {
                        outputstr = u_sess->utils_cxt.int8output_buffer;
                        int length = 0;
                        pg_lltoa(DatumGetInt64(attr), outputstr, &length);
#ifndef ENABLE_MULTIPLE_NODES
                        t_thrd.xact_cxt.callPrint = false;
#endif
                        pq_sendcountedtext_printtup(buf, outputstr, length, thisState->encoding,
                                                    (void *)&thisState->convert_finfo);
                        continue;
                    }
                    case F_BPCHAROUT: 
                        /* support dolphin customizing bpcharout */
                        if (u_sess->attr.attr_sql.dolphin) {
                            outputstr = OutputFunctionCall(&thisState->finfo, attr);
                            need_free = true;
                            break;
                        }
                    case F_VARCHAROUT: 
                        outputstr = output_text_to_cstring((text*)DatumGetPointer(attr));
                        need_free = !check_need_free_varchar_output(outputstr);
                        break;
                    case F_NUMERIC_OUT: 
                        outputstr = output_numeric_out(DatumGetNumeric(attr));
                        need_free = !check_need_free_numeric_output(outputstr);
                        break;
                    case F_DATE_OUT:
                        /* support dolphin customizing dateout */
                        if (u_sess->attr.attr_sql.dolphin) {
                            outputstr = OutputFunctionCall(&thisState->finfo, attr);
                            need_free = true;
                        } else {
                            outputstr = output_date_out(DatumGetDateADT(attr));
                            need_free = !check_need_free_date_output(outputstr);
                        }
                        break;
                    case F_VECTOR_OUT:
                        outputstr = u_sess->utils_cxt.vectoroutput_buffer;
                        PrintOutVector(outputstr, attr);
                        break;
                    case F_TIMESTAMP_OUT: {
                        outputstr = u_sess->utils_cxt.timestamp_output_buffer;
                        Timestamp ts = DatumGetTimestamp(attr);

                        timestamp_out(ts, outputstr);
                        break;
                    }
                    default:
                        outputstr = OutputFunctionCall(&thisState->finfo, attr);
                        if (typeinfo->attrs[i].atttypid == YEAROID && strcmp(outputstr, "0") == 0) {
                            pfree(outputstr);
                            outputstr = pstrdup("0000");
                        } 
                        need_free = false;
                        break;
                }
#ifndef ENABLE_MULTIPLE_NODES
                t_thrd.xact_cxt.callPrint = false;
#endif
                if (is_type_support_not_escape_zero(typeinfo->attrs[i].atttypid) &&
                    SQL_MODE_NOT_ESCAPE_ZERO_IN_BINARY() && is_req_from_jdbc()) {
                    int escape_len = strlen(outputstr);
                    zero_without_escape_output = (char*)palloc(escape_len + 1);
                    for (int i = 0; i < escape_len; i++) {
                        if ((i + 3 < escape_len) && outputstr[i] == '\\' && outputstr[i + 1] == '0' &&
                            outputstr[i + 2] == '0' &&  outputstr[i + 3] == '0') {
                            zero_without_escape_output[actual_len++] = 0;
                            i = i + 3;
                        } else {
                            zero_without_escape_output[actual_len++] = outputstr[i];
                        }
                    }
                    zero_without_escape_output[actual_len] = '\0';
                    dolphin_pq_sendcountedtext_binary_printtup(buf, zero_without_escape_output, actual_len, thisState->encoding, (void*)&thisState->convert_finfo);
                    pfree(zero_without_escape_output);
                } else {
                    pq_sendcountedtext_printtup(buf, outputstr, strlen(outputstr), thisState->encoding, (void*)&thisState->convert_finfo);
                }
                if (need_free) {
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

    AddCheckInfo(buf);
    pq_endmessage_reuse(buf);
}


static void dolphin_default_printtup_startup(DestReceiver *self, int operation, TupleDesc typeinfo)
{
    DR_printtup *myState = (DR_printtup *)self;
    Portal portal = myState->portal;

    /* create buffer to be used for all messages */
    initStringInfo(&myState->buf);

    if (PG_PROTOCOL_MAJOR(FrontendProtocol) < 3) {
        /*
         * Send portal name to frontend (obsolete cruft, gone in proto 3.0)
         *
         * If portal name not specified, use "blank" portal.
         */
        const char *portalName = portal->name;

        if (portalName == NULL || portalName[0] == '\0')
            portalName = "blank";

        pq_puttextmessage('P', portalName);
    }

    /*
     * If we are supposed to emit row descriptions, then send the tuple
     * descriptor of the tuples.
     */
    if (myState->sendDescrip)
        SendRowDescriptionMessage(&myState->buf, typeinfo, FetchPortalTargetList(portal), portal->formats);

    /* ----------------
     * We could set up the derived attr info at this time, but we postpone it
     * until the first call of printtup, for 2 reasons:
     * 1. We don't waste time (compared to the old way) if there are no
     *	  tuples at all to output.
     * 2. Checking in printtup allows us to handle the case that the tuples
     *	  change type midway through (although this probably can't happen in
     *	  the current executor).
     * ----------------
     */
}


static void dolphin_default_printtup_shutdown(DestReceiver *self)
{
    DR_printtup *myState = (DR_printtup *)self;

    if (myState->myinfo != NULL)
        pfree(myState->myinfo);
    myState->myinfo = NULL;

    myState->attrinfo = NULL;
}


static void dolphin_default_printtup_destroy(DestReceiver *self)
{
    pfree(self);
}


DestReceiver* dophin_default_printtup_create_DR(CommandDest dest)
{
    DR_printtup *self = (DR_printtup *)palloc0(sizeof(DR_printtup));

    if (StreamTopConsumerAmI())
        self->pub.receiveSlot = printtupStream;
    else
        self->pub.receiveSlot = dolphin_default_printtup; /* might get changed later */

    self->pub.sendBatch = printBatch;
    self->pub.rStartup = dolphin_default_printtup_startup;
    self->pub.rShutdown = dolphin_default_printtup_shutdown;
    self->pub.rDestroy = dolphin_default_printtup_destroy;
    self->pub.sendRowDesc = SendRowDescriptionMessage;
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

#endif

