/* -------------------------------------------------------------------------
 *
 * uuid.c
 *	  Functions for the built-in type "uuid".
 *
 * Copyright (c) 2007-2012, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/uuid.c
 *
 * -------------------------------------------------------------------------
 */
#include <cstdio>

#include "postgres.h"
#include "pgxc/pgxcnode.h"
#include "knl/knl_variable.h"

#include "access/hash.h"
#include "access/xlog.h"
#include "executor/executor.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"
#include "utils/uuid.h"
#include "utils/timestamp.h"
#include "utils/inet.h"
#include <net/if.h>
#include "plugin_postgres.h"

static void string_to_uuid(const char* source, pg_uuid_t* uuid);
static int uuid_internal_cmp(const pg_uuid_t* arg1, const pg_uuid_t* arg2);

PG_FUNCTION_INFO_V1_PUBLIC(uuid_generate);
extern "C" DLL_PUBLIC Datum uuid_generate(PG_FUNCTION_ARGS);

Datum uuid_in(PG_FUNCTION_ARGS)
{
    char* uuid_str = PG_GETARG_CSTRING(0);
    pg_uuid_t* uuid = NULL;

    uuid = (pg_uuid_t*)palloc(sizeof(*uuid));
    string_to_uuid(uuid_str, uuid);
    PG_RETURN_UUID_P(uuid);
}

#define HEX_CHARS "0123456789abcdef"

Datum uuid_out(PG_FUNCTION_ARGS)
{
    pg_uuid_t* uuid = PG_GETARG_UUID_P(0);
    StringInfoData buf;
    int i;

    initStringInfo(&buf);
    for (i = 0; i < UUID_LEN; i++) {
        int hi;
        int lo;

        /*
         * We print uuid values as a string of 8, 4, 4, 4, and then 12
         * hexadecimal characters, with each group is separated by a hyphen
         * ("-"). Therefore, add the hyphens at the appropriate places here.
         */
        if (i == 4 || i == 6 || i == 8 || i == 10)
            appendStringInfoChar(&buf, '-');

        hi = uuid->data[i] >> 4;
        lo = uuid->data[i] & 0x0F;

        appendStringInfoChar(&buf, HEX_CHARS[hi]);
        appendStringInfoChar(&buf, HEX_CHARS[lo]);
    }

    PG_RETURN_CSTRING(buf.data);
}

/*
 * We allow UUIDs as a series of 32 hexadecimal digits with an optional dash
 * after each group of 4 hexadecimal digits, and optionally surrounded by {}.
 * (The canonical format 8x-4x-4x-4x-12x, where "nx" means n hexadecimal
 * digits, is the only one used for output.)
 */
static void string_to_uuid(const char* source, pg_uuid_t* uuid)
{
    const char* src = source;
    bool braces = false;
    int i;
    errno_t rc = EOK;

    if (src[0] == '{') {
        src++;
        braces = true;
    }

    for (i = 0; i < UUID_LEN; i++) {
        char str_buf[3];

        if (src[0] == '\0' || src[1] == '\0')
            goto syntax_error;

        rc = memcpy_s(str_buf, sizeof(str_buf), src, 2);
        securec_check(rc, "\0", "\0");

        if (!isxdigit((unsigned char)str_buf[0]) || !isxdigit((unsigned char)str_buf[1]))
            goto syntax_error;

        str_buf[2] = '\0';
        uuid->data[i] = (unsigned char)strtoul(str_buf, NULL, 16);
        src += 2;
        if (src[0] == '-' && (i % 2) == 1 && i < UUID_LEN - 1)
            src++;
    }

    if (braces) {
        if (*src != '}')
            goto syntax_error;
        src++;
    }

    if (*src != '\0')
        goto syntax_error;

    return;

syntax_error:
    ereport(
        ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), errmsg("invalid input syntax for uuid: \"%s\"", source)));
}

Datum uuid_recv(PG_FUNCTION_ARGS)
{
    StringInfo buffer = (StringInfo)PG_GETARG_POINTER(0);
    pg_uuid_t* uuid = NULL;

    uuid = (pg_uuid_t*)palloc(UUID_LEN);
    errno_t rc = memcpy_s(uuid->data, UUID_LEN, pq_getmsgbytes(buffer, UUID_LEN), UUID_LEN);
    securec_check(rc, "\0", "\0");
    PG_RETURN_POINTER(uuid);
}

Datum uuid_send(PG_FUNCTION_ARGS)
{
    pg_uuid_t* uuid = PG_GETARG_UUID_P(0);
    StringInfoData buffer;

    pq_begintypsend(&buffer);
    pq_sendbytes(&buffer, (char*)uuid->data, UUID_LEN);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

/* internal uuid compare function */
static int uuid_internal_cmp(const pg_uuid_t* arg1, const pg_uuid_t* arg2)
{
    return memcmp(arg1->data, arg2->data, UUID_LEN);
}

Datum uuid_lt(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) < 0);
}

Datum uuid_le(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) <= 0);
}

Datum uuid_eq(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) == 0);
}

Datum uuid_ge(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) >= 0);
}

Datum uuid_gt(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) > 0);
}

Datum uuid_ne(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_BOOL(uuid_internal_cmp(arg1, arg2) != 0);
}

/* handler for btree index operator */
Datum uuid_cmp(PG_FUNCTION_ARGS)
{
    pg_uuid_t* arg1 = PG_GETARG_UUID_P(0);
    pg_uuid_t* arg2 = PG_GETARG_UUID_P(1);

    PG_RETURN_INT32(uuid_internal_cmp(arg1, arg2));
}

/* hash index support */
Datum uuid_hash(PG_FUNCTION_ARGS)
{
    pg_uuid_t* key = PG_GETARG_UUID_P(0);

    return hash_any(key->data, UUID_LEN);
}

static int hash_ctoa(char ch)
{
    int res = 0;
    if (ch >= 'a' && ch <= 'f') {
        res = 10 + (ch - 'a');
    } else {
        res = ch - '0';
    }
    return res;
}

Datum hash16in(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);
    int len = strlen(str);
    uint64 res = 0;
    if (len > 16) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for hash16: \"%s\"", str)));
    }

    for (int i = 0; i < len; ++i) {
        if (str[0] == '-') {
            res = 0;
            break;
        }
        if (!((str[i] >= 'a' && str[i] <= 'f') || (str[i] >= '0' && str[i] <= '9'))) {
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                    errmsg("invalid input syntax for hash16: \"%s\"", str)));
        }
        res = res * 16 + hash_ctoa(str[i]);
    }

    PG_RETURN_TRANSACTIONID(res);
}

Datum hash16out(PG_FUNCTION_ARGS)
{
    uint64 hash16 = PG_GETARG_TRANSACTIONID(0);
    StringInfoData buf;
    StringInfoData res;

    initStringInfo(&buf);
    initStringInfo(&res);

    if (hash16 >= 0) {
        for (int i = 0; i < 16; ++i) {
            int ho = hash16 & 0x0F;
            hash16 = hash16 >> 4;
            appendStringInfoChar(&buf, HEX_CHARS[ho]);
        }
        for (int j = strlen(buf.data) - 1; j >= 0; --j) {
            appendStringInfoChar(&res, buf.data[j]);
        }
    }
    PG_RETURN_CSTRING(res.data);
}

static void string_to_hash32(const char* source, hash32_t* hash32)
{
    const char* src = source;
    int len = strlen(src);
    if (len != HASH32_LEN * 2) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for hash32: \"%s\"", source)));
    }

    for (int i = 0; i < HASH32_LEN; ++i) {
        hash32->data[i] = hash_ctoa(src[i * 2]) * 16 + hash_ctoa(src[i * 2 + 1]);
    }
}

Datum hash32in(PG_FUNCTION_ARGS)
{
    char *hash32_str = PG_GETARG_CSTRING(0);
    hash32_t *hash32 = NULL;

    hash32 = (hash32_t*)palloc(sizeof(hash32_t));
    string_to_hash32(hash32_str, hash32);
    PG_RETURN_HASH32_P(hash32);
}

Datum hash32out(PG_FUNCTION_ARGS)
{
    hash32_t* hash32 = PG_GETARG_HASH32_P(0);
    StringInfoData buf;
    int i;

    initStringInfo(&buf);
    for (i = 0; i < HASH32_LEN; i++) {
        int h_low;
        int h_high;

        h_low = hash32->data[i] & 0x0F;
        h_high = (hash32->data[i] >> 4) & 0x0F;

        appendStringInfoChar(&buf, HEX_CHARS[h_high]);
        appendStringInfoChar(&buf, HEX_CHARS[h_low]);
    }

    PG_RETURN_CSTRING(buf.data);
}

Datum hash16_add(PG_FUNCTION_ARGS)
{
    uint64 arg1 = DatumGetUInt64(PG_GETARG_DATUM(0));
    uint64 arg2 = DatumGetUInt64(PG_GETARG_DATUM(1));

    PG_RETURN_DATUM(UInt64GetDatum(arg1 + arg2));
}

Datum hash16_eq(PG_FUNCTION_ARGS)
{
    uint64 arg1 = DatumGetUInt64(PG_GETARG_DATUM(0));
    uint64 arg2 = DatumGetUInt64(PG_GETARG_DATUM(1));

    PG_RETURN_BOOL(arg1 == arg2);
}


#define UUID_VERSION 0x1000
#define FORMATTED_UUID_LEN 36
#define UUID_FIRST_PART_LEN 8
#define UUID_MID_PART_LEN 4
#define UUID_LAST_PART_LEN 12
#define UUID_TIME_OFFSET ((uint64)141427 * 24 * 60 * 60 * 1000 * 1000 * 10)
#define INT_RANGE_REVISE_PARAM 128
#define HEX_BASE 16
#define URANDOM_FILE_PATH "/dev/urandom"
#define MAC_CHAR_NUM 6
#define CLOCK_SEQ_CHAR_NUM 2
#define MaxMacAddrList 10

static uint64 uuid_time = 0;
static uint nano_seq = 0;
static char g_clockSeqAndMac[]="0000-000000000000";
static pthread_mutex_t gUuidMutex = PTHREAD_MUTEX_INITIALIZER;

/*
* transfer char* into hexadecimal style.
* offset is used to modify the range of source char in case of getting range of signed int8. set to 0 if not needed.
*/
static void string_to_hex(char* source, char* dest, int src_len, int offset)
{
    int dest_index = 0;
    for (int i = 0; i < src_len; i++) {
        dest[dest_index++] = HEX_CHARS[(source[i] + offset) / HEX_BASE];
        dest[dest_index++] = HEX_CHARS[(source[i] + offset) % HEX_BASE];
    }
}

static void int_to_hex(uint64 source, char* dest, int src_len)
{
    char* dest_ptr = dest + src_len;
    for (int i = 0; i < src_len; i++) {
        *(--dest_ptr) = HEX_CHARS[source & 0xF];
        source >>= 4;
    }
}

/*
 * get specific num of character read from /dev/urandom, storing in rand_buf.
 */
static void pseudo_rand_read(char* rand_buf, int bytes_read)
{
    FILE* f = fopen(URANDOM_FILE_PATH, "r");
    if (!f) {
        ereport(ERROR, (errcode(ERRCODE_FILE_READ_FAILED), (errmsg("cannot open urandom file"))));
    }
    size_t b_read;
    char* buf_ptr = rand_buf;
    while (bytes_read) {
        b_read = fread(buf_ptr, 1, bytes_read, f);
        if (b_read <= 0) {
            fclose(f);
            ereport(ERROR, (errcode(ERRCODE_FILE_READ_FAILED), (errmsg("failed to get random number"))));
        }
        buf_ptr += b_read;
        bytes_read -= (int)b_read;
    }
    fclose(f);
}

static uint64 GetMACAddr(void)
{
    macaddr mac;
    uint64 macAddr;
    int sockFd = NO_SOCKET;
    struct ifconf ifconfInfo;
    struct ifreq ifreqInfo;
    char *buf = NULL;
    errno_t ss_rc = EOK;
    uint32 i;

    ss_rc = memset_s((void *)&mac, sizeof(macaddr), 0, sizeof(macaddr));
    securec_check(ss_rc, "\0", "\0");

    sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sockFd != NO_SOCKET) {
        buf = (char *)palloc(MaxMacAddrList * sizeof(ifreq));
        ifconfInfo.ifc_len = MaxMacAddrList * sizeof(ifreq);
        ifconfInfo.ifc_buf = buf;

        if (ioctl(sockFd, SIOCGIFCONF, &ifconfInfo) != -1) {
            struct ifreq *ifrepTmp = ifconfInfo.ifc_req;
            for (i = 0; i < (ifconfInfo.ifc_len / sizeof(struct ifreq)); i++) {
                ss_rc = strcpy_s(ifreqInfo.ifr_name, strlen(ifrepTmp->ifr_name) + 1, ifrepTmp->ifr_name);
                securec_check(ss_rc, "\0", "\0");

                if (ioctl(sockFd, SIOCGIFFLAGS, &ifreqInfo) == 0) {
                    if (!(ifreqInfo.ifr_flags & IFF_LOOPBACK)) {
                        if (ioctl(sockFd, SIOCGIFHWADDR, &ifreqInfo) == 0) {
                            mac.a = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[0];
                            mac.b = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[1];
                            mac.c = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[2];
                            mac.d = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[3];
                            mac.e = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[4];
                            mac.f = (unsigned char)ifreqInfo.ifr_hwaddr.sa_data[5];
                            break;
                        }
                    }
                }
                ifrepTmp++;
            }
        }
        pfree_ext(buf);
        close(sockFd);
    }
    macAddr = ((uint64)mac.a << 40) | ((uint64)mac.b << 32) | ((uint64)mac.c << 24) | ((uint64)mac.d << 16) |
            ((uint64)mac.e << 8) | (uint64)mac.f;
    return macAddr;
}

/*
 * generate uuid in v1 style.
 */
Datum uuid_generate(PG_FUNCTION_ARGS)
{
    char clockSeq[CLOCK_SEQ_CHAR_NUM];
    /* two extra bytes, one for '-', one for '\0' */
    char clockSeqAndMac[UUID_MID_PART_LEN + UUID_LAST_PART_LEN + 2];
    AutoMutexLock localeLock(&gUuidMutex);
    localeLock.lock();

    // first time in this function. get mac address and clock seq first
    if (unlikely(!uuid_time)) {
        pseudo_rand_read(clockSeq, CLOCK_SEQ_CHAR_NUM);
        string_to_hex(clockSeq, g_clockSeqAndMac, CLOCK_SEQ_CHAR_NUM, INT_RANGE_REVISE_PARAM);

        uint64 mac_addr = GetMACAddr();
        // no mac obtained. init or use random string as virtual mac
        if (!mac_addr) {
            char virtual_mac_string[6] = {0};
            if (strlen(virtual_mac_string) == 0) {
                pseudo_rand_read(virtual_mac_string, MAC_CHAR_NUM);
            }
            mac_addr = ((uint64)virtual_mac_string[0] << 40) | ((uint64)virtual_mac_string[1] << 32) |
                    ((uint64)virtual_mac_string[2] << 24) | ((uint64)virtual_mac_string[3] << 16) |
                    ((uint64)virtual_mac_string[4] << 8) | (uint64)virtual_mac_string[5];
        }
        int_to_hex(mac_addr, g_clockSeqAndMac + UUID_MID_PART_LEN + 1, UUID_LAST_PART_LEN);
    }

    uint64 curr_time;
    /*
     * this is why nano_seq should be uint but not uint64, cause nano_seq will increase, if it is uint64
     * add increase to UINT64_MAX, the final result of curr_time will be overflow.
     */
#ifdef HAVE_INT64_TIMESTAMP
    curr_time = GetCurrentTimestamp() * 10 + UUID_TIME_OFFSET + nano_seq;
#else
    curr_time = GetCurrentTimestamp() * 1000 * 1000 * 10 + UUID_TIME_OFFSET + nano_seq;
#endif
    // give back nano_seq we borrowed before
    if (curr_time > uuid_time && nano_seq > 0) {
        uint64 give_back = Min(nano_seq, curr_time - uuid_time - 1);
        curr_time -= give_back;
        nano_seq -= give_back;
    }

    // borrow an extra nano_seq if curr_time == uuid_time to prevent timestamp collision
    if (curr_time == uuid_time) {
        /*
         * If nanoseq overflows, we need to start over with a new numberspace, cause in the next loop,
         * the value of curr_time may collide with an already generated value. So if nano_seq overflows,
         * we won't increase curr_time, then the curr_time will equal to uuid_time, which will lead to
         * new numberspace.
         */
        if (likely(++nano_seq)) {
            ++curr_time;
        }
    }

    // if first time creating uuid, or sys time changed backward, we reset clock_seq to prevent time stamp duplication.
    // meanwhile, we reset nano_seq.
    if (unlikely(curr_time <= uuid_time)) {
        pseudo_rand_read(clockSeq, CLOCK_SEQ_CHAR_NUM);
        string_to_hex(clockSeq, g_clockSeqAndMac, CLOCK_SEQ_CHAR_NUM, INT_RANGE_REVISE_PARAM);
        nano_seq = 0;
    }
    /* we need a local copy during hold the lock, so it won't be changed by other thread */
    int rc = strcpy_s(clockSeqAndMac, UUID_MID_PART_LEN + UUID_LAST_PART_LEN + 2, g_clockSeqAndMac);
    securec_check(rc, "", "");
    uuid_time = curr_time;
    localeLock.unLock();

    uint32 timestamp_low = (uint32)(curr_time & 0xFFFFFFFF);
    uint16 timestamp_mid = (uint16)((curr_time >> 32) & 0xFFFF);
    uint16 timestamp_high_and_v = (uint16)((curr_time >> 48) | UUID_VERSION);

    // result len equals to 32 + 4 * '-' + '\0'
    char* res = (char*)palloc(FORMATTED_UUID_LEN + 1);
    res[FORMATTED_UUID_LEN] = '\0';
    int cursor = 0;

    int_to_hex(timestamp_low, res, UUID_FIRST_PART_LEN);
    cursor += UUID_FIRST_PART_LEN;
    res[cursor++] = '-';

    int_to_hex(timestamp_mid, res + cursor, UUID_MID_PART_LEN);
    cursor += UUID_MID_PART_LEN;
    res[cursor++] = '-';

    int_to_hex(timestamp_high_and_v, res + cursor, UUID_MID_PART_LEN);
    cursor += UUID_MID_PART_LEN;
    res[cursor++] = '-';

    rc = strcpy_s(res + cursor, FORMATTED_UUID_LEN + 1 - cursor, clockSeqAndMac);
    securec_check(rc, "", "");

    VarChar* result = (VarChar*)cstring_to_text_with_len(res, FORMATTED_UUID_LEN);
    PG_RETURN_VARCHAR_P(result);
}