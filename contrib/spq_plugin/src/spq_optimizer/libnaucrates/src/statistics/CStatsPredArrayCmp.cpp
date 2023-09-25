//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2020 VMware, Inc.
//
//	@filename:
//		CStatsPredArrayCmp.cpp
//
//	@doc:
//		Implementation of statistics for ArrayCmp filter
//---------------------------------------------------------------------------

#include "naucrates/statistics/CStatsPredArrayCmp.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CColRefTable.h"
#include "naucrates/md/CMDIdSPQDB.h"

using namespace spqnaucrates;
using namespace spqopt;
using namespace spqmd;


// Ctor
CStatsPredArrayCmp::CStatsPredArrayCmp(ULONG colid,
									   CStatsPred::EStatsCmpType stats_cmp_type,
									   CPointArray *points)
	: CStatsPred(colid), m_stats_cmp_type(stats_cmp_type), m_points(points)
{
	SPQOS_ASSERT(CStatsPred::EstatscmptEq == m_stats_cmp_type);
}

// EOF
