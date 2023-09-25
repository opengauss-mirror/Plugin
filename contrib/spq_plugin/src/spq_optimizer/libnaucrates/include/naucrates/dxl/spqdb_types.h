//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		spqdb_types.h
//
//	@doc:
//		Types from SPQDB.
//---------------------------------------------------------------------------
#ifndef SPQDXL_spqdb_types_H
#define SPQDXL_spqdb_types_H

#include "spqos/types.h"

using namespace spqos;

typedef ULONG OID;

#define SPQDB_INT2 OID(21)
#define SPQDB_INT4 OID(23)
#define SPQDB_INT8 OID(20)
#define SPQDB_BOOL OID(16)
#define SPQDB_TID OID(27)
#define SPQDB_OID OID(26)
#define SPQDB_XID OID(28)
#define SPQDB_CID OID(29)
#define SPQDB_NID OID(30)
#define SPQDB_BID OID(31)
#define SPQDB_UID OID(32)

#define SPQDB_NUMERIC OID(1700)
#define SPQDB_FLOAT4 OID(700)
#define SPQDB_FLOAT8 OID(701)
#define SPQDB_CASH OID(790)

// time related types
#define SPQDB_DATE OID(1082)
#define SPQDB_TIME OID(1083)
#define SPQDB_TIMETZ OID(1266)
#define SPQDB_TIMESTAMP OID(1114)
#define SPQDB_TIMESTAMPTZ OID(1184)
#define SPQDB_ABSTIME OID(702)
#define SPQDB_RELTIME OID(703)
#define SPQDB_INTERVAL OID(1186)
#define SPQDB_TIMEINTERVAL OID(704)

// text related types
#define SPQDB_CHAR OID(1042)
#define SPQDB_VARCHAR OID(1043)
#define SPQDB_TEXT OID(25)
#define SPQDB_NAME OID(19)
#define SPQDB_SINGLE_CHAR OID(18)

// network related types
#define SPQDB_INET OID(869)
#define SPQDB_CIDR OID(650)
#define SPQDB_MACADDR OID(829)

#define SPQDB_UNKNOWN OID(705)

#define SPQDB_DATE_TIMESTAMP_EQUALITY OID(2347)
#define SPQDB_TIMESTAMP_DATE_EQUALITY OID(2373)
#define SPQDB_DATE_TO_TIMESTAMP_CAST OID(2024)
#define SPQDB_TIMESTAMP_TO_DATE_CAST OID(1082)

#define SPQDB_COUNT_STAR OID(2803)  // count(*)
#define SPQDB_COUNT_ANY OID(2147)   // count(Any)
#define SPQDB_UUID OID(2950)
#define SPQDB_ANY OID(2283)
#define SPQDB_INT8_CAST OID(1779)  // int8(numeric)
#define SPQDB_PERCENTILE_DISC \
	OID(3972)  // percentile_disc(fraction) group within(ANYELEMENT)
#define SPQDB_PERCENTILE_CONT_FLOAT8 \
	OID(3974)  // percentile_cont(fraction) group within(FLOAT8)
#define SPQDB_PERCENTILE_CONT_INTERVAL \
	OID(3976)  // percentile_cont(fraction) group within(INTERVAL)
#define SPQDB_PERCENTILE_CONT_TIMESTAMP \
	OID(6119)  // percentile_cont(fraction) group within(timestamp)
#define SPQDB_PERCENTILE_CONT_TIMESTAMPTZ \
	OID(6123)  // percentile_cont(fraction) group within(timestamptz)
#define SPQDB_MEDIAN_FLOAT8 OID(6127)  // median(fraction) group within(FLOAT8)
#define SPQDB_MEDIAN_INTERVAL \
	OID(6128)  // median(fraction) group within(INTERVAL)
#define SPQDB_MEDIAN_TIMESTAMP \
	OID(6129)  // median(fraction) group within(timestamp)
#define SPQDB_MEDIAN_TIMESTAMPTZ \
	OID(6130)  // median(fraction) group within(timestamptz)

#endif	// !SPQDXL_spqdb_types_H


// EOF
