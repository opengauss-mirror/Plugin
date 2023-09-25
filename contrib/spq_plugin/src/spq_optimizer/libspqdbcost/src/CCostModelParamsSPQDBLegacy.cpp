//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CCostModelParamsSPQDBLegacy.cpp
//
//	@doc:
//		Parameters of SPQDB's legacy cost model
//---------------------------------------------------------------------------

#include "spqdbcost/CCostModelParamsSPQDBLegacy.h"

using namespace spqopt;

// sequential i/o bandwidth
const CDouble CCostModelParamsSPQDBLegacy::DSeqIOBandwidthVal = 1024.0;

// random i/o bandwidth
const CDouble CCostModelParamsSPQDBLegacy::DRandomIOBandwidthVal = 30.0;

// tuple processing bandwidth
const CDouble CCostModelParamsSPQDBLegacy::DTupProcBandwidthVal = 512.0;

// tuple update bandwidth
const CDouble CCostModelParamsSPQDBLegacy::DTupUpdateBandwidthVal = 256.0;

// network bandwidth
const CDouble CCostModelParamsSPQDBLegacy::DNetBandwidthVal = 1024.0;

// number of segments
const CDouble CCostModelParamsSPQDBLegacy::DSegmentsVal = 4.0;

// nlj outer factor
const CDouble CCostModelParamsSPQDBLegacy::DNLJOuterFactorVal = 1024.0;

// nlj factor
const CDouble CCostModelParamsSPQDBLegacy::DNLJFactorVal = 1.0;

// hj factor
const CDouble CCostModelParamsSPQDBLegacy::DHJFactorVal = 2.5;

// hash building factor
const CDouble CCostModelParamsSPQDBLegacy::DHashFactorVal = 2.0;

// default cost
const CDouble CCostModelParamsSPQDBLegacy::DDefaultCostVal = 100.0;

#include "spqos/base.h"
#include "spqos/string/CWStringConst.h"


#define SPQOPT_COSTPARAM_NAME_MAX_LENGTH 80

// parameter names in the same order of param enumeration
const CHAR rgszCostParamNames[CCostModelParamsSPQDBLegacy::EcpSentinel]
							 [SPQOPT_COSTPARAM_NAME_MAX_LENGTH] = {
								 "SeqIOBandwidth",	 "RandomIOBandwidth",
								 "TupProcBandwidth", "TupUpdateBandwidth",
								 "NetworkBandwidth", "Segments",
								 "NLJOuterFactor",	 "NLJFactor",
								 "HJFactor",		 "HashFactor",
								 "DefaultCost",
};

//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::CCostModelParamsSPQDBLegacy
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDBLegacy::CCostModelParamsSPQDBLegacy(CMemoryPool *mp)
	: m_mp(mp)
{
	SPQOS_ASSERT(NULL != mp);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		m_rspqcp[ul] = NULL;
	}

	// populate param array with default param values
	m_rspqcp[EcpSeqIOBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpSeqIOBandwidth, DSeqIOBandwidthVal,
				   DSeqIOBandwidthVal - 128.0, DSeqIOBandwidthVal + 128.0);
	m_rspqcp[EcpRandomIOBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpRandomIOBandwidth, DRandomIOBandwidthVal,
				   DRandomIOBandwidthVal - 8.0, DRandomIOBandwidthVal + 8.0);
	m_rspqcp[EcpTupProcBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpTupProcBandwidth, DTupProcBandwidthVal,
				   DTupProcBandwidthVal - 32.0, DTupProcBandwidthVal + 32.0);
	m_rspqcp[EcpTupUpdateBandwith] = SPQOS_NEW(mp) SCostParam(
		EcpTupUpdateBandwith, DTupUpdateBandwidthVal,
		DTupUpdateBandwidthVal - 32.0, DTupUpdateBandwidthVal + 32.0);
	m_rspqcp[EcpNetBandwidth] = SPQOS_NEW(mp)
		SCostParam(EcpNetBandwidth, DNetBandwidthVal, DNetBandwidthVal - 128.0,
				   DNetBandwidthVal + 128.0);
	m_rspqcp[EcpSegments] = SPQOS_NEW(mp) SCostParam(
		EcpSegments, DSegmentsVal, DSegmentsVal - 2.0, DSegmentsVal + 2.0);
	m_rspqcp[EcpNLJOuterFactor] = SPQOS_NEW(mp)
		SCostParam(EcpNLJOuterFactor, DNLJOuterFactorVal,
				   DNLJOuterFactorVal - 128.0, DNLJOuterFactorVal + 128.0);
	m_rspqcp[EcpNLJFactor] = SPQOS_NEW(mp) SCostParam(
		EcpNLJFactor, DNLJFactorVal, DNLJFactorVal - 0.5, DNLJFactorVal + 0.5);
	m_rspqcp[EcpHJFactor] = SPQOS_NEW(mp) SCostParam(
		EcpHJFactor, DHJFactorVal, DHJFactorVal - 1.0, DHJFactorVal + 1.0);
	m_rspqcp[EcpHashFactor] =
		SPQOS_NEW(mp) SCostParam(EcpHashFactor, DHashFactorVal,
								DHashFactorVal - 1.0, DHashFactorVal + 1.0);
	m_rspqcp[EcpDefaultCost] =
		SPQOS_NEW(mp) SCostParam(EcpDefaultCost, DDefaultCostVal,
								DDefaultCostVal - 32.0, DDefaultCostVal + 32.0);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::~CCostModelParamsSPQDBLegacy
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDBLegacy::~CCostModelParamsSPQDBLegacy()
{
	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		SPQOS_DELETE(m_rspqcp[ul]);
		m_rspqcp[ul] = NULL;
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::PcpLookup
//
//	@doc:
//		Lookup param by id;
//
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDBLegacy::SCostParam *
CCostModelParamsSPQDBLegacy::PcpLookup(ULONG id) const
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);

	return m_rspqcp[ecp];
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::PcpLookup
//
//	@doc:
//		Lookup param by name;
//		return NULL if name is not recognized
//
//---------------------------------------------------------------------------
CCostModelParamsSPQDBLegacy::SCostParam *
CCostModelParamsSPQDBLegacy::PcpLookup(const CHAR *szName) const
{
	SPQOS_ASSERT(NULL != szName);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		if (0 == clib::Strcmp(szName, rgszCostParamNames[ul]))
		{
			return PcpLookup((ECostParam) ul);
		}
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::SetParam
//
//	@doc:
//		Set param by id
//
//---------------------------------------------------------------------------
void
CCostModelParamsSPQDBLegacy::SetParam(ULONG id, CDouble dVal,
									 CDouble dLowerBound, CDouble dUpperBound)
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);

	SPQOS_DELETE(m_rspqcp[ecp]);
	m_rspqcp[ecp] = NULL;
	m_rspqcp[ecp] =
		SPQOS_NEW(m_mp) SCostParam(ecp, dVal, dLowerBound, dUpperBound);
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::SetParam
//
//	@doc:
//		Set param by name
//
//---------------------------------------------------------------------------
void
CCostModelParamsSPQDBLegacy::SetParam(const CHAR *szName, CDouble dVal,
									 CDouble dLowerBound, CDouble dUpperBound)
{
	SPQOS_ASSERT(NULL != szName);

	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		if (0 == clib::Strcmp(szName, rgszCostParamNames[ul]))
		{
			SPQOS_DELETE(m_rspqcp[ul]);
			m_rspqcp[ul] = NULL;
			m_rspqcp[ul] =
				SPQOS_NEW(m_mp) SCostParam(ul, dVal, dLowerBound, dUpperBound);

			return;
		}
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CCostModelParamsSPQDBLegacy::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CCostModelParamsSPQDBLegacy::OsPrint(IOstream &os) const
{
	for (ULONG ul = 0; ul < EcpSentinel; ul++)
	{
		SCostParam *pcp = PcpLookup((ECostParam) ul);
		os << rgszCostParamNames[ul] << " : " << pcp->Get() << "  ["
		   << pcp->GetLowerBoundVal() << "," << pcp->GetUpperBoundVal() << "]"
		   << std::endl;
	}
	return os;
}

BOOL
CCostModelParamsSPQDBLegacy::Equals(ICostModelParams *pcm) const
{
	CCostModelParamsSPQDBLegacy *pcmgOther =
		dynamic_cast<CCostModelParamsSPQDBLegacy *>(pcm);
	if (NULL == pcmgOther)
		return false;

	for (ULONG ul = 0U; ul < SPQOS_ARRAY_SIZE(m_rspqcp); ul++)
	{
		if (!m_rspqcp[ul]->Equals(pcmgOther->m_rspqcp[ul]))
			return false;
	}

	return true;
}

const CHAR *
CCostModelParamsSPQDBLegacy::SzNameLookup(ULONG id) const
{
	ECostParam ecp = (ECostParam) id;
	SPQOS_ASSERT(EcpSentinel > ecp);
	return rgszCostParamNames[ecp];
}


// EOF
