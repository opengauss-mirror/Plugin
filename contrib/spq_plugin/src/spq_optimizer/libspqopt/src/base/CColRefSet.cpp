//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CColRefSet.cpp
//
//	@doc:
//		Implementation of column reference set based on bit sets
//---------------------------------------------------------------------------

#include "spqopt/base/CColRefSet.h"

#include "spqos/base.h"

#include "spqopt/base/CAutoOptCtxt.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColumnFactory.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CColRefSet);

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, ULONG ulSizeBits)
	: CBitSet(mp, ulSizeBits)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		copy ctor;
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, const CColRefSet &bs) : CBitSet(mp, bs)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::CColRefSet
//
//	@doc:
//		ctor, copy from col refs array
//
//---------------------------------------------------------------------------
CColRefSet::CColRefSet(CMemoryPool *mp, const CColRefArray *colref_array,
					   ULONG size)
	: CBitSet(mp, size)
{
	Include(colref_array);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::~CColRefSet
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CColRefSet::~CColRefSet()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FMember
//
//	@doc:
//		Check if given column ref is in the set
//
//---------------------------------------------------------------------------
BOOL
CColRefSet::FMember(const CColRef *colref) const
{
	return CBitSet::Get(colref->Id());
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::PcrAny
//
//	@doc:
//		Return random member
//
//---------------------------------------------------------------------------
CColRef *
CColRefSet::PcrAny() const
{
	// for now return the first column
	return PcrFirst();
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::PcrFirst
//
//	@doc:
//		Return first member
//
//---------------------------------------------------------------------------
CColRef *
CColRefSet::PcrFirst() const
{
	CColRefSetIter crsi(*this);
	if (crsi.Advance())
	{
		return crsi.Pcr();
	}

	SPQOS_ASSERT(0 == Size());
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include a constant column ref in set
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRef *colref)
{
	CBitSet::ExchangeSet(colref->Id());
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include column refs from an array
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRefArray *colref_array)
{
	ULONG length = colref_array->Size();
	for (ULONG i = 0; i < length; i++)
	{
		Include((*colref_array)[i]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Include
//
//	@doc:
//		Include a set of columns in bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Include(const CColRefSet *pcrs)
{
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		Include(crsi.Pcr());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove column from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRef *colref)
{
	CBitSet::ExchangeClear(colref->Id());
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove a set of columns from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRefSet *pcrs)
{
	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		Exclude(crsi.Pcr());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Exclude
//
//	@doc:
//		Remove an array of columns from bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Exclude(const CColRefArray *colref_array)
{
	for (ULONG i = 0; i < colref_array->Size(); i++)
	{
		Exclude((*colref_array)[i]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Replace
//
//	@doc:
//		Replace column with another column in bitset
//
//---------------------------------------------------------------------------
void
CColRefSet::Replace(const CColRef *pcrOut, const CColRef *pcrIn)
{
	if (FMember(pcrOut))
	{
		Exclude(pcrOut);
		Include(pcrIn);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Replace
//
//	@doc:
//		Replace an array of columns with another array of columns
//
//---------------------------------------------------------------------------
void
CColRefSet::Replace(const CColRefArray *pdrspqcrOut,
					const CColRefArray *pdrspqcrIn)
{
	const ULONG length = pdrspqcrOut->Size();
	SPQOS_ASSERT(length == pdrspqcrIn->Size());

	for (ULONG ul = 0; ul < length; ul++)
	{
		Replace((*pdrspqcrOut)[ul], (*pdrspqcrIn)[ul]);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Pdrspqcr
//
//	@doc:
//		Convert set into array
//
//---------------------------------------------------------------------------
CColRefArray *
CColRefSet::Pdrspqcr(CMemoryPool *mp) const
{
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);

	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		colref_array->Append(crsi.Pcr());
	}

	return colref_array;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::Phmicr
//
//	@doc:
//		Convert set into map
//
//---------------------------------------------------------------------------
IntToColRefMap *
CColRefSet::Phmicr(CMemoryPool *mp) const
{
	IntToColRefMap *phmicr = SPQOS_NEW(mp) IntToColRefMap(mp);

	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		phmicr->Insert(SPQOS_NEW(mp) INT(crsi.Pcr()->Id()), crsi.Pcr());
	}

	return phmicr;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::HashValue
//
//	@doc:
//		Compute hash value by combining hashes of components
//
//---------------------------------------------------------------------------
ULONG
CColRefSet::HashValue()
{
	ULONG size = this->Size();
	ULONG ulHash = spqos::HashValue<ULONG>(&size);

	// limit the number of columns used in hash computation
	ULONG length = std::min(size, (ULONG) 8);

	CColRefSetIter crsi(*this);
	for (ULONG i = 0; i < length; i++)
	{
		(void) crsi.Advance();
		ulHash =
			spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(crsi.Pcr()));
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::OsPrint
//
//	@doc:
//		Helper function to print a colref set
//
//---------------------------------------------------------------------------
IOstream &
CColRefSet::OsPrint(IOstream &os) const
{
	return OsPrint(os, spqos::ulong_max);
}

IOstream &
CColRefSet::OsPrint(IOstream &os, ULONG ulLenMax) const
{
	ULONG length = Size();
	ULONG ul = 0;

	CColRefSetIter crsi(*this);
	while (crsi.Advance() && ul < std::min(length, ulLenMax))
	{
		CColRef *colref = crsi.Pcr();
		colref->OsPrint(os);
		if (ul < length - 1)
		{
			os << ", ";
		}
		ul++;
	}

	if (ulLenMax < length)
	{
		os << "...";
	}

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::ExtractColIds
//
//	@doc:
//		Extract array of column ids from colrefset
//
//---------------------------------------------------------------------------
void
CColRefSet::ExtractColIds(CMemoryPool *mp, ULongPtrArray *colids) const
{
	CColRefSetIter crsi(*this);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		ULONG colid = colref->Id();
		colids->Append(SPQOS_NEW(mp) ULONG(colid));
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FContained
//
//	@doc:
//		Check if the current colrefset is a subset of any of the colrefsets in
//		the given array
//
//---------------------------------------------------------------------------
BOOL
CColRefSet::FContained(const CColRefSetArray *pdrspqcrs)
{
	SPQOS_ASSERT(NULL != pdrspqcrs);

	const ULONG length = pdrspqcrs->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		if ((*pdrspqcrs)[ul]->ContainsAll(this))
		{
			return true;
		}
	}

	return false;
}

BOOL
CColRefSet::FIntersects(const CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pcrs);
	CColRefSet *intersecting_colrefset = SPQOS_NEW(m_mp) CColRefSet(m_mp, *this);
	intersecting_colrefset->Intersection(pcrs);

	BOOL intersects = intersecting_colrefset->Size() > 0;
	intersecting_colrefset->Release();

	return intersects;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRefSet::FCovered
//
//	@doc:
//		Are the columns in the column reference set covered by the array of
//		column ref sets
//---------------------------------------------------------------------------
BOOL
CColRefSet::FCovered(CColRefSetArray *pdrspqcrs, CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pdrspqcrs);
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(0 < pdrspqcrs->Size());

	if (0 == pcrs->Size())
	{
		return false;
	}

	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		BOOL fFound = false;
		const ULONG length = pdrspqcrs->Size();
		for (ULONG ul = 0; ul < length && !fFound; ul++)
		{
			CColRefSet *pcrs = (*pdrspqcrs)[ul];
			if (pcrs->FMember(colref))
			{
				fFound = true;
			}
		}

		if (!fFound)
		{
			return false;
		}
	}

	return true;
}


// EOF
