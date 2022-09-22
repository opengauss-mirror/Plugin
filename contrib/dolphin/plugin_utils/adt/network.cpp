/*
 *	PostgreSQL type definitions for the INET and CIDR types.
 *
 *	src/backend/utils/adt/network.c
 *
 *	Jon Postel RIP 16 Oct 1998
 */

#include "postgres.h"
#include "knl/knl_variable.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "access/hash.h"
#include "catalog/pg_type.h"
#include "libpq/ip.h"
#include "libpq/libpq-be.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "utils/builtins.h"
#include "utils/inet.h"
#include "plugin_postgres.h"

static int32 network_cmp_internal(inet* a1, inet* a2);
static int bitncmp(const void* l, const void* r, int n);
static bool addressOK(unsigned char* a, int bits, int family);
static int ip_addrsize(inet* inetptr);
static inet* internal_inetpl(inet* ip, int64 addend);

/*
 *	Access macros.	We use VARDATA_ANY so that we can process short-header
 *	varlena values without detoasting them.  This requires a trick:
 *	VARDATA_ANY assumes the varlena header is already filled in, which is
 *	not the case when constructing a new value (until SET_INET_VARSIZE is
 *	called, which we typically can't do till the end).  Therefore, we
 *	always initialize the newly-allocated value to zeroes (using palloc0).
 *	A zero length word will look like the not-1-byte case to VARDATA_ANY,
 *	and so we correctly construct an uncompressed value.
 *
 *	Note that ip_maxbits() and SET_INET_VARSIZE() require
 *	the family field to be set correctly.
 */

#define ip_family(inetptr) (((inet_struct*)VARDATA_ANY(inetptr))->family)

#define ip_bits(inetptr) (((inet_struct*)VARDATA_ANY(inetptr))->bits)

#define ip_addr(inetptr) (((inet_struct*)VARDATA_ANY(inetptr))->ipaddr)

#define ip_maxbits(inetptr) ((ip_family(inetptr) == PGSQL_AF_INET) ? 32 : 128)

#define SET_INET_VARSIZE(dst) SET_VARSIZE(dst, VARHDRSZ + offsetof(inet_struct, ipaddr) + ip_addrsize(dst))

/*
 * Return the number of bytes of address storage needed for this data type.
 */
static int ip_addrsize(inet* inetptr)
{
    switch (ip_family(inetptr)) {
        case PGSQL_AF_INET:
            return 4;
        case PGSQL_AF_INET6:
            return 16;
        default:
            return 0;
    }
}

/*
 * Common INET/CIDR input routine
 */
static inet* network_in(char* src, bool is_cidr)
{
    int bits;
    inet* dst = NULL;

    dst = (inet*)palloc0(sizeof(inet));

    /*
     * First, check to see if this is an IPv6 or IPv4 address.	IPv6 addresses
     * will have a : somewhere in them (several, in fact) so if there is one
     * present, assume it's V6, otherwise assume it's V4.
     */

    if (strchr(src, ':') != NULL)
        ip_family(dst) = PGSQL_AF_INET6;
    else
        ip_family(dst) = PGSQL_AF_INET;

    bits = inet_net_pton(ip_family(dst), src, ip_addr(dst), is_cidr ? ip_addrsize(dst) : -1);
    if ((bits < 0) || (bits > ip_maxbits(dst)))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                /* translator: first %s is inet or cidr */
                errmsg("invalid input syntax for type %s: \"%s\"", is_cidr ? "cidr" : "inet", src)));

    /*
     * Error check: CIDR values must not have any bits set beyond the masklen.
     */
    if (is_cidr) {
        if (!addressOK(ip_addr(dst), bits, ip_family(dst)))
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                    errmsg("invalid cidr value: \"%s\"", src),
                    errdetail("Value has bits set to right of mask.")));
    }

    ip_bits(dst) = bits;
    SET_INET_VARSIZE(dst);

    return dst;
}

Datum inet_in(PG_FUNCTION_ARGS)
{
    char* src = PG_GETARG_CSTRING(0);

    PG_RETURN_INET_P(network_in(src, false));
}

Datum cidr_in(PG_FUNCTION_ARGS)
{
    char* src = PG_GETARG_CSTRING(0);

    PG_RETURN_INET_P(network_in(src, true));
}

/*
 * Common INET/CIDR output routine
 */
static char* network_out(inet* src, bool is_cidr)
{
    char tmp[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255/128")];
    char* dst = NULL;
    int len;

    dst = inet_net_ntop(ip_family(src), ip_addr(src), ip_bits(src), tmp, sizeof(tmp));
    if (dst == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("could not format inet value: %m")));

    /* For CIDR, add /n if not present */
    if (is_cidr && strchr(tmp, '/') == NULL) {
        len = strlen(tmp);
        errno_t ss_rc = snprintf_s(tmp + len, sizeof(tmp) - len, sizeof(tmp) - len - 1, "/%u", ip_bits(src));
        securec_check_ss(ss_rc, "\0", "\0");
    }

    return pstrdup(tmp);
}

Datum inet_out(PG_FUNCTION_ARGS)
{
    inet* src = PG_GETARG_INET_PP(0);

    char* result = network_out(src, false);
    /* free memory if allocated by the toaster */
    PG_FREE_IF_COPY(src, 0);

    PG_RETURN_CSTRING(result);
}

Datum cidr_out(PG_FUNCTION_ARGS)
{
    inet* src = PG_GETARG_INET_PP(0);

    char* result = network_out(src, true);
    /* free memory if allocated by the toaster */
    PG_FREE_IF_COPY(src, 0);

    PG_RETURN_CSTRING(result);
}

/*
 *		network_recv		- converts external binary format to inet
 *
 * The external representation is (one byte apiece for)
 * family, bits, is_cidr, address length, address in network byte order.
 *
 * Presence of is_cidr is largely for historical reasons, though it might
 * allow some code-sharing on the client side.	We send it correctly on
 * output, but ignore the value on input.
 */
static inet* network_recv(StringInfo buf, bool is_cidr)
{
    inet* addr = NULL;
    char* addrptr = NULL;
    int bits;
    int nb, i;

    /* make sure any unused bits in a CIDR value are zeroed */
    addr = (inet*)palloc0(sizeof(inet));

    ip_family(addr) = pq_getmsgbyte(buf);
    if (ip_family(addr) != PGSQL_AF_INET && ip_family(addr) != PGSQL_AF_INET6)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                /* translator: %s is inet or cidr */
                errmsg("invalid address family in external \"%s\" value", is_cidr ? "cidr" : "inet")));
    bits = pq_getmsgbyte(buf);
    if (bits < 0 || bits > ip_maxbits(addr))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                /* translator: %s is inet or cidr */
                errmsg("invalid bits in external \"%s\" value", is_cidr ? "cidr" : "inet")));
    ip_bits(addr) = bits;
    i = pq_getmsgbyte(buf); /* ignore is_cidr */
    nb = pq_getmsgbyte(buf);
    if (nb != ip_addrsize(addr))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                /* translator: %s is inet or cidr */
                errmsg("invalid length in external \"%s\" value", is_cidr ? "cidr" : "inet")));

    addrptr = (char*)ip_addr(addr);
    for (i = 0; i < nb; i++)
        addrptr[i] = pq_getmsgbyte(buf);

    /*
     * Error check: CIDR values must not have any bits set beyond the masklen.
     */
    if (is_cidr) {
        if (!addressOK(ip_addr(addr), bits, ip_family(addr)))
            ereport(ERROR,
                (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                    errmsg("invalid external \"cidr\" value"),
                    errdetail("Value has bits set to right of mask.")));
    }

    SET_INET_VARSIZE(addr);

    return addr;
}

Datum inet_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_INET_P(network_recv(buf, false));
}

Datum cidr_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    PG_RETURN_INET_P(network_recv(buf, true));
}

/*
 *		network_send		- converts inet to binary format
 */
static bytea* network_send(inet* addr, bool is_cidr)
{
    StringInfoData buf;
    char* addrptr = NULL;
    int nb, i;

    pq_begintypsend(&buf);
    pq_sendbyte(&buf, ip_family(addr));
    pq_sendbyte(&buf, ip_bits(addr));
    pq_sendbyte(&buf, is_cidr);
    nb = ip_addrsize(addr);
    if (nb < 0)
        nb = 0;
    pq_sendbyte(&buf, nb);
    addrptr = (char*)ip_addr(addr);
    for (i = 0; i < nb; i++)
        pq_sendbyte(&buf, addrptr[i]);
    return pq_endtypsend(&buf);
}

Datum inet_send(PG_FUNCTION_ARGS)
{
    inet* addr = PG_GETARG_INET_PP(0);

    PG_RETURN_BYTEA_P(network_send(addr, false));
}

Datum cidr_send(PG_FUNCTION_ARGS)
{
    inet* addr = PG_GETARG_INET_PP(0);

    PG_RETURN_BYTEA_P(network_send(addr, true));
}

Datum inet_to_cidr(PG_FUNCTION_ARGS)
{
    inet* src = PG_GETARG_INET_PP(0);
    inet* dst = NULL;
    int bits;
    int byte;
    int nbits;
    int maxbytes;

    bits = ip_bits(src);

    /* safety check */
    if ((bits < 0) || (bits > ip_maxbits(src)))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid inet bit length: %d", bits)));

    /* clone the original data */
    dst = (inet*)palloc(VARSIZE_ANY(src));
    errno_t ss_rc = memcpy_s(dst, VARSIZE_ANY(src), src, VARSIZE_ANY(src));
    securec_check(ss_rc, "\0", "\0");

    /* zero out any bits to the right of the netmask */
    byte = bits / 8;

    nbits = bits % 8;
    /* clear the first byte, this might be a partial byte */
    if (nbits != 0) {
        ip_addr(dst)[byte] &= ~(unsigned int)(0xFF >> nbits);
        byte++;
    }
    /* clear remaining bytes */
    maxbytes = ip_addrsize(dst);
    while (byte < maxbytes) {
        ip_addr(dst)[byte] = 0;
        byte++;
    }

    PG_RETURN_INET_P(dst);
}

Datum inet_set_masklen(PG_FUNCTION_ARGS)
{
    inet* src = PG_GETARG_INET_PP(0);
    int bits = PG_GETARG_INT32(1);
    inet* dst = NULL;

    if (bits == -1)
        bits = ip_maxbits(src);

    if ((bits < 0) || (bits > ip_maxbits(src)))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid mask length: %d", bits)));

    /* clone the original data */
    dst = (inet*)palloc(VARSIZE_ANY(src));
    errno_t ss_rc = memcpy_s(dst, VARSIZE_ANY(src), src, VARSIZE_ANY(src));
    securec_check(ss_rc, "\0", "\0");

    ip_bits(dst) = bits;

    PG_RETURN_INET_P(dst);
}

Datum cidr_set_masklen(PG_FUNCTION_ARGS)
{
    inet* src = PG_GETARG_INET_PP(0);
    int bits = PG_GETARG_INT32(1);
    inet* dst = NULL;
    int byte;
    int nbits;
    int maxbytes;

    if (bits == -1)
        bits = ip_maxbits(src);

    if ((bits < 0) || (bits > ip_maxbits(src)))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid mask length: %d", bits)));

    /* clone the original data */
    dst = (inet*)palloc(VARSIZE_ANY(src));
    errno_t ss_rc = memcpy_s(dst, VARSIZE_ANY(src), src, VARSIZE_ANY(src));
    securec_check(ss_rc, "\0", "\0");

    ip_bits(dst) = bits;

    /* zero out any bits to the right of the new netmask */
    byte = bits / 8;

    nbits = bits % 8;
    /* clear the first byte, this might be a partial byte */
    if (nbits != 0) {
        ip_addr(dst)[byte] &= ~(unsigned int)(0xFF >> nbits);
        byte++;
    }
    /* clear remaining bytes */
    maxbytes = ip_addrsize(dst);
    while (byte < maxbytes) {
        ip_addr(dst)[byte] = 0;
        byte++;
    }

    PG_RETURN_INET_P(dst);
}

/*
 *	Basic comparison function for sorting and inet/cidr comparisons.
 *
 * Comparison is first on the common bits of the network part, then on
 * the length of the network part, and then on the whole unmasked address.
 * The effect is that the network part is the major sort key, and for
 * equal network parts we sort on the host part.  Note this is only sane
 * for CIDR if address bits to the right of the mask are guaranteed zero;
 * otherwise logically-equal CIDRs might compare different.
 */

static int32 network_cmp_internal(inet* a1, inet* a2)
{
    if (ip_family(a1) == ip_family(a2)) {
        int order;

        order = bitncmp(ip_addr(a1), ip_addr(a2), Min(ip_bits(a1), ip_bits(a2)));
        if (order != 0)
            return order;
        order = ((int)ip_bits(a1)) - ((int)ip_bits(a2));
        if (order != 0)
            return order;
        return bitncmp(ip_addr(a1), ip_addr(a2), ip_maxbits(a1));
    }

    return ip_family(a1) - ip_family(a2);
}

Datum network_cmp(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_INT32(network_cmp_internal(a1, a2));
}

/*
 *	Boolean ordering tests.
 */
Datum network_lt(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) < 0);
}

Datum network_le(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) <= 0);
}

Datum network_eq(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) == 0);
}

Datum network_ge(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) >= 0);
}

Datum network_gt(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) > 0);
}

Datum network_ne(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    PG_RETURN_BOOL(network_cmp_internal(a1, a2) != 0);
}
/*
 * MIN/MAX support functions.
 */
Datum network_smaller(PG_FUNCTION_ARGS)
{
    inet *a1 = PG_GETARG_INET_PP(0);
    inet *a2 = PG_GETARG_INET_PP(1);

    if (network_cmp_internal(a1, a2) < 0)
        PG_RETURN_INET_P(a1);
    else
        PG_RETURN_INET_P(a2);
}

Datum network_larger(PG_FUNCTION_ARGS)
{
    inet *a1 = PG_GETARG_INET_PP(0);
    inet *a2 = PG_GETARG_INET_PP(1);

    if (network_cmp_internal(a1, a2) > 0)
        PG_RETURN_INET_P(a1);
    else
        PG_RETURN_INET_P(a2);
}
/*
 * Support function for hash indexes on inet/cidr.
 */
Datum hashinet(PG_FUNCTION_ARGS)
{
    inet* addr = PG_GETARG_INET_PP(0);
    int addrsize = ip_addrsize(addr);

    /* XXX this assumes there are no pad bytes in the data structure */
    Datum result = hash_any((unsigned char*)VARDATA_ANY(addr), addrsize + 2);
    /* Avoid leaking memory for toasted inputs */
    PG_FREE_IF_COPY(addr, 0);

    return result;
}

/*
 *	Boolean network-inclusion tests.
 */
Datum network_sub(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    if (ip_family(a1) == ip_family(a2)) {
        PG_RETURN_BOOL(ip_bits(a1) > ip_bits(a2) && bitncmp(ip_addr(a1), ip_addr(a2), ip_bits(a2)) == 0);
    }

    PG_RETURN_BOOL(false);
}

Datum network_subeq(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    if (ip_family(a1) == ip_family(a2)) {
        PG_RETURN_BOOL(ip_bits(a1) >= ip_bits(a2) && bitncmp(ip_addr(a1), ip_addr(a2), ip_bits(a2)) == 0);
    }

    PG_RETURN_BOOL(false);
}

Datum network_sup(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    if (ip_family(a1) == ip_family(a2)) {
        PG_RETURN_BOOL(ip_bits(a1) < ip_bits(a2) && bitncmp(ip_addr(a1), ip_addr(a2), ip_bits(a1)) == 0);
    }

    PG_RETURN_BOOL(false);
}

Datum network_supeq(PG_FUNCTION_ARGS)
{
    inet* a1 = PG_GETARG_INET_PP(0);
    inet* a2 = PG_GETARG_INET_PP(1);

    if (ip_family(a1) == ip_family(a2)) {
        PG_RETURN_BOOL(ip_bits(a1) <= ip_bits(a2) && bitncmp(ip_addr(a1), ip_addr(a2), ip_bits(a1)) == 0);
    }

    PG_RETURN_BOOL(false);
}

/*
 * Extract data from a network datatype.
 */
Datum network_host(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    char* ptr = NULL;
    char tmp[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255/128")];

    /* force display of max bits, regardless of masklen... */
    if (inet_net_ntop(ip_family(ip), ip_addr(ip), ip_maxbits(ip), tmp, sizeof(tmp)) == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("could not format inet value: %m")));

    /* Suppress /n if present (shouldn't happen now) */
    if ((ptr = strchr(tmp, '/')) != NULL)
        *ptr = '\0';

    PG_RETURN_TEXT_P(cstring_to_text(tmp));
}

/*
 * network_show implements the inet and cidr casts to text.  This is not
 * quite the same behavior as network_out, hence we can't drop it in favor
 * of CoerceViaIO.
 */
Datum network_show(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    int len;
    char tmp[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255/128")];
    errno_t ss_rc;

    if (inet_net_ntop(ip_family(ip), ip_addr(ip), ip_maxbits(ip), tmp, sizeof(tmp)) == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("could not format inet value: %m")));

    /* Add /n if not present (which it won't be) */
    if (strchr(tmp, '/') == NULL) {
        len = strlen(tmp);
        ss_rc = snprintf_s(tmp + len, sizeof(tmp) - len, sizeof(tmp) - len - 1, "/%u", ip_bits(ip));
        securec_check_ss(ss_rc, "\0", "\0");
    }

    PG_RETURN_TEXT_P(cstring_to_text(tmp));
}

Datum inet_abbrev(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    char* dst = NULL;
    char tmp[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255/128")];

    dst = inet_net_ntop(ip_family(ip), ip_addr(ip), ip_bits(ip), tmp, sizeof(tmp));

    if (dst == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("could not format inet value: %m")));

    PG_RETURN_TEXT_P(cstring_to_text(tmp));
}

Datum cidr_abbrev(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    char* dst = NULL;
    char tmp[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:255.255.255.255/128")];

    dst = inet_cidr_ntop(ip_family(ip), ip_addr(ip), ip_bits(ip), tmp, sizeof(tmp));

    if (dst == NULL)
        ereport(ERROR, (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION), errmsg("could not format cidr value: %m")));

    PG_RETURN_TEXT_P(cstring_to_text(tmp));
}

Datum network_masklen(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);

    PG_RETURN_INT32(ip_bits(ip));
}

Datum network_family(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);

    switch (ip_family(ip)) {
        case PGSQL_AF_INET:
            PG_RETURN_INT32(4);
            break;
        case PGSQL_AF_INET6:
            PG_RETURN_INT32(6);
            break;
        default:
            PG_RETURN_INT32(0);
            break;
    }
}

#ifdef DOLPHIN
static int check_ip(char *src, bool is_check_v4)
{
    int bits;
    inet* dst = (inet*)palloc0(sizeof(inet));

    ip_family(dst) = is_check_v4 ? PGSQL_AF_INET : PGSQL_AF_INET6;

    bits = inet_net_pton(ip_family(dst), src, ip_addr(dst), -1);
    if ((bits < 0) || (bits > ip_maxbits(dst))) {
        pfree(dst);
        return 0;
    }

    pfree(dst);
    return 1;
}

static int is_ipvx(FunctionCallInfo fcinfo, bool is_check_v4)
{
    if PG_ARGISNULL(0)
        return 0;

    Oid argtypeid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (!OidIsValid(argtypeid))
        return 0;

    int result = 0;
    if (argtypeid == TEXTOID) {
        result = check_ip(text_to_cstring(PG_GETARG_TEXT_PP(0)), is_check_v4);
    } else if (argtypeid == INETOID) {
        result = (ip_family(PG_GETARG_INET_PP(0)) == (is_check_v4 ? PGSQL_AF_INET : PGSQL_AF_INET6));
    } else if (argtypeid == CSTRINGOID) {
        result = check_ip(PG_GETARG_CSTRING(0), is_check_v4);
    }

    return result;
}

PG_FUNCTION_INFO_V1_PUBLIC(network_is_ipv4);
extern "C" DLL_PUBLIC Datum network_is_ipv4(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(network_is_ipv6);
extern "C" DLL_PUBLIC Datum network_is_ipv6(PG_FUNCTION_ARGS);

Datum network_is_ipv4(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT32(is_ipvx(fcinfo, true));
}

Datum network_is_ipv6(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT32(is_ipvx(fcinfo, false));
}
#endif

Datum network_broadcast(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* dst = NULL;
    int byte;
    int bits;
    int maxbytes;
    unsigned char mask;
    unsigned char *a = NULL, *b = NULL;

    /* make sure any unused bits are zeroed */
    dst = (inet*)palloc0(sizeof(inet));

    if (ip_family(ip) == PGSQL_AF_INET)
        maxbytes = 4;
    else
        maxbytes = 16;

    bits = ip_bits(ip);
    a = ip_addr(ip);
    b = ip_addr(dst);

    for (byte = 0; byte < maxbytes; byte++) {
        if (bits >= 8) {
            mask = 0x00;
            bits -= 8;
        } else if (bits == 0)
            mask = 0xff;
        else {
            mask = 0xff >> bits;
            bits = 0;
        }

        b[byte] = a[byte] | mask;
    }

    ip_family(dst) = ip_family(ip);
    ip_bits(dst) = ip_bits(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

Datum network_network(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* dst = NULL;
    int byte;
    int bits;
    unsigned char mask;
    unsigned char *a, *b;

    /* make sure any unused bits are zeroed */
    dst = (inet*)palloc0(sizeof(inet));

    bits = ip_bits(ip);
    a = ip_addr(ip);
    b = ip_addr(dst);

    byte = 0;

    while (bits) {
        if (bits >= 8) {
            mask = 0xff;
            bits -= 8;
        } else {
            mask = 0xff << (unsigned int)(8 - bits);
            bits = 0;
        }

        b[byte] = a[byte] & mask;
        byte++;
    }

    ip_family(dst) = ip_family(ip);
    ip_bits(dst) = ip_bits(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

Datum network_netmask(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* dst = NULL;
    int byte;
    int bits;
    unsigned char mask;
    unsigned char* b = NULL;

    /* make sure any unused bits are zeroed */
    dst = (inet*)palloc0(sizeof(inet));

    bits = ip_bits(ip);
    b = ip_addr(dst);

    byte = 0;

    while (bits) {
        if (bits >= 8) {
            mask = 0xff;
            bits -= 8;
        } else {
            mask = 0xff << (unsigned int)(8 - bits);
            bits = 0;
        }

        b[byte] = mask;
        byte++;
    }

    ip_family(dst) = ip_family(ip);
    ip_bits(dst) = ip_maxbits(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

Datum network_hostmask(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* dst = NULL;
    int byte;
    int bits;
    int maxbytes;
    unsigned char mask;
    unsigned char* b = NULL;

    /* make sure any unused bits are zeroed */
    dst = (inet*)palloc0(sizeof(inet));

    if (ip_family(ip) == PGSQL_AF_INET)
        maxbytes = 4;
    else
        maxbytes = 16;

    bits = ip_maxbits(ip) - ip_bits(ip);
    b = ip_addr(dst);

    byte = maxbytes - 1;

    while (bits) {
        if (bits >= 8) {
            mask = 0xff;
            bits -= 8;
        } else {
            mask = 0xff >> (8 - bits);
            bits = 0;
        }
        if (unlikely(byte < 0)) {
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("could not format inet value")));
        }
        b[byte] = mask;
        byte--;
    }

    ip_family(dst) = ip_family(ip);
    ip_bits(dst) = ip_maxbits(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

/*
 * Convert a value of a network datatype to an approximate scalar value.
 * This is used for estimating selectivities of inequality operators
 * involving network types.
 */
double convert_network_to_scalar(Datum value, Oid typid)
{
    switch (typid) {
        case INETOID:
        case CIDROID: {
            inet* ip = DatumGetInetPP(value);
            int len;
            double res;
            int i;

            /*
             * Note that we don't use the full address for IPv6.
             */
            if (ip_family(ip) == PGSQL_AF_INET)
                len = 4;
            else
                len = 5;

            res = ip_family(ip);
            for (i = 0; i < len; i++) {
                res *= 256;
                res += ip_addr(ip)[i];
            }
            return res;

            break;
        }
        case MACADDROID: {
            macaddr* mac = DatumGetMacaddrP(value);
            double res;

            res = (mac->a << 16) | (mac->b << 8) | (mac->c);
            res *= 256 * 256 * 256;
            res += (mac->d << 16) | (mac->e << 8) | (mac->f);
            return res;
        }
        default:
            break;
    }

    /*
     * Can't get here unless someone tries to use scalarltsel/scalargtsel on
     * an operator with one network and one non-network operand.
     */
    ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("unsupported type: %u", typid)));
    return 0;
}

/*
 * int
 * bitncmp(l, r, n)
 *		compare bit masks l and r, for n bits.
 * return:
 *		-1, 1, or 0 in the libc tradition.
 * note:
 *		network byte order assumed.  this means 192.5.5.240/28 has
 *		0x11110000 in its fourth octet.
 */
static int bitncmp(const void* l, const void* r, int n)
{
    u_int lb, rb;
    int x, b;

    b = n / 8;
    x = memcmp(l, r, b);
    if (x || (n % 8) == 0)
        return x;

    lb = ((const u_char*)l)[b];
    rb = ((const u_char*)r)[b];
    for (b = n % 8; b > 0; b--) {
        if (IS_HIGHBIT_SET(lb) != IS_HIGHBIT_SET(rb)) {
            if (IS_HIGHBIT_SET(lb))
                return 1;
            return -1;
        }
        lb <<= 1;
        rb <<= 1;
    }
    return 0;
}

static bool addressOK(unsigned char* a, int bits, int family)
{
    int byte;
    int nbits;
    int maxbits;
    int maxbytes;
    unsigned char mask;

    if (family == PGSQL_AF_INET) {
        maxbits = 32;
        maxbytes = 4;
    } else {
        maxbits = 128;
        maxbytes = 16;
    }
    Assert(bits <= maxbits);

    if (bits == maxbits)
        return true;

    byte = bits / 8;

    nbits = bits % 8;
    mask = 0xff;
    if (bits != 0)
        mask >>= nbits;

    while (byte < maxbytes) {
        if ((a[byte] & mask) != 0)
            return false;
        mask = 0xff;
        byte++;
    }

    return true;
}

/*
 * These functions are used by planner to generate indexscan limits
 * for clauses a << b and a <<= b
 */

/* return the minimal value for an IP on a given network */
Datum network_scan_first(Datum in)
{
    return DirectFunctionCall1(network_network, in);
}

/*
 * return "last" IP on a given network. It's the broadcast address,
 * however, masklen has to be set to its max btis, since
 * 192.168.0.255/24 is considered less than 192.168.0.255/32
 *
 * inet_set_masklen() hacked to max out the masklength to 128 for IPv6
 * and 32 for IPv4 when given '-1' as argument.
 */
Datum network_scan_last(Datum in)
{
    return DirectFunctionCall2(inet_set_masklen, DirectFunctionCall1(network_broadcast, in), Int32GetDatum(-1));
}

/*
 * IP address that the client is connecting from (NULL if Unix socket)
 */
Datum inet_client_addr(PG_FUNCTION_ARGS)
{
    Port* port = u_sess->proc_cxt.MyProcPort;
    char remote_host[NI_MAXHOST];
    int ret;

    if (port == NULL)
        PG_RETURN_NULL();

    switch (port->raddr.addr.ss_family) {
        case AF_INET:
#ifdef HAVE_IPV6
        case AF_INET6:
#endif
            break;
        default:
            PG_RETURN_NULL();
            break;
    }

    remote_host[0] = '\0';

    ret = pg_getnameinfo_all(&port->raddr.addr,
        port->raddr.salen,
        remote_host,
        sizeof(remote_host),
        NULL,
        0,
        NI_NUMERICHOST | NI_NUMERICSERV);
    if (ret != 0)
        PG_RETURN_NULL();

    clean_ipv6_addr(port->raddr.addr.ss_family, remote_host);

    PG_RETURN_INET_P(network_in(remote_host, false));
}

/*
 * port that the client is connecting from (NULL if Unix socket)
 */
Datum inet_client_port(PG_FUNCTION_ARGS)
{
    Port* port = u_sess->proc_cxt.MyProcPort;
    char remote_port[NI_MAXSERV];
    int ret;

    if (port == NULL)
        PG_RETURN_NULL();

    switch (port->raddr.addr.ss_family) {
        case AF_INET:
#ifdef HAVE_IPV6
        case AF_INET6:
#endif
            break;
        default:
            PG_RETURN_NULL();
            break;
    }

    remote_port[0] = '\0';

    ret = pg_getnameinfo_all(&port->raddr.addr,
        port->raddr.salen,
        NULL,
        0,
        remote_port,
        sizeof(remote_port),
        NI_NUMERICHOST | NI_NUMERICSERV);
    if (ret != 0)
        PG_RETURN_NULL();

    PG_RETURN_DATUM(DirectFunctionCall1(int4in, CStringGetDatum(remote_port)));
}

/*
 * IP address that the server accepted the connection on (NULL if Unix socket)
 */
Datum inet_server_addr(PG_FUNCTION_ARGS)
{
    Port* port = u_sess->proc_cxt.MyProcPort;
    char local_host[NI_MAXHOST];
    int ret;

    if (port == NULL)
        PG_RETURN_NULL();

    switch (port->laddr.addr.ss_family) {
        case AF_INET:
#ifdef HAVE_IPV6
        case AF_INET6:
#endif
            break;
        default:
            PG_RETURN_NULL();
            break;
    }

    local_host[0] = '\0';

    ret = pg_getnameinfo_all(
        &port->laddr.addr, port->laddr.salen, local_host, sizeof(local_host), NULL, 0, NI_NUMERICHOST | NI_NUMERICSERV);
    if (ret != 0)
        PG_RETURN_NULL();

    clean_ipv6_addr(port->laddr.addr.ss_family, local_host);

    PG_RETURN_INET_P(network_in(local_host, false));
}

/*
 * port that the server accepted the connection on (NULL if Unix socket)
 */
Datum inet_server_port(PG_FUNCTION_ARGS)
{
    Port* port = u_sess->proc_cxt.MyProcPort;
    char local_port[NI_MAXSERV];
    int ret;

    if (port == NULL)
        PG_RETURN_NULL();

    switch (port->laddr.addr.ss_family) {
        case AF_INET:
#ifdef HAVE_IPV6
        case AF_INET6:
#endif
            break;
        default:
            PG_RETURN_NULL();
            break;
    }

    local_port[0] = '\0';

    ret = pg_getnameinfo_all(
        &port->laddr.addr, port->laddr.salen, NULL, 0, local_port, sizeof(local_port), NI_NUMERICHOST | NI_NUMERICSERV);
    if (ret != 0)
        PG_RETURN_NULL();

    PG_RETURN_DATUM(DirectFunctionCall1(int4in, CStringGetDatum(local_port)));
}

Datum inetnot(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* dst = NULL;

    dst = (inet*)palloc0(sizeof(inet));

    {
        int nb = ip_addrsize(ip);
        unsigned char* pip = ip_addr(ip);
        unsigned char* pdst = ip_addr(dst);

        while (nb-- > 0)
            pdst[nb] = ~pip[nb];
    }
    ip_bits(dst) = ip_bits(ip);

    ip_family(dst) = ip_family(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

Datum inetand(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* ip2 = PG_GETARG_INET_PP(1);
    inet* dst = NULL;

    dst = (inet*)palloc0(sizeof(inet));

    if (ip_family(ip) != ip_family(ip2))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot AND inet values of different sizes")));
    else {
        int nb = ip_addrsize(ip);
        unsigned char* pip = ip_addr(ip);
        unsigned char* pip2 = ip_addr(ip2);
        unsigned char* pdst = ip_addr(dst);

        while (nb-- > 0)
            pdst[nb] = pip[nb] & pip2[nb];
    }
    ip_bits(dst) = Max(ip_bits(ip), ip_bits(ip2));

    ip_family(dst) = ip_family(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

Datum inetor(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* ip2 = PG_GETARG_INET_PP(1);
    inet* dst = NULL;

    dst = (inet*)palloc0(sizeof(inet));

    if (ip_family(ip) != ip_family(ip2))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot OR inet values of different sizes")));
    else {
        int nb = ip_addrsize(ip);
        unsigned char* pip = ip_addr(ip);
        unsigned char* pip2 = ip_addr(ip2);
        unsigned char* pdst = ip_addr(dst);

        while (nb-- > 0)
            pdst[nb] = pip[nb] | pip2[nb];
    }
    ip_bits(dst) = Max(ip_bits(ip), ip_bits(ip2));

    ip_family(dst) = ip_family(ip);
    SET_INET_VARSIZE(dst);

    PG_RETURN_INET_P(dst);
}

static inet* internal_inetpl(inet* ip, int64 addend)
{
    inet* dst = NULL;

    dst = (inet*)palloc0(sizeof(inet));

    {
        int nb = ip_addrsize(ip);
        unsigned char* pip = ip_addr(ip);
        unsigned char* pdst = ip_addr(dst);
        int carry = 0;

        while (nb-- > 0) {
            carry = pip[nb] + (int)(addend & 0xFF) + carry;
            pdst[nb] = (unsigned char)(carry & 0xFF);
            carry >>= 8;

            /*
             * We have to be careful about right-shifting addend because
             * right-shift isn't portable for negative values, while simply
             * dividing by 256 doesn't work (the standard rounding is in the
             * wrong direction, besides which there may be machines out there
             * that round the wrong way).  So, explicitly clear the low-order
             * byte to remove any doubt about the correct result of the
             * division, and then divide rather than shift.
             */
            addend &= ~((int64)0xFF);
            addend /= 0x100;
        }

        /*
         * At this point we should have addend and carry both zero if original
         * addend was >= 0, or addend -1 and carry 1 if original addend was <
         * 0.  Anything else means overflow.
         */
        if (!((addend == 0 && carry == 0) || (addend == -1 && carry == 1)))
            ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("result is out of range")));
    }

    ip_bits(dst) = ip_bits(ip);
    ip_family(dst) = ip_family(ip);
    SET_INET_VARSIZE(dst);

    return dst;
}

Datum inetpl(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    int64 addend = PG_GETARG_INT64(1);

    PG_RETURN_INET_P(internal_inetpl(ip, addend));
}

Datum inetmi_int8(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    int64 addend = PG_GETARG_INT64(1);

    PG_RETURN_INET_P(internal_inetpl(ip, -addend));
}

Datum inetmi(PG_FUNCTION_ARGS)
{
    inet* ip = PG_GETARG_INET_PP(0);
    inet* ip2 = PG_GETARG_INET_PP(1);
    int64 res = 0;

    if (ip_family(ip) != ip_family(ip2))
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("cannot subtract inet values of different sizes")));
    else {
        /*
         * We form the difference using the traditional complement, increment,
         * and add rule, with the increment part being handled by starting the
         * carry off at 1.	If you don't think integer arithmetic is done in
         * two's complement, too bad.
         */
        int nb = ip_addrsize(ip);
        int byte = 0;
        unsigned char* pip = ip_addr(ip);
        unsigned char* pip2 = ip_addr(ip2);
        int carry = 1;

        while (nb-- > 0) {
            int lobyte;

            carry = pip[nb] + (~pip2[nb] & 0xFF) + carry;
            lobyte = carry & 0xFF;
            if ((unsigned int)(byte) < sizeof(int64)) {
                res |= ((int64)lobyte) << (unsigned int)(byte * 8);
            } else {
                /*
                 * Input wider than int64: check for overflow.	All bytes to
                 * the left of what will fit should be 0 or 0xFF, depending on
                 * sign of the now-complete result.
                 */
                if ((res < 0) ? (lobyte != 0xFF) : (lobyte != 0))
                    ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("result is out of range")));
            }
            carry >>= 8;
            byte++;
        }

        /*
         * If input is narrower than int64, overflow is not possible, but we
         * have to do proper sign extension.
         */
        if (carry == 0 && (unsigned int)(byte) < sizeof(int64))
            res |= ((int64)-1) << (unsigned int)(byte * 8);
    }

    PG_RETURN_INT64(res);
}

#ifdef DOLPHIN
PG_FUNCTION_INFO_V1_PUBLIC(inetaton);
extern "C" DLL_PUBLIC Datum inetaton(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(inetpton);
extern "C" DLL_PUBLIC Datum inetpton(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(inetntoa);
extern "C" DLL_PUBLIC Datum inetntoa(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(inetntop);
extern "C" DLL_PUBLIC Datum inetntop(PG_FUNCTION_ARGS);

static const int IN6ADDRSZ = sizeof(in6_addr);
static const int INADDRSZ = sizeof(in_addr);
static const int IN6ADDRNWS = IN6ADDRSZ / 2;
static const char HEXDIGS[] = "0123456789abcdef";
static const int IPV4_LEN_MIN = 7;
static const int IPV4_LEN_MAX = 15;
static const int IN6_SEP_MAX = 7;
static const int IN6_SEP_MIN = 2;
static const int INADDPARTNUM = 4;

Datum inetaton(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    Oid argtypeid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (!OidIsValid(argtypeid))
        PG_RETURN_NULL();

    char *cp = NULL;
    if (argtypeid == BOOLOID) {
        if (PG_GETARG_BOOL(0))
            PG_RETURN_INT64(1);
        PG_RETURN_INT64(0);
    } else
        cp = text_to_cstring(PG_GETARG_TEXT_PP(0));
    if (*cp == '\0')
        PG_RETURN_NULL();
    uint64 val;
    int n;
    char c;
    u_int parts[INADDRSZ];
    u_int *pp = parts;

    for (;;) {
        val = 0;
        while ((c = *cp) != '\0') {
            if (isdigit((unsigned char)c)) {
                val = (val * 10) + (c - '0');
                if (val > 255)
                    PG_RETURN_NULL();
                cp++;
                continue;
            }
            break;
        }
        if (*cp == '.') {
            if (pp >= parts + INADDRSZ - 1 || val > 0xff) {
                PG_RETURN_NULL();
            }
            *pp++ = val;
            cp++;
            if (!isdigit((unsigned char)*cp))
                PG_RETURN_NULL();
        } else {
            break;
        }
    }

    while (*cp) {
        if (!isspace((unsigned char)*cp++)) {
            PG_RETURN_NULL();
        }
    }

    n = pp - parts + 1;
    switch (n) {
        case 1: /* a -- 32 bits */
            break;
        case 2: /* a.b -- 8.24 bits */
            if (val > 0xffffff) {
                PG_RETURN_NULL();
            }
            val |= parts[0] << 24;
            break;
        case 3: /* a.b.c -- 8.8.16 bits */
            if (val > 0xffff) {
                PG_RETURN_NULL();
            }
            val |= (parts[0] << 24) | (parts[1] << 16);
            break;
        case 4: /* a.b.c.d -- 8.8.8.8 bits */
            if (val > 0xff) {
                PG_RETURN_NULL();
            }
            val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
            break;
        default:
            break;
    }
    PG_RETURN_INT64(val);
}

static bool ptonipv4(const char *src, int length, unsigned char *address)
{
    if (length < IPV4_LEN_MIN || length > IPV4_LEN_MAX) {
        return false;
    }

    unsigned char *bytes = address;
    int value = 0;
    int part_len = 0;
    int dot_num = 0;
    char c = 0;

    for (const char *p = src; (p - src) < length; p++) {
        c = *p;
        if (isdigit(c)) {
            ++part_len;
            if (part_len > INADDPARTNUM - 1) {
                return false;
            }
            value = value * 10 + (c - '0');
            if (value > 255) {
                return false;
            }
        } else if (c == '.') {
            if (part_len == 0) {
                return false;
            }
            bytes[dot_num++] = (unsigned char)value;
            value = 0;
            part_len = 0;
            if (dot_num > INADDPARTNUM - 1) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (c == '.' || dot_num != INADDPARTNUM - 1) {
        return false;
    }
    bytes[INADDPARTNUM - 1] = (unsigned char)value;
    return true;
}

static bool ptonipv6(const char *src, int length, char *address)
{
    if (length < IN6_SEP_MIN || length > IN6ADDRNWS * 4 + IN6_SEP_MAX) {
        return false;
    }

    const char *p = src;
    if (*p == ':') {
        ++p;
        if (*p != ':') {
            return false;
        }
    }
    char *bytes = address;
    char *bytes_end = bytes + IN6ADDRSZ;
    char *dst = bytes;
    char *blank = NULL;
    const char *part_begin = p;
    int part_len = 0;
    int part_digit = 0;
    for (; ((p - src) < length); p++) {
        if (!*p)
            break;
        char c = *p;
        if (c == ':') {
            part_begin = p + 1;

            if (!part_len) {
                if (blank) {
                    return false;
                }

                blank = dst;
                continue;
            }

            if (((part_begin - src) >= length) || !*(part_begin)) {
                return false;
            }

            if (dst + 2 > bytes_end) {
                return false;
            }

            dst[0] = (unsigned char)(part_digit >> 8) & 0xff;
            dst[1] = (unsigned char)part_digit & 0xff;
            dst += 2;
            part_len = 0;
            part_digit = 0;
        } else if (c == '.') {
            if (dst + INADDRSZ > bytes_end) {
                return false;
            }
            int v4_start = part_begin - src;
            if (!ptonipv4(part_begin, length - v4_start, (unsigned char *)dst)) {
                return false;
            }

            dst += INADDRSZ;
            part_len = 0;

            break;
        } else {
            const char *hdp = strchr(HEXDIGS, tolower(c));

            if (!hdp || part_len >= 4) {
                return false;
            }

            part_digit <<= 4;
            part_digit |= hdp - HEXDIGS;
            Assert(part_digit <= 0xffff);
            part_len++;
        }
    }
    if (part_len > 0) {
        if (dst + 2 > bytes_end) {
            return false;
        }

        dst[0] = (unsigned char)(part_digit >> 8) & 0xff;
        dst[1] = (unsigned char)part_digit & 0xff;
        dst += 2;
    }
    if (blank) {
        if (dst == bytes_end) {
            return false;
        }

        size_t bytes_to_move = dst - blank;

        for (size_t i = 1; i <= bytes_to_move; ++i) {
            bytes_end[-(static_cast<ssize_t>(i))] = blank[bytes_to_move - i];
            blank[bytes_to_move - i] = 0;
        }
        dst = bytes_end;
    }
    if (dst < bytes_end) {
        return false;
    }
    return true;
}

Datum inetpton(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    char *ip = text_to_cstring(PG_GETARG_TEXT_PP(0));
    char res_ipv4[INADDRSZ] = {0};
    char res_ipv6[IN6ADDRSZ] = {0};

    if (ptonipv4(ip, strlen(ip), (unsigned char *)res_ipv4)) {
        PG_RETURN_TEXT_P(cstring_to_text_with_len(res_ipv4, INADDRSZ));
    }
    if (ptonipv6(ip, strlen(ip), res_ipv6)) {
        PG_RETURN_TEXT_P(cstring_to_text_with_len(res_ipv6, IN6ADDRSZ));
    }
    PG_RETURN_NULL();
}

Datum inetntoa(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    Oid argtypeid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    if (!OidIsValid(argtypeid))
        PG_RETURN_NULL();

    uint64 n = 0;
    if (argtypeid == BOOLOID) {
        n = PG_GETARG_BOOL(0) ? 1 : 0;
    } else
        n = PG_GETARG_INT64(0);

    if (n > UINT32_MAX) {
        PG_RETURN_NULL();
    }
    char res[IPV4_LEN_MAX + 2] = {0};
    errno_t rc = EOK;

    unsigned char buf[INADDRSZ];
    *((unsigned int *)buf) = n;

    for (unsigned char *p = buf + INADDRSZ - 1; p >= buf; p--) {
        unsigned int c = *p;
        char digit[INADDRSZ + 1] = {0};

        rc = sprintf_s(digit, sizeof(digit), "%u.", c);
        securec_check_ss(rc, "\0", "\0");

        rc = strcat_s(res, sizeof(res), digit);
        securec_check(rc, "\0", "\0");
    }
    res[strlen(res) - 1] = '\0';
    PG_RETURN_TEXT_P(cstring_to_text(res));
}

static void find_gap(unsigned short *src, int *pos, int *length)
{
    int tmplen = -1;
    int tmppos = -1;
    for (int i = 0; i < IN6ADDRNWS; i++) {
        if (src[i] != 0) {
            if (tmppos >= 0) {
                if (tmplen > *length) {
                    *pos = tmppos;
                    *length = tmplen;
                }
                tmppos = -1;
                tmplen = -1;
            }
        } else {
            if (tmppos >= 0) {
                tmplen++;
            } else {
                tmppos = i;
                tmplen = 1;
            }
        }
    }
    if (tmppos >= 0) {
        if (tmplen > *length)
            *pos = tmppos;
        *length = tmplen;
    }
    return;
}

static void ntopipv4(const in_addr *src, char *str)
{
    const unsigned char *bytes = (const unsigned char *)src;
    errno_t rc = 0;
    rc = sprintf_s(str, INET_ADDRSTRLEN, "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
    securec_check_ss(rc, "\0", "\0");
    return;
}

static void ntopipv6(const in6_addr *src, char *str)
{
    int pos = -1;
    int length = -1;
    const unsigned char *bytes = (const unsigned char *)src;
    uint16 words[IN6ADDRNWS];

    for (int i = 0; i < IN6ADDRNWS; ++i)
        words[i] = (bytes[2 * i] << 8) + bytes[2 * i + 1];
    find_gap(words, &pos, &length);
    char *p = str;

    for (int i = 0; i < IN6ADDRNWS; ++i) {
        if (i == pos) {
            if (i == 0) {
                *p = ':';
                ++p;
            }

            *p = ':';
            ++p;

            i += length - 1;
        } else if (i == IN6ADDRNWS - 2 && pos == 0 &&
                   (length == IN6ADDRNWS - 2 || (length == IN6ADDRNWS - 3 && words[IN6ADDRNWS - 3] == 0xffff))) {
            ntopipv4((const in_addr *)(bytes + 12), p);
            return;
        } else {
            errno_t rc = 0;
            rc = sprintf_s(p, INET_ADDRSTRLEN, "%x", words[i]);
            securec_check_ss(rc, "\0", "\0");
            if (rc < 0)
                return;
            p += rc;

            if (i != IN6ADDRNWS - 1) {
                *p = ':';
                ++p;
            }
        }
    }
    *p = 0;
    return;
}

Datum inetntop(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_NULL();

    bytea *in = PG_GETARG_BYTEA_PP(0);
    char *ip = VARDATA_ANY(in);
    int len = VARSIZE_ANY_EXHDR(in);

    if (len == INADDRSZ) {
        char res_address[INET_ADDRSTRLEN] = {0};
        ntopipv4((const in_addr *)ip, res_address);
        PG_RETURN_TEXT_P(cstring_to_text(res_address));
    } else if (len == IN6ADDRSZ) {
        char res_address[INET6_ADDRSTRLEN] = {0};
        ntopipv6((const in6_addr *)ip, res_address);
        PG_RETURN_TEXT_P(cstring_to_text(res_address));
    }
    PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1_PUBLIC(is_v4compat);
extern "C" DLL_PUBLIC Datum is_v4compat(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1_PUBLIC(is_v4mapped);
extern "C" DLL_PUBLIC Datum is_v4mapped(PG_FUNCTION_ARGS);

Datum is_v4compat(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_INT16(0);

    bytea *in = PG_GETARG_BYTEA_PP(0);
    char *ip = VARDATA_ANY(in);
    int len = VARSIZE_ANY_EXHDR(in);

    if (len != IN6ADDRSZ)
        PG_RETURN_INT16(0);

    PG_RETURN_INT16(IN6_IS_ADDR_V4COMPAT((const in6_addr *)ip) == true ? 1 : 0);
}

Datum is_v4mapped(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        PG_RETURN_INT16(0);

    bytea *in = PG_GETARG_BYTEA_PP(0);
    char *ip = VARDATA_ANY(in);
    int len = VARSIZE_ANY_EXHDR(in);

    if (len != IN6ADDRSZ)
        PG_RETURN_INT16(0);

    PG_RETURN_INT16(IN6_IS_ADDR_V4MAPPED((const in6_addr *)ip) == true ? 1 : 0);
}
#endif

/*
 * clean_ipv6_addr --- remove any '%zone' part from an IPv6 address string
 *
 * XXX This should go away someday!
 *
 * This is a kluge needed because we don't yet support zones in stored inet
 * values.	Since the result of getnameinfo() might include a zone spec,
 * call this to remove it anywhere we want to feed getnameinfo's output to
 * network_in.	Beats failing entirely.
 *
 * An alternative approach would be to let network_in ignore %-parts for
 * itself, but that would mean we'd silently drop zone specs in user input,
 * which seems not such a good idea.
 */
void clean_ipv6_addr(int addr_family, char* addr)
{
#ifdef HAVE_IPV6
    if (addr_family == AF_INET6) {
        char* pct = strchr(addr, '%');

        if (pct != NULL)
            *pct = '\0';
    }
#endif
}
