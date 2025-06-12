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
 * bytestream.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/bytestream.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_BYTE_STREAM_H
#define _PROTO_BYTE_STREAM_H

#include "postgres.h"
#include "lib/stringinfo.h"
#include "libpq/libpq-be.h"

#define MAX_LEN_ENCODE_BYTE 251
#define MAX_LEN_ENCODE_2_BYTE 65536
#define MAX_LEN_ENCODE_3_BYTE 16777216
#define NULL_STRING_MARK 251

#define BYTE_1_LEN 1
#define BYTE_2_LEN 2
#define BYTE_3_LEN 3
#define BYTE_4_LEN 4
#define BYTE_8_LEN 8

#define PROTO_PACKET_HEADER_LEN 4
#define PROTO_PAYLOAD_LEN 3
#define PROTO_SEQUENCE_LEN 1 
#define PROTO_RECV_BUFFER_SIZE 8192

#define SHIFT_BYTE_STEP 8
#define MAX_LEFT_SHIFT_BIT 32
#define MAX_LEFT_SHIFT_INT 4 

extern int dq_putmessage(const char* packet, size_t len); 
extern int dq_getmessage(StringInfo s, uint32 maxlen); 
extern int dq_special_getmessage(StringInfo buf);

static inline void dq_append_int_len(StringInfo buf, uint64 num, int len)
{
    /* Make more room if needed */
    if (buf->len + len >= buf->maxlen) {
        enlargeStringInfo(buf, len);
    }

    for (int i = 0; i < len; i++) {
        uint8 ni = num & 0xff;

        buf->data[buf->len] = ni;
        buf->len++;
        num >>= 8;
    }
}

/* append a binary [u]int8 to a StringInfo buffer */
static inline void dq_append_int1(StringInfo buf, uint8 i)
{
    if (buf->len + 1 >= buf->maxlen) {
        enlargeStringInfo(buf, 1);
    }
    buf->data[buf->len] = i;
    buf->len++;
}

/* append a binary [u]int16 to a StringInfo buffer */
static inline void dq_append_int2(StringInfo buf, uint16 i)
{
    dq_append_int_len(buf, i, sizeof(uint16));
}

/* append a binary [u]int24 to a StringInfo buffer */
static inline void dq_append_int3(StringInfo buf, uint32 i)
{
    dq_append_int_len(buf, i, BYTE_3_LEN);
}

/* append a binary [u]int32 to a StringInfo buffer */
static inline void dq_append_int4(StringInfo buf, uint32 i)
{
    dq_append_int_len(buf, i, sizeof(uint32));
}

/* append a binary [u]int64 to a StringInfo buffer */
static inline void dq_append_int8(StringInfo buf, uint64 i)
{
    dq_append_int_len(buf, i, sizeof(uint64));
}

static inline int dq_append_int_lenenc(StringInfo buf, uint64 i)
{
    if (i < MAX_LEN_ENCODE_BYTE) {
        dq_append_int1(buf, i);
    } else if (i < MAX_LEN_ENCODE_2_BYTE) {
        dq_append_int1(buf, 0xfc);
        dq_append_int2(buf, i);
    } else if (i < MAX_LEN_ENCODE_3_BYTE) {
        dq_append_int1(buf, 0xfd);
        dq_append_int3(buf, i);
    } else {
        dq_append_int1(buf, 0xfe);
        dq_append_int8(buf, i);
    }

    return 0;
}

static inline void dq_append_string_null(StringInfo buf, const char *data)
{
    Assert(buf != NULL);

    /* Make more room if needed */
    int datalen = strlen(data);
    enlargeStringInfo(buf, datalen + 1);

    /* OK, append the data */
    errno_t rc = memcpy_s(buf->data + buf->len, (size_t)(buf->maxlen - buf->len), data, (size_t)datalen);
    securec_check(rc, "\0", "\0");
    buf->len += datalen;
    
    /* append terminal null */
    buf->data[buf->len] = '\0';
    buf->len++;
}

static inline void dq_append_string_len(StringInfo buf, const char *data, uint32 len)
{
    appendBinaryStringInfo(buf, data, len);
}

static inline void dq_append_string_lenenc(StringInfo buf, const char *data, int dataLen = -1)
{
    size_t len = dataLen != -1 ? dataLen : (data ? strlen(data) : 0);
    if (!data) {
        dq_append_int1(buf, NULL_STRING_MARK);
    } else if (len == 0) {
        dq_append_int1(buf, 0x00);
    } else {
        dq_append_int_lenenc(buf, len);
        dq_append_string_len(buf, data, len);
    }
}

static inline void dq_append_string_eof(StringInfo buf, const char *data)
{
    int len = data ? strlen(data) : 0;
    if (len > 0) {
        appendBinaryStringInfo(buf, data, len);
    }
}

static inline void dq_append_payload_len(StringInfo buf, uint32 len)
{
    dq_append_int3(buf, len);
}

static inline void dq_append_sequence_id(StringInfo buf, uint8 id)
{
    dq_append_int1(buf, id);
}

static inline void dq_get_int_len(StringInfo buf, void *num, int len)
{
    char *data = buf->data + buf->cursor;
    int i, shift;
    uint32 l = 0, h = 0;

    if (len < 0 || len > (buf->len - buf->cursor)) {
        ereport(ERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("insufficient data left in message")));
    }

    /**
     * for some reason left-shift > 32 leads to negative numbers 
     */

    for (i = 0, shift = 0; i < len && i < MAX_LEFT_SHIFT_INT; i++, shift += SHIFT_BYTE_STEP) {
        l |= ((unsigned char)data[i] << shift);
    }

    for (shift = 0; i < len; i++, shift += SHIFT_BYTE_STEP) {
        h |= ((unsigned char)data[i] << shift);
    }

    if (h) {
        *((uint64 *)num) = l | ((uint64)h << MAX_LEFT_SHIFT_BIT); 
    } else {
        *((uint32 *)num) = l;
    }
    
    buf->cursor += len;
}

static inline void dq_get_payload_len(StringInfo buf, uint32 *len)
{
    dq_get_int_len(buf, (void *)len, PROTO_PAYLOAD_LEN);
}

static inline void dq_get_int1(StringInfo buf, uint8 *num)
{
    *num = buf->data[buf->cursor++]; 
}

static inline void dq_get_int2(StringInfo buf, uint32 *num)
{
    dq_get_int_len(buf, (void *)num, BYTE_2_LEN);
    *num = *num & 0xffff;
}

static inline void dq_get_int3(StringInfo buf, uint32 *num)
{
    dq_get_int_len(buf, (void *)num, BYTE_3_LEN);
    *num = *num & 0xffffff;
}

static inline void dq_get_int4(StringInfo buf, uint32 *num)
{
    dq_get_int_len(buf, (void *)num, BYTE_4_LEN);
}

static inline void dq_get_int8(StringInfo buf, uint64 *num)
{
    dq_get_int_len(buf, (void *)num, BYTE_8_LEN);
}

static inline uint8 dq_get_int_lenenc(StringInfo buf, void *num)
{
    uint8 first;
    dq_get_int1(buf, &first);

    if (first < 0xfb) {
        *((uint32 *)num) = first;  
    } else if (first == 0xfc) {
        dq_get_int2(buf, (uint32 *)num);
    } else if (first == 0xfd) {
        dq_get_int3(buf, (uint32 *)num);
    } else if (first == 0xfe) {
        dq_get_int_len(buf, (uint64 *)num, BYTE_8_LEN);
    }

    return first;
}

static inline void dq_skip_bytes(StringInfo buf, int len)
{
    buf->cursor += len;
} 

static inline char* dq_get_string_len(StringInfo msg, int len)
{
    if (len < 0 || len > (msg->len - msg->cursor)) {
        ereport(ERROR, (errcode(ERRCODE_PROTOCOL_VIOLATION), errmsg("insufficient data left in message")));
    }
    
    if (len > 0) {
        char *buf = (char*) palloc(len + 1);
        int rcs = memcpy_s(buf, len, &msg->data[msg->cursor], len);
        securec_check(rcs, "\0", "\0");
        msg->cursor += len;
        buf[len] = 0x00;
        return buf;
    } else {
        return NULL;
    }
}

static inline char* dq_get_string_null(StringInfo msg)
{
    /*
     * It's safe to use strlen() here because a StringInfo is guaranteed to
     * have a trailing null byte.  But check we found a null inside the
     * message.
     */
    int slen = strlen(&msg->data[msg->cursor]);

    return dq_get_string_len(msg, slen + 1);
}

static inline char* dq_get_string_eof(StringInfo msg)
{
    return  dq_get_string_len(msg, msg->len - msg->cursor); 
}

static inline char* dq_get_string_lenenc(StringInfo msg)
{
    uint64 len;

    uint8 first = dq_get_int_lenenc(msg, (void *)&len);
    if (first != 0xfe) {
        len = len & 0xffffffff;
    }
    // stringInfo only alloc int-length size memroy here, 
    // mysql proto define the 8 byte int type, but not used so far.
    return dq_get_string_len(msg, len);
}

#endif /* BYTE_STREAM_H */