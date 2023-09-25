//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CKeyCollection.cpp
//
//	@doc:
//		Implementation of key collections
//---------------------------------------------------------------------------

#include "spqopt/base/CKeyCollection.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CKeyCollection);

//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::CKeyCollection
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CKeyCollection::CKeyCollection(CMemoryPool *mp) : m_mp(mp), m_pdrspqcrs(NULL)
{
	SPQOS_ASSERT(NULL != mp);

	m_pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::CKeyCollection
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CKeyCollection::CKeyCollection(CMemoryPool *mp, CColRefSet *pcrs)
	: m_mp(mp), m_pdrspqcrs(NULL)
{
	SPQOS_ASSERT(NULL != pcrs && 0 < pcrs->Size());

	m_pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	// we own the set
	Add(pcrs);
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::CKeyCollection
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CKeyCollection::CKeyCollection(CMemoryPool *mp, CColRefArray *colref_array)
	: m_mp(mp), m_pdrspqcrs(NULL)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != colref_array && 0 < colref_array->Size());

	m_pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref_array);
	Add(pcrs);

	// we own the array
	colref_array->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::~CKeyCollection
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CKeyCollection::~CKeyCollection()
{
	m_pdrspqcrs->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::Add
//
//	@doc:
//		Add key set to collection; takes ownership
//
//---------------------------------------------------------------------------
void
CKeyCollection::Add(CColRefSet *pcrs)
{
	SPQOS_ASSERT(!FKey(pcrs) && "no duplicates allowed");

	m_pdrspqcrs->Append(pcrs);
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::FKey
//
//	@doc:
//		Check if set constitutes key
//
//---------------------------------------------------------------------------
BOOL
CKeyCollection::FKey(const CColRefSet *pcrs,
					 BOOL fExactMatch  // true: match keys exactly,
									   //  false: match keys by inclusion
) const
{
	const ULONG ulSets = m_pdrspqcrs->Size();
	for (ULONG ul = 0; ul < ulSets; ul++)
	{
		if (fExactMatch)
		{
			// accept only exact matches
			if (pcrs->Equals((*m_pdrspqcrs)[ul]))
			{
				return true;
			}
		}
		else
		{
			// if given column set includes a key, then it is also a key
			if (pcrs->ContainsAll((*m_pdrspqcrs)[ul]))
			{
				return true;
			}
		}
	}

	return false;
}



//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::FKey
//
//	@doc:
//		Check if array constitutes key
//
//---------------------------------------------------------------------------
BOOL
CKeyCollection::FKey(CMemoryPool *mp, const CColRefArray *colref_array) const
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref_array);

	BOOL fKey = FKey(pcrs);
	pcrs->Release();

	return fKey;
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::PdrspqcrTrim
//
//	@doc:
//		Return first subsumed key as column array
//
//---------------------------------------------------------------------------
CColRefArray *
CKeyCollection::PdrspqcrTrim(CMemoryPool *mp,
							const CColRefArray *colref_array) const
{
	CColRefArray *pdrspqcrTrim = NULL;
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(colref_array);

	const ULONG ulSets = m_pdrspqcrs->Size();
	for (ULONG ul = 0; ul < ulSets; ul++)
	{
		CColRefSet *pcrsKey = (*m_pdrspqcrs)[ul];
		if (pcrs->ContainsAll(pcrsKey))
		{
			pdrspqcrTrim = pcrsKey->Pdrspqcr(mp);
			break;
		}
	}
	pcrs->Release();

	return pdrspqcrTrim;
}

//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::PdrspqcrKey
//
//	@doc:
//		Extract a key
//
//---------------------------------------------------------------------------
CColRefArray *
CKeyCollection::PdrspqcrKey(CMemoryPool *mp) const
{
	if (0 == m_pdrspqcrs->Size())
	{
		return NULL;
	}

	SPQOS_ASSERT(NULL != (*m_pdrspqcrs)[0]);

	CColRefArray *colref_array = (*m_pdrspqcrs)[0]->Pdrspqcr(mp);
	return colref_array;
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::PdrspqcrHashableKey
//
//	@doc:
//		Extract a hashable key
//
//---------------------------------------------------------------------------
CColRefArray *
CKeyCollection::PdrspqcrHashableKey(CMemoryPool *mp) const
{
	const ULONG ulSets = m_pdrspqcrs->Size();
	for (ULONG ul = 0; ul < ulSets; ul++)
	{
		CColRefArray *pdrspqcrKey = (*m_pdrspqcrs)[ul]->Pdrspqcr(mp);
		if (CUtils::IsHashable(pdrspqcrKey))
		{
			return pdrspqcrKey;
		}
		pdrspqcrKey->Release();
	}

	// no hashable key is found
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::PdrspqcrKey
//
//	@doc:
//		Extract the key at a position
//
//---------------------------------------------------------------------------
CColRefArray *
CKeyCollection::PdrspqcrKey(CMemoryPool *mp, ULONG ulIndex) const
{
	if (0 == m_pdrspqcrs->Size())
	{
		return NULL;
	}

	SPQOS_ASSERT(NULL != (*m_pdrspqcrs)[ulIndex]);

	CColRefArray *colref_array = (*m_pdrspqcrs)[ulIndex]->Pdrspqcr(mp);
	return colref_array;
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::PcrsKey
//
//	@doc:
//		Extract key at given position
//
//---------------------------------------------------------------------------
CColRefSet *
CKeyCollection::PcrsKey(CMemoryPool *mp, ULONG ulIndex) const
{
	if (0 == m_pdrspqcrs->Size())
	{
		return NULL;
	}

	SPQOS_ASSERT(NULL != (*m_pdrspqcrs)[ulIndex]);

	CColRefSet *pcrsKey = (*m_pdrspqcrs)[ulIndex];
	return SPQOS_NEW(mp) CColRefSet(mp, *pcrsKey);
}


//---------------------------------------------------------------------------
//	@function:
//		CKeyCollection::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CKeyCollection::OsPrint(IOstream &os) const
{
	os << " Keys: (";

	const ULONG ulSets = m_pdrspqcrs->Size();
	for (ULONG ul = 0; ul < ulSets; ul++)
	{
		if (0 < ul)
		{
			os << ", ";
		}

		SPQOS_ASSERT(NULL != (*m_pdrspqcrs)[ul]);
		os << "[" << (*(*m_pdrspqcrs)[ul]) << "]";
	}

	return os << ")";
}


// EOF
