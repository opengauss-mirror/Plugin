//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CCTEReq.cpp
//
//	@doc:
//		Implementation of CTE requirements
//---------------------------------------------------------------------------

#include "spqopt/base/CCTEReq.h"

#include "spqos/base.h"

#include "spqopt/base/COptCtxt.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CCTEReq);
FORCE_GENERATE_DBGSTR(CCTEReq::CCTEReqEntry);

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReqEntry::CCTEReqEntry
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCTEReq::CCTEReqEntry::CCTEReqEntry(ULONG id, CCTEMap::ECteType ect,
									BOOL fRequired, CDrvdPropPlan *pdpplan)
	: m_id(id), m_ect(ect), m_fRequired(fRequired), m_pdpplan(pdpplan)
{
	SPQOS_ASSERT(CCTEMap::EctSentinel > ect);
	SPQOS_ASSERT_IMP(NULL == pdpplan, CCTEMap::EctProducer == ect);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReqEntry::~CCTEReqEntry
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCTEReq::CCTEReqEntry::~CCTEReqEntry()
{
	CRefCount::SafeRelease(m_pdpplan);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReqEntry::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CCTEReq::CCTEReqEntry::HashValue() const
{
	ULONG ulHash =
		spqos::CombineHashes(spqos::HashValue<ULONG>(&m_id),
							spqos::HashValue<CCTEMap::ECteType>(&m_ect));
	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue<BOOL>(&m_fRequired));

	if (NULL != m_pdpplan)
	{
		ulHash = spqos::CombineHashes(ulHash, m_pdpplan->HashValue());
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReqEntry::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CCTEReq::CCTEReqEntry::Equals(CCTEReqEntry *pcre) const
{
	SPQOS_ASSERT(NULL != pcre);
	if (m_id != pcre->Id() || m_ect != pcre->Ect() ||
		m_fRequired != pcre->FRequired())
	{
		return false;
	}

	CDrvdPropPlan *pdpplan = pcre->PdpplanProducer();
	if (NULL == m_pdpplan && NULL == pdpplan)
	{
		return true;
	}

	if (NULL != m_pdpplan && NULL != pdpplan)
	{
		return m_pdpplan->Equals(pdpplan);
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReqEntry::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CCTEReq::CCTEReqEntry::OsPrint(IOstream &os) const
{
	os << m_id << (CCTEMap::EctProducer == m_ect ? ":p" : ":c")
	   << (m_fRequired ? " " : "(opt) ");

	if (NULL != m_pdpplan)
	{
		os << "Plan Props: " << *m_pdpplan;
	}
	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::CCTEReq
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CCTEReq::CCTEReq(CMemoryPool *mp)
	: m_mp(mp), m_phmcter(NULL), m_pdrspqulRequired(NULL)
{
	SPQOS_ASSERT(NULL != mp);

	m_phmcter = SPQOS_NEW(m_mp) UlongToCTEReqEntryMap(m_mp);
	m_pdrspqulRequired = SPQOS_NEW(m_mp) ULongPtrArray(m_mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::~CCTEReq
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CCTEReq::~CCTEReq()
{
	m_phmcter->Release();
	m_pdrspqulRequired->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::Insert
//
//	@doc:
//		Insert a new map entry. No entry with the same id can already exist
//
//---------------------------------------------------------------------------
void
CCTEReq::Insert(ULONG ulCteId, CCTEMap::ECteType ect, BOOL fRequired,
				CDrvdPropPlan *pdpplan)
{
	SPQOS_ASSERT(CCTEMap::EctSentinel > ect);
	CCTEReqEntry *pcre =
		SPQOS_NEW(m_mp) CCTEReqEntry(ulCteId, ect, fRequired, pdpplan);
#ifdef SPQOS_DEBUG
	BOOL fSuccess =
#endif	// SPQOS_DEBUG
		m_phmcter->Insert(SPQOS_NEW(m_mp) ULONG(ulCteId), pcre);
	SPQOS_ASSERT(fSuccess);
	if (fRequired)
	{
		m_pdrspqulRequired->Append(SPQOS_NEW(m_mp) ULONG(ulCteId));
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::InsertConsumer
//
//	@doc:
//		Insert a new consumer entry. with the given id. The plan properties are
//		taken from the given context
//
//---------------------------------------------------------------------------
void
CCTEReq::InsertConsumer(ULONG id, CDrvdPropArray *pdrspqdpCtxt)
{
	ULONG ulProducerId = spqos::ulong_max;
	CDrvdPropPlan *pdpplan = CDrvdPropPlan::Pdpplan((*pdrspqdpCtxt)[0])
								 ->GetCostModel()
								 ->PdpplanProducer(&ulProducerId);
	SPQOS_ASSERT(NULL != pdpplan);
	SPQOS_ASSERT(ulProducerId == id &&
				"unexpected CTE producer plan properties");

	pdpplan->AddRef();
	Insert(id, CCTEMap::EctConsumer, true /*fRequired*/, pdpplan);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::PcreLookup
//
//	@doc:
//		Lookup info for given cte id
//
//---------------------------------------------------------------------------
CCTEReq::CCTEReqEntry *
CCTEReq::PcreLookup(ULONG ulCteId) const
{
	return m_phmcter->Find(&ulCteId);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::FSubset
//
//	@doc:
//		Check if the current requirement is a subset of the given one
//
//---------------------------------------------------------------------------
BOOL
CCTEReq::FSubset(const CCTEReq *pcter) const
{
	SPQOS_ASSERT(NULL != pcter);

	// compare number of entries first
	if (m_phmcter->Size() > pcter->m_phmcter->Size())
	{
		return false;
	}

	if (0 == m_phmcter->Size())
	{
		// empty subset
		return true;
	}

	// iterate over map entries
	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance())
	{
		const CCTEReqEntry *pcre = hmcri.Value();
		CCTEReqEntry *pcreOther = pcter->PcreLookup(pcre->Id());
		if (NULL == pcreOther || !pcre->Equals(pcreOther))
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::FContainsRequirement
//
//	@doc:
//		Check if the given CTE is in the requirements
//
//---------------------------------------------------------------------------
BOOL
CCTEReq::FContainsRequirement(const ULONG id, const CCTEMap::ECteType ect) const
{
	CCTEReqEntry *pcre = PcreLookup(id);
	return (NULL != pcre && pcre->Ect() == ect);
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::Ect
//
//	@doc:
//		Return the CTE type associated with the given ID in the requirements
//
//---------------------------------------------------------------------------
CCTEMap::ECteType
CCTEReq::Ect(const ULONG id) const
{
	CCTEReqEntry *pcre = PcreLookup(id);
	SPQOS_ASSERT(NULL != pcre);
	return pcre->Ect();
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CCTEReq::HashValue() const
{
	ULONG ulHash = 0;

	// how many map entries to use for hash computation
	const ULONG ulMaxEntries = 5;
	ULONG ul = 0;

	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance() && ul < ulMaxEntries)
	{
		const CCTEReqEntry *pcre = hmcri.Value();
		ulHash = spqos::CombineHashes(ulHash, pcre->HashValue());
		ul++;
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::PcterUnresolved
//
//	@doc:
//		Unresolved CTE requirements given a derived CTE map
//
//---------------------------------------------------------------------------
CCTEReq *
CCTEReq::PcterUnresolved(CMemoryPool *mp, CCTEMap *pcm)
{
	SPQOS_ASSERT(NULL != pcm);
	CCTEReq *pcterUnresolved = SPQOS_NEW(mp) CCTEReq(mp);

	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance())
	{
		// if a cte is marked as required and it is not found in the given map
		// then keep it as required, else make it optional
		const CCTEReqEntry *pcre = hmcri.Value();
		ULONG id = pcre->Id();
		BOOL fRequired =
			pcre->FRequired() && CCTEMap::EctSentinel == pcm->Ect(id);
		CDrvdPropPlan *pdpplan = pcre->PdpplanProducer();
		if (NULL != pdpplan)
		{
			pdpplan->AddRef();
		}

		pcterUnresolved->Insert(id, pcre->Ect(), fRequired, pdpplan);
	}

	return pcterUnresolved;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::PcterUnresolvedSequence
//
//	@doc:
//		Unresolved CTE requirements given a derived CTE map for a sequence
//		operator
//
//---------------------------------------------------------------------------
CCTEReq *
CCTEReq::PcterUnresolvedSequence(
	CMemoryPool *mp, CCTEMap *pcm,
	CDrvdPropArray *
		pdrspqdpCtxt	 // context contains derived plan properties of producer tree
)
{
	SPQOS_ASSERT(NULL != pcm);
	CCTEReq *pcterUnresolved = SPQOS_NEW(mp) CCTEReq(mp);

	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance())
	{
		const CCTEReqEntry *pcre = hmcri.Value();

		ULONG id = pcre->Id();
		CCTEMap::ECteType ect = pcre->Ect();
		BOOL fRequired = pcre->FRequired();

		CCTEMap::ECteType ectDrvd = pcm->Ect(id);
		if (fRequired && CCTEMap::EctSentinel != ectDrvd)
		{
			SPQOS_ASSERT(CCTEMap::EctConsumer == ect);
			SPQOS_ASSERT(CCTEMap::EctConsumer == ectDrvd);
			// already found, so mark it as optional
			CDrvdPropPlan *pdpplan = pcre->PdpplanProducer();
			SPQOS_ASSERT(NULL != pdpplan);
			pdpplan->AddRef();
			pcterUnresolved->Insert(id, ect, false /*fReqiored*/, pdpplan);
		}
		else if (!fRequired && CCTEMap::EctProducer == ect &&
				 CCTEMap::EctSentinel != ectDrvd)
		{
			SPQOS_ASSERT(CCTEMap::EctProducer == ectDrvd);

			// found a producer. require the corresponding consumer and
			// extract producer plan properties from passed context
			pcterUnresolved->InsertConsumer(id, pdrspqdpCtxt);
		}
		else
		{
			// either required and not found yet, or optional
			// in both cases, pass it down as is
			CDrvdPropPlan *pdpplan = pcre->PdpplanProducer();
			SPQOS_ASSERT_IMP(NULL == pdpplan, CCTEMap::EctProducer == ect);
			if (NULL != pdpplan)
			{
				pdpplan->AddRef();
			}
			pcterUnresolved->Insert(id, ect, fRequired, pdpplan);
		}
	}

	// if something is in pcm and not in the requirments, it has to be a producer
	// in which case, add the corresponding consumer as unresolved
	ULongPtrArray *pdrspqulProducers = pcm->PdrspqulAdditionalProducers(mp, this);
	const ULONG length = pdrspqulProducers->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		ULONG *pulId = (*pdrspqulProducers)[ul];
		pcterUnresolved->InsertConsumer(*pulId, pdrspqdpCtxt);
	}
	pdrspqulProducers->Release();

	return pcterUnresolved;
}

//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::PcterAllOptional
//
//	@doc:
//		Create a copy of the current requirement where all the entries are marked optional
//
//---------------------------------------------------------------------------
CCTEReq *
CCTEReq::PcterAllOptional(CMemoryPool *mp)
{
	CCTEReq *pcter = SPQOS_NEW(mp) CCTEReq(mp);

	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance())
	{
		const CCTEReqEntry *pcre = hmcri.Value();
		CDrvdPropPlan *pdpplan = pcre->PdpplanProducer();
		if (NULL != pdpplan)
		{
			pdpplan->AddRef();
		}
		pcter->Insert(pcre->Id(), pcre->Ect(), false /*fRequired*/, pdpplan);
	}

	return pcter;
}


//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::Pdpplan
//
//	@doc:
//		Lookup plan properties for given cte id
//
//---------------------------------------------------------------------------
CDrvdPropPlan *
CCTEReq::Pdpplan(ULONG ulCteId) const
{
	const CCTEReqEntry *pcre = PcreLookup(ulCteId);
	if (NULL != pcre)
	{
		return pcre->PdpplanProducer();
	}

	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CCTEReq::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CCTEReq::OsPrint(IOstream &os) const
{
	UlongToCTEReqEntryMapIter hmcri(m_phmcter);
	while (hmcri.Advance())
	{
		CCTEReqEntry *pcre = const_cast<CCTEReqEntry *>(hmcri.Value());
		pcre->OsPrint(os);
	}

	return os;
}

namespace spqopt
{
IOstream &
operator<<(IOstream &os, CCTEReq &cter)
{
	return cter.OsPrint(os);
}

}  // namespace spqopt

// EOF
