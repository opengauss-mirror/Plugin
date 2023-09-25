//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CStatsPredJoin.h
//
//	@doc:
//		Join predicate used for join cardinality estimation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CStatsPredJoin_H
#define SPQNAUCRATES_CStatsPredJoin_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CRefCount.h"

#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CStatsPred.h"

namespace spqnaucrates
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CStatsPredJoin
//
//	@doc:
//		Join predicate used for join cardinality estimation
//---------------------------------------------------------------------------
class CStatsPredJoin : public CRefCount
{
private:
	// private copy ctor
	CStatsPredJoin(const CStatsPredJoin &);

	// private assignment operator
	CStatsPredJoin &operator=(CStatsPredJoin &);

	// column id
	ULONG m_colidOuter;

	// comparison type
	CStatsPred::EStatsCmpType m_stats_cmp_type;

	// column id
	ULONG m_colidInner;

public:
	// c'tor
	CStatsPredJoin(ULONG colid1, CStatsPred::EStatsCmpType stats_cmp_type,
				   ULONG colid2)
		: m_colidOuter(colid1),
		  m_stats_cmp_type(stats_cmp_type),
		  m_colidInner(colid2)
	{
	}

	// accessors
	BOOL
	HasValidColIdOuter() const
	{
		return spqos::ulong_max != m_colidOuter;
	}

	ULONG
	ColIdOuter() const
	{
		return m_colidOuter;
	}

	// comparison type
	CStatsPred::EStatsCmpType
	GetCmpType() const
	{
		return m_stats_cmp_type;
	}

	BOOL
	HasValidColIdInner() const
	{
		return spqos::ulong_max != m_colidInner;
	}

	ULONG
	ColIdInner() const
	{
		return m_colidInner;
	}

	// d'tor
	virtual ~CStatsPredJoin()
	{
	}

};	// class CStatsPredJoin

// array of filters
typedef CDynamicPtrArray<CStatsPredJoin, CleanupRelease> CStatsPredJoinArray;
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CStatsPredJoin_H

// EOF
