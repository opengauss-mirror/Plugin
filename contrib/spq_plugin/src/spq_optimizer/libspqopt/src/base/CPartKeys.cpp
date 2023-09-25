//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CPartKeys.cpp
//
//	@doc:
//		Implementation of partitioning keys
//---------------------------------------------------------------------------

#include "spqopt/base/CPartKeys.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CUtils.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CPartKeys);

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::CPartKeys
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CPartKeys::CPartKeys(CColRef2dArray *pdrspqdrspqcr) : m_pdrspqdrspqcr(pdrspqdrspqcr)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcr);
	m_num_of_part_levels = pdrspqdrspqcr->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::~CPartKeys
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CPartKeys::~CPartKeys()
{
	m_pdrspqdrspqcr->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::PcrKey
//
//	@doc:
//		Return key at a given level
//
//---------------------------------------------------------------------------
CColRef *
CPartKeys::PcrKey(ULONG ulLevel) const
{
	SPQOS_ASSERT(ulLevel < m_num_of_part_levels);
	CColRefArray *colref_array = (*m_pdrspqdrspqcr)[ulLevel];
	return (*colref_array)[0];
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::FOverlap
//
//	@doc:
//		Check whether the key columns overlap the given column
//
//---------------------------------------------------------------------------
BOOL
CPartKeys::FOverlap(CColRefSet *pcrs) const
{
	for (ULONG ul = 0; ul < m_num_of_part_levels; ul++)
	{
		CColRef *colref = PcrKey(ul);
		if (pcrs->FMember(colref))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::PpartkeysCopy
//
//	@doc:
//		Copy part key into the given memory pool
//
//---------------------------------------------------------------------------
CPartKeys *
CPartKeys::PpartkeysCopy(CMemoryPool *mp)
{
	CColRef2dArray *pdrspqdrspqcrCopy = SPQOS_NEW(mp) CColRef2dArray(mp);

	const ULONG length = m_pdrspqdrspqcr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefArray *colref_array = (*m_pdrspqdrspqcr)[ul];
		CColRefArray *pdrspqcrCopy = SPQOS_NEW(mp) CColRefArray(mp);
		const ULONG num_cols = colref_array->Size();
		for (ULONG ulCol = 0; ulCol < num_cols; ulCol++)
		{
			pdrspqcrCopy->Append((*colref_array)[ulCol]);
		}
		pdrspqdrspqcrCopy->Append(pdrspqcrCopy);
	}

	return SPQOS_NEW(mp) CPartKeys(pdrspqdrspqcrCopy);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::PdrspqpartkeysCopy
//
//	@doc:
//		Copy array of part keys into given memory pool
//
//---------------------------------------------------------------------------
CPartKeysArray *
CPartKeys::PdrspqpartkeysCopy(CMemoryPool *mp,
							 const CPartKeysArray *pdrspqpartkeys)
{
	SPQOS_ASSERT(NULL != pdrspqpartkeys);

	CPartKeysArray *pdrspqpartkeysCopy = SPQOS_NEW(mp) CPartKeysArray(mp);
	const ULONG length = pdrspqpartkeys->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		pdrspqpartkeysCopy->Append((*pdrspqpartkeys)[ul]->PpartkeysCopy(mp));
	}
	return pdrspqpartkeysCopy;
}


//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::PpartkeysRemap
//
//	@doc:
//		Create a new PartKeys object from the current one by remapping the
//		keys using the given hashmap
//
//---------------------------------------------------------------------------
CPartKeys *
CPartKeys::PpartkeysRemap(CMemoryPool *mp,
						  UlongToColRefMap *colref_mapping) const
{
	SPQOS_ASSERT(NULL != colref_mapping);
	CColRef2dArray *pdrspqdrspqcr = SPQOS_NEW(mp) CColRef2dArray(mp);

	for (ULONG ul = 0; ul < m_num_of_part_levels; ul++)
	{
		CColRef *colref =
			CUtils::PcrRemap(PcrKey(ul), colref_mapping, false /*must_exist*/);

		CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
		colref_array->Append(colref);

		pdrspqdrspqcr->Append(colref_array);
	}

	return SPQOS_NEW(mp) CPartKeys(pdrspqdrspqcr);
}

//---------------------------------------------------------------------------
//	@function:
//		CPartKeys::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPartKeys::OsPrint(IOstream &os) const
{
	os << "(";
	for (ULONG ul = 0; ul < m_num_of_part_levels; ul++)
	{
		CColRef *colref = PcrKey(ul);
		os << *colref;

		// separator
		os << (ul == m_num_of_part_levels - 1 ? "" : ", ");
	}

	os << ")";

	return os;
}

// EOF
