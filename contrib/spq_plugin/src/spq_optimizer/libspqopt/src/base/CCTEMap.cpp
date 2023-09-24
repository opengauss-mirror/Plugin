//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCTEMap.cpp
//
//	@doc:
//		Implementation of CTE map
//---------------------------------------------------------------------------

#include "spqopt/base/CCTEMap.h"

#include "spqos/base.h"

#include "spqopt/base/CCTEReq.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CCTEMap);
FORCE_GENERATE_DBGSTR(CCTEMap::CCTEMapEntry);

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::CCTEMap
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCTEMap::CCTEMap(CMemoryPool *mp) : m_mp(mp), m_phmcm(NULL)
{
	SPQOS_ASSERT(NULL != mp);

	m_phmcm = SPQOS_NEW(m_mp) UlongToCTEMapEntryMap(m_mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::~CCTEMap
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCTEMap::~CCTEMap()
{
	m_phmcm->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::Insert
//
//	@doc:
//		Insert a new map entry. No entry with the same id can already exist
//
//---------------------------------------------------------------------------
void
CCTEMap::Insert(ULONG ulCteId, ECteType ect, CDrvdPropPlan *pdpplan)
{
	SPQOS_ASSERT(EctSentinel > ect);

	if (NULL != pdpplan)
	{
		pdpplan->AddRef();
	}

	CCTEMapEntry *pcme = SPQOS_NEW(m_mp) CCTEMapEntry(ulCteId, ect, pdpplan);
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		m_phmcm->Insert(SPQOS_NEW(m_mp) ULONG(ulCteId), pcme);
	SPQOS_ASSERT(fSuccess);
}


//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::PdpplanProducer
//
//	@doc:
//		At any point in a query plan, derived plan properties must contain
//		at most one CTE producer;
//		this function returns plan properties of that producer if found, otherwise
//		return NULL;
//		the function also asserts that map has no other producer entries
//
//
//---------------------------------------------------------------------------
CDrvdPropPlan *
CCTEMap::PdpplanProducer(
	ULONG *
		pulId  // output: CTE producer Id, set to spqos::ulong_max if no producer found
) const
{
	SPQOS_ASSERT(NULL != pulId);

	CDrvdPropPlan *pdpplanProducer = NULL;
	*pulId = spqos::ulong_max;
	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (NULL == pdpplanProducer && hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		CCTEMap::ECteType ect = pcme->Ect();
		CDrvdPropPlan *pdpplan = pcme->Pdpplan();
		if (CCTEMap::EctProducer == ect)
		{
			SPQOS_ASSERT(NULL != pdpplan);
			pdpplanProducer = pdpplan;
			*pulId = pcme->Id();
		}
	}

#ifdef SPQOS_DEBUG
	while (hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		CCTEMap::ECteType ect = pcme->Ect();
		SPQOS_ASSERT(CCTEMap::EctConsumer == ect &&
					"CTE map has properties of more than one producer");
	}
#endif	// SPQOS_DEBUG

	return pdpplanProducer;
}


//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::AddUnresolved
//
//	@doc:
//		Helper to add entries found in the first map and are
//		unresolved based on the second map
//
//---------------------------------------------------------------------------
void
CCTEMap::AddUnresolved(const CCTEMap &cmFirst, const CCTEMap &cmSecond,
					   CCTEMap *pcmResult)
{
	SPQOS_ASSERT(NULL != pcmResult);
	// iterate on first map and lookup entries in second map
	UlongToCTEMapEntryMapIter hmcmi(cmFirst.m_phmcm);
	while (hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		ULONG id = pcme->Id();
		ECteType ectFirst = pcme->Ect();
		CDrvdPropPlan *pdpplanFirst = pcme->Pdpplan();

		if (NULL != pcmResult->PcmeLookup(id))
		{
			// skip entries already in the result map
			continue;
		}

		// check if entry exists in second map
		CCTEMapEntry *pcmeSecond = cmSecond.PcmeLookup(id);

		// if entry does not exist in second map, or exists with the same cte type
		// then it should be in the result
		if (NULL == pcmeSecond || ectFirst == pcmeSecond->Ect())
		{
			pcmResult->Insert(id, ectFirst, pdpplanFirst);
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::PcmeLookup
//
//	@doc:
//		Lookup info for given cte id
//
//---------------------------------------------------------------------------
CCTEMap::CCTEMapEntry *
CCTEMap::PcmeLookup(ULONG ulCteId) const
{
	return m_phmcm->Find(&ulCteId);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::FSubset
//
//	@doc:
//		Check if the current map is a subset of the given one
//
//---------------------------------------------------------------------------
BOOL
CCTEMap::FSubset(const CCTEMap *pcm) const
{
	SPQOS_ASSERT(NULL != pcm);

	if (m_phmcm->Size() > pcm->m_phmcm->Size())
	{
		return false;
	}

	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		CCTEMapEntry *pcmeOther = pcm->PcmeLookup(pcme->Id());
		if (NULL == pcmeOther || pcmeOther->Ect() != pcme->Ect())
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CCTEMap::HashValue() const
{
	ULONG ulHash = 0;

	// how many map entries to use for hash computation
	ULONG ulMaxEntries = 5;
	ULONG ul = 0;

	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (hmcmi.Advance() && ul < ulMaxEntries)
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		ulHash = spqos::CombineHashes(ulHash, pcme->HashValue());
		ul++;
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::Ect
//
//	@doc:
//		Return the CTE type associated with the given ID in the derived map
//
//---------------------------------------------------------------------------
CCTEMap::ECteType
CCTEMap::Ect(const ULONG id) const
{
	CCTEMapEntry *pcme = PcmeLookup(id);
	if (NULL == pcme)
	{
		return EctSentinel;
	}

	return pcme->Ect();
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::PcmCombine
//
//	@doc:
//		Combine the two given maps and return the result
//
//---------------------------------------------------------------------------
CCTEMap *
CCTEMap::PcmCombine(CMemoryPool *mp, const CCTEMap &cmFirst,
					const CCTEMap &cmSecond)
{
	CCTEMap *pcmResult = SPQOS_NEW(mp) CCTEMap(mp);

	// add entries from first map that are not resolvable based on second map
	AddUnresolved(cmFirst, cmSecond, pcmResult);

	// add entries from second map that are not resolvable based on first map
	AddUnresolved(cmSecond, cmFirst, pcmResult);

	return pcmResult;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::FSatisfies
//
//	@doc:
//		Check whether the current CTE map satisfies the given CTE requirements
//
//---------------------------------------------------------------------------
BOOL
CCTEMap::FSatisfies(const CCTEReq *pcter) const
{
	SPQOS_ASSERT(NULL != pcter);
	// every CTE marked as "Required" must be in the current map
	ULongPtrArray *pdrspqul = pcter->PdrspqulRequired();
	const ULONG ulReqd = pdrspqul->Size();
	for (ULONG ul = 0; ul < ulReqd; ul++)
	{
		ULONG *pulId = (*pdrspqul)[ul];
		ECteType ect = pcter->Ect(*pulId);

		CCTEMapEntry *pcme = this->PcmeLookup(*pulId);
		if (NULL == pcme || pcme->Ect() != ect)
		{
			return false;
		}
	}

	// every CTE consumer in the current map must be in the requirements (does not
	// matter whether it is marked as required or optional)
	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		ECteType ect = pcme->Ect();
		if (CCTEMap::EctConsumer == ect &&
			!pcter->FContainsRequirement(pcme->Id(), ect))
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::PdrspqulAdditionalProducers
//
//	@doc:
//		Return producer ids that are in this map but not in the given requirement
//
//---------------------------------------------------------------------------
ULongPtrArray *
CCTEMap::PdrspqulAdditionalProducers(CMemoryPool *mp, const CCTEReq *pcter) const
{
	SPQOS_ASSERT(NULL != pcter);
	ULongPtrArray *pdrspqul = SPQOS_NEW(mp) ULongPtrArray(mp);

	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (hmcmi.Advance())
	{
		const CCTEMapEntry *pcme = hmcmi.Value();
		ULONG id = pcme->Id();
		ECteType ect = pcme->Ect();

		if (CCTEMap::EctProducer == ect &&
			!pcter->FContainsRequirement(id, ect))
		{
			pdrspqul->Append(SPQOS_NEW(mp) ULONG(id));
		}
	}

	return pdrspqul;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEMap::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CCTEMap::OsPrint(IOstream &os) const
{
	UlongToCTEMapEntryMapIter hmcmi(m_phmcm);
	while (hmcmi.Advance())
	{
		CCTEMapEntry *pcme = const_cast<CCTEMapEntry *>(hmcmi.Value());
		pcme->OsPrint(os);
		os << " ";
	}

	return os;
}

namespace spqopt
{
IOstream &
operator<<(IOstream &os, CCTEMap &cm)
{
	return cm.OsPrint(os);
}

}  // namespace spqopt

// EOF
