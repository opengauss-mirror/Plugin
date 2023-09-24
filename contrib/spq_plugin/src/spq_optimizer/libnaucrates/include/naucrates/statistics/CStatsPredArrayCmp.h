//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2020 VMware, Inc.
//
//	@filename:
//		CStatsPredArrayCmp.h
//
//	@doc:
//		ArrayCmp filter on statistics
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CStatsPredArrayCmp_H
#define SPQNAUCRATES_CStatsPredArrayCmp_H

#include "spqos/base.h"

#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CPoint.h"
#include "naucrates/statistics/CStatsPred.h"

// fwd declarations
namespace spqopt
{
class CColRef;
}

namespace spqnaucrates
{
using namespace spqos;
using namespace spqmd;
using namespace spqopt;

class CStatsPredArrayCmp : public CStatsPred
{
private:
	// private copy ctor
	CStatsPredArrayCmp(const CStatsPredArrayCmp &);

	// private assignment operator
	CStatsPredArrayCmp &operator=(CStatsPredArrayCmp &);

	// comparison type
	CStatsPred::EStatsCmpType m_stats_cmp_type;

	CPointArray *m_points;

public:
	// ctor
	CStatsPredArrayCmp(ULONG colid, CStatsPred::EStatsCmpType stats_cmp_type,
					   CPointArray *points);

	// dtor
	virtual ~CStatsPredArrayCmp()
	{
		m_points->Release();
	}

	// comparison types for stats computation
	virtual CStatsPred::EStatsCmpType
	GetCmpType() const
	{
		return m_stats_cmp_type;
	}

	// filter type id
	virtual EStatsPredType
	GetPredStatsType() const
	{
		return CStatsPred::EsptArrayCmp;
	}

	virtual CPointArray *
	GetPoints() const
	{
		return m_points;
	}

	// conversion function
	static CStatsPredArrayCmp *
	ConvertPredStats(CStatsPred *pred_stats)
	{
		SPQOS_ASSERT(NULL != pred_stats);
		SPQOS_ASSERT(CStatsPred::EsptArrayCmp == pred_stats->GetPredStatsType());

		return dynamic_cast<CStatsPredArrayCmp *>(pred_stats);
	}

};	// class CStatsPredArrayCmp

}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_CStatsPredArrayCmp_H

// EOF
