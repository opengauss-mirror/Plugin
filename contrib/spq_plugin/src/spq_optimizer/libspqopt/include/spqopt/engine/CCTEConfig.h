//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CCTEConfig.h
//
//	@doc:
//		CTE configurations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CCTEConfig_H
#define SPQOPT_CCTEConfig_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/common/CRefCount.h"
#include "spqos/memory/CMemoryPool.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CCTEConfig
//
//	@doc:
//		CTE configurations
//
//---------------------------------------------------------------------------
class CCTEConfig : public CRefCount
{
private:
	// CTE inlining cut-off
	ULONG m_ulCTEInliningCutoff;

	// private copy ctor
	CCTEConfig(const CCTEConfig &);

public:
	// ctor
	CCTEConfig(ULONG cte_inlining_cut_off)
		: m_ulCTEInliningCutoff(cte_inlining_cut_off)
	{
	}

	// CTE inlining cut-off
	ULONG
	UlCTEInliningCutoff() const
	{
		return m_ulCTEInliningCutoff;
	}

	// generate default optimizer configurations
	static CCTEConfig *
	PcteconfDefault(CMemoryPool *mp)
	{
		return SPQOS_NEW(mp) CCTEConfig(0 /* cte_inlining_cut_off */);
	}

};	// class CCTEConfig
}  // namespace spqopt

#endif	// !SPQOPT_CCTEConfig_H

// EOF
