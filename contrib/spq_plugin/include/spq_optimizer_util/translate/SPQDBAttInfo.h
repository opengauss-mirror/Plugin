//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		SPQDBAttInfo.h
//
//	@doc:
//		Class to uniquely identify a column in SPQDB
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_SPQDBAttInfo_H
#define SPQDXL_SPQDBAttInfo_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"
#include "spqos/utils.h"

#include "naucrates/dxl/spqdb_types.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		SPQDBAttInfo
//
//	@doc:
//		Class to uniquely identify a column in SPQDB
//
//---------------------------------------------------------------------------
class SPQDBAttInfo : public CRefCount
{
private:
	// query level number
	ULONG m_query_level;

	// varno in the rtable
	ULONG m_varno;

	// attno
	INT m_attno;

	// copy c'tor
	SPQDBAttInfo(const SPQDBAttInfo &);

public:
	// ctor
	SPQDBAttInfo(ULONG query_level, ULONG var_no, INT attrnum)
		: m_query_level(query_level), m_varno(var_no), m_attno(attrnum)
	{
	}

	// d'tor
	virtual ~SPQDBAttInfo()
	{
	}

	// accessor
	ULONG
	GetQueryLevel() const
	{
		return m_query_level;
	}

	// accessor
	ULONG
	GetVarNo() const
	{
		return m_varno;
	}

	// accessor
	INT
	GetAttNo() const
	{
		return m_attno;
	}

	// equality check
	BOOL
	Equals(const SPQDBAttInfo &spqdb_att_info) const
	{
		return m_query_level == spqdb_att_info.m_query_level &&
			   m_varno == spqdb_att_info.m_varno &&
			   m_attno == spqdb_att_info.m_attno;
	}

	// hash value
	ULONG
	HashValue() const
	{
		return spqos::CombineHashes(
			spqos::HashValue(&m_query_level),
			spqos::CombineHashes(spqos::HashValue(&m_varno),
								spqos::HashValue(&m_attno)));
	}
};

// hash function
inline ULONG
HashSPQDBAttInfo(const SPQDBAttInfo *spqdb_att_info)
{
	SPQOS_ASSERT(NULL != spqdb_att_info);
	return spqdb_att_info->HashValue();
}

// equality function
inline BOOL
EqualSPQDBAttInfo(const SPQDBAttInfo *spqdb_att_info_a,
				 const SPQDBAttInfo *spqdb_att_info_b)
{
	SPQOS_ASSERT(NULL != spqdb_att_info_a && NULL != spqdb_att_info_b);
	return spqdb_att_info_a->Equals(*spqdb_att_info_b);
}

}  // namespace spqdxl

#endif	// !SPQDXL_SPQDBAttInfo_H

// EOF
