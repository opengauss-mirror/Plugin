//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CConfigParamMapping.h
//
//	@doc:
//		Mapping of SPQDB config params to traceflags
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQOPT_CGUCMapping_H
#define SPQOPT_CGUCMapping_H

#include "spqos/base.h"
#include "spqos/common/CBitSet.h"
#include "spqos/memory/CMemoryPool.h"

#include "naucrates/traceflags/traceflags.h"

using namespace spqos;

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CConfigParamMapping
//
//	@doc:
//		Functionality for mapping SPQDB config params to traceflags
//
//---------------------------------------------------------------------------
class CConfigParamMapping
{
private:
	//------------------------------------------------------------------
	//	@class:
	//		SConfigMappingElem
	//
	//	@doc:
	//		Unit describing the mapping of a single SPQDB config param
	//		to a trace flag
	//
	//------------------------------------------------------------------
	struct SConfigMappingElem
	{
		// trace flag
		EOptTraceFlag m_trace_flag;

		// config param address
		BOOL *m_is_param;

		// if true, we negate the config param value before setting traceflag value
		BOOL m_negate_param;

		// description
		const WCHAR *description_str;
	};

	// array of mapping elements
	static SConfigMappingElem m_elements[];

	// private ctor
	CConfigParamMapping(const CConfigParamMapping &);

public:
	// pack enabled optimizer config params in a traceflag bitset
	static CBitSet *PackConfigParamInBitset(CMemoryPool *mp, ULONG xform_id);
	static void InitConfigParamElements(CBitSet *traceflag_bitset);
};
}  // namespace spqdxl

#endif	// ! SPQOPT_CGUCMapping_H

// EOF
