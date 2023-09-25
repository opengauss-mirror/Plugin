//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 Greenplum, Inc.
//
//	@filename:
//		CFunctionalDependency.cpp
//
//	@doc:
//		Implementation of functional dependency
//---------------------------------------------------------------------------

#include "spqopt/base/CFunctionalDependency.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"


using namespace spqopt;

FORCE_GENERATE_DBGSTR(CFunctionalDependency);

//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::CFunctionalDependency
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CFunctionalDependency::CFunctionalDependency(CColRefSet *pcrsKey,
											 CColRefSet *pcrsDetermined)
	: m_pcrsKey(pcrsKey), m_pcrsDetermined(pcrsDetermined)
{
	SPQOS_ASSERT(0 < pcrsKey->Size());
	SPQOS_ASSERT(0 < m_pcrsDetermined->Size());
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::~CFunctionalDependency
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CFunctionalDependency::~CFunctionalDependency()
{
	m_pcrsKey->Release();
	m_pcrsDetermined->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::FIncluded
//
//	@doc:
//		Determine if all FD columns are included in the given column set
//
//---------------------------------------------------------------------------
BOOL
CFunctionalDependency::FIncluded(CColRefSet *pcrs) const
{
	return pcrs->ContainsAll(m_pcrsKey) && pcrs->ContainsAll(m_pcrsDetermined);
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CFunctionalDependency::HashValue() const
{
	return spqos::CombineHashes(m_pcrsKey->HashValue(),
							   m_pcrsDetermined->HashValue());
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CFunctionalDependency::Equals(const CFunctionalDependency *pfd) const
{
	if (NULL == pfd)
	{
		return false;
	}

	return m_pcrsKey->Equals(pfd->PcrsKey()) &&
		   m_pcrsDetermined->Equals(pfd->PcrsDetermined());
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::OsPrint
//
//	@doc:
//		Print function
//
//---------------------------------------------------------------------------
IOstream &
CFunctionalDependency::OsPrint(IOstream &os) const
{
	os << "(" << *m_pcrsKey << ")";
	os << " --> (" << *m_pcrsDetermined << ")";
	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CFunctionalDependency::HashValue(const CFunctionalDependencyArray *pdrspqfd)
{
	ULONG ulHash = 0;
	if (NULL != pdrspqfd)
	{
		const ULONG size = pdrspqfd->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			ulHash = spqos::CombineHashes(ulHash, (*pdrspqfd)[ul]->HashValue());
		}
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::Equals
//
//	@doc:
//		Equality function
//
//---------------------------------------------------------------------------
BOOL
CFunctionalDependency::Equals(const CFunctionalDependencyArray *pdrspqfdFst,
							  const CFunctionalDependencyArray *pdrspqfdSnd)
{
	if (NULL == pdrspqfdFst && NULL == pdrspqfdSnd)
		return true; /* both empty */

	if (NULL == pdrspqfdFst || NULL == pdrspqfdSnd)
		return false; /* one is empty, the other is not */

	const ULONG ulLenFst = pdrspqfdFst->Size();
	const ULONG ulLenSnd = pdrspqfdSnd->Size();

	if (ulLenFst != ulLenSnd)
		return false;

	BOOL fEqual = true;
	for (ULONG ulFst = 0; fEqual && ulFst < ulLenFst; ulFst++)
	{
		const CFunctionalDependency *pfdFst = (*pdrspqfdFst)[ulFst];
		BOOL fMatch = false;
		for (ULONG ulSnd = 0; !fMatch && ulSnd < ulLenSnd; ulSnd++)
		{
			const CFunctionalDependency *pfdSnd = (*pdrspqfdSnd)[ulSnd];
			fMatch = pfdFst->Equals(pfdSnd);
		}
		fEqual = fMatch;
	}

	return fEqual;
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::PcrsKeys
//
//	@doc:
//		Create a set of all keys
//
//---------------------------------------------------------------------------
CColRefSet *
CFunctionalDependency::PcrsKeys(CMemoryPool *mp,
								const CFunctionalDependencyArray *pdrspqfd)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	if (pdrspqfd != NULL)
	{
		const ULONG size = pdrspqfd->Size();
		for (ULONG ul = 0; ul < size; ul++)
		{
			pcrs->Include((*pdrspqfd)[ul]->PcrsKey());
		}
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CFunctionalDependency::PdrspqcrKeys
//
//	@doc:
//		Create an array of all keys
//
//---------------------------------------------------------------------------
CColRefArray *
CFunctionalDependency::PdrspqcrKeys(CMemoryPool *mp,
								   const CFunctionalDependencyArray *pdrspqfd)
{
	CColRefSet *pcrs = PcrsKeys(mp, pdrspqfd);
	CColRefArray *colref_array = pcrs->Pdrspqcr(mp);
	pcrs->Release();

	return colref_array;
}


// EOF
