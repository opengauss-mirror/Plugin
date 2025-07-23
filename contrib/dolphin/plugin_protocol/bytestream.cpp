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
#include "pgstat.h"

int dq_putmessage(const char *packet, size_t len)
{
    if (t_thrd.libpq_cxt.DoingCopyOut || t_thrd.libpq_cxt.PqCommBusy) {
        return 0;
    }
    t_thrd.libpq_cxt.PqCommBusy = true;

    // put header size
    char header[BYTE_4_LEN];
    uint32 num = len;
    int i = 0;
    for (; i < BYTE_3_LEN; i++) {
        uint8 ni = num & 0xff;

        header[i] = ni;
        num >>= SHIFT_BYTE_STEP;
    }
    header[i] = u_sess->proc_cxt.nextSeqid++;

    if (internal_putbytes(header, BYTE_4_LEN)) {
        goto fail;
    }

    if (internal_putbytes(packet, len)) {
        goto fail;
    }
    t_thrd.libpq_cxt.PqCommBusy = false;
    return 0;

fail:
    t_thrd.libpq_cxt.PqCommBusy = false;
    return EOF;
}

/* In contrast to pq_recvbuf, receive messages of a specific length and do not read more data */
static int dolphin_pq_getbytes(char* s, size_t len)
{
    /* Ensure that we're in blocking mode */
    pq_set_nonblocking(false);

    /* Can fill buffer from PqRecvLength and upwards */
    while (len > 0) {
        int r;

        WaitState oldStatus = pgstat_report_waitstatus(STATE_WAIT_COMM);
        r = secure_read(u_sess->proc_cxt.MyProcPort, s, len);
        (void)pgstat_report_waitstatus(oldStatus);

        if (r < 0) {
            if (errno == EINTR) {
                continue; /* Ok if interrupted */
            }

            /*
             * Careful: an ereport() that tries to write to the client would
             * cause recursion to here, leading to stack overflow and core
             * dump!  This message must go *only* to the postmaster log.
             */
            ereport(COMMERROR,
                (errcode_for_socket_access(), errmsg("could not receive data from client: %s", gs_comm_strerror())));
            return EOF;
        }
        if (r == 0) {
            /*
             * EOF detected.  We used to write a log message here, but it's
             * better to expect the ultimate caller to do that.
             */
            return EOF;
        }

        s += r;
        len -= r;
    }
    return 0;
}

int dq_special_getmessage(StringInfo buf)
{
    uint32 len;
    uint8 seq_id;

    resetStringInfo(buf);
    enlargeStringInfo(buf, PROTO_PAYLOAD_LEN);
    // Read PROTO_PAYLOAD_LEN bytes from recvbuf
    if (dolphin_pq_getbytes(buf->data, PROTO_PAYLOAD_LEN) == EOF) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION),
                            errmsg("unexpected EOF within PROTO_PAYLOAD_LEN word")));
        return EOF;
    }
    buf->len = PROTO_PAYLOAD_LEN;
    dq_get_payload_len(buf, &len);
    //Disconnect and report an error when the received data length exceeds the required upper limit
    if (PROTO_RECV_BUFFER_SIZE < len) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("Abnormal data reception")));
        return EOF;
    }

    // read one byte sequence id
    if (dolphin_pq_getbytes((char*)&seq_id, 1) == EOF || seq_id != 1) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("unexpected EOF within seq_id word")));
        return EOF;
    }
    u_sess->proc_cxt.nextSeqid = ++seq_id;
    resetStringInfo(buf);
    enlargeStringInfo(buf, len);
    if (len > 0) {
        // read playload_lenght bytes into buf->data
        if (dolphin_pq_getbytes(buf->data, len) == EOF) {
            ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("incomplete message from client")));
            return EOF;
        }

        buf->len = len;
        buf->cursor = 0;
        buf->data[len] = '\0';
    }

    return 0;
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
    u_sess->proc_cxt.nextSeqid = ++seq_id;

    if (unlikely(len == 0)) {
        return 0;
    }

    /* no enough room */
    if (len >= buf->maxlen) {
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
    }

    // read playload_lenght bytes into buf->data
    if (pq_getbytes(buf->data, len) == EOF) {
        ereport(COMMERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("incomplete message from client")));
        return EOF;
    }

    buf->len = len;
    buf->cursor = 0;
    buf->data[len] = '\0';

    return 0;
}
