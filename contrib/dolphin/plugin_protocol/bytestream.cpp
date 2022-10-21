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
 * bytestream.cpp
 *
 * IDENTIFICATION
 *    dolphin/plugin_protocol/bytestream.cpp
 * -------------------------------------------------------------------------
 */
#include "knl/knl_thread.h"
#include "libpq/libpq.h"

#include "libpq/pqcomm.h"
#include "mb/pg_wchar.h"

#include "plugin_protocol/bytestream.h"
#include "plugin_postgres.h"

THR_LOCAL uint8 next_seqid;

int dq_putmessage(const char *packet, size_t len)
{
    if (t_thrd.libpq_cxt.DoingCopyOut || t_thrd.libpq_cxt.PqCommBusy) {
        return 0;
    }
    t_thrd.libpq_cxt.PqCommBusy = true;

    // put header size
    StringInfo header = makeStringInfo();
    dq_append_payload_len(header, len);
    dq_append_sequence_id(header, next_seqid++); 

    if (internal_putbytes(header->data, header->len)) {
        goto fail;
    }

    if (internal_putbytes(packet, len)) {
        goto fail;
    }
    t_thrd.libpq_cxt.PqCommBusy = false;

    DestroyStringInfo(header);
    return 0;

fail:
    t_thrd.libpq_cxt.PqCommBusy = false;
    pfree(header);
    return EOF;
}

int dq_getmessage(StringInfo buf, uint32 maxlen)
{
    uint32 len;
    uint8 seq_id;

    resetStringInfo(buf);

    enlargeStringInfo(buf, PROTO_PAYLOAD_LEN);
    // Read PROTO_PAYLOAD_LEN bytes from recvbuf
    if (pq_getbytes(buf->data, PROTO_PAYLOAD_LEN) == EOF) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("unexpected EOF within PROTO_PAYLOAD_LEN word")));
        return EOF;
    }
    buf->len = PROTO_PAYLOAD_LEN;

    // convert payload length bytes stream to uint32
    dq_get_payload_len(buf, &len);

    // read one byte sequence id
    seq_id = pq_getbyte();
    if (seq_id == EOF) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("unexpected EOF within seq_id word")));
        return EOF; 
    }
    next_seqid = ++seq_id;

    // Read rest of payload length bytes into buf
    if (len > 0) {
        PG_TRY();
        {
            enlargeStringInfo(buf, len);
        }
        PG_CATCH();
        {
            if (pq_discardbytes(len) == EOF) {
                ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("incomplete message from client")));
            }
            PG_RE_THROW();
        }
        PG_END_TRY();

        // read playload_lenght bytes into buf->data
        if (pq_getbytes(buf->data, len) == EOF) {
            ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("incomplete message from client")));
            return EOF;
        }

        buf->len = len;
        buf->cursor = 0;
        buf->data[len] = '\0';
    }

    return 0;
}