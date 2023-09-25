//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		COrderSpec.cpp
//
//	@doc:
//		Specification of order property
//---------------------------------------------------------------------------

#include "spqopt/base/COrderSpec.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CPhysicalSort.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"
#endif	// SPQOS_DEBUG

using namespace spqopt;
using namespace spqmd;

// string encoding of null treatment
const CHAR rgszNullCode[][16] = {"Auto", "NULLsFirst", "NULLsLast"};
SPQOS_CPL_ASSERT(COrderSpec::EntSentinel == SPQOS_ARRAY_SIZE(rgszNullCode));


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::COrderExpression::COrderExpression
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
COrderSpec::COrderExpression::COrderExpression(spqmd::IMDId *mdid,
											   const CColRef *colref,
											   ENullTreatment ent)
	: m_mdid(mdid), m_pcr(colref), m_ent(ent)
{
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(mdid->IsValid());
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::COrderExpression::~COrderExpression
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
COrderSpec::COrderExpression::~COrderExpression()
{
	m_mdid->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::COrderExpression::Matches
//
//	@doc:
//		Check if order expression equal to given one;
//
//---------------------------------------------------------------------------
BOOL
COrderSpec::COrderExpression::Matches(const COrderExpression *poe) const
{
	SPQOS_ASSERT(NULL != poe);

	return poe->m_mdid->Equals(m_mdid) && poe->m_pcr == m_pcr &&
		   poe->m_ent == m_ent;
}


FORCE_GENERATE_DBGSTR(spqopt::COrderSpec::COrderExpression);

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::COrderExpression::OsPrint
//
//	@doc:
//		Print order expression
//
//---------------------------------------------------------------------------
IOstream &
COrderSpec::COrderExpression::OsPrint(IOstream &os) const
{
	os << "( ";
	m_mdid->OsPrint(os);
	os << ", ";
	m_pcr->OsPrint(os);
	os << ", " << rgszNullCode[m_ent] << " )";

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::COrderSpec
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
COrderSpec::COrderSpec(CMemoryPool *mp) : m_mp(mp), m_pdrspqoe(NULL)
{
	m_pdrspqoe = SPQOS_NEW(mp) COrderExpressionArray(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::~COrderSpec
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
COrderSpec::~COrderSpec()
{
	m_pdrspqoe->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::Append
//
//	@doc:
//		Append order expression;
//
//---------------------------------------------------------------------------
void
COrderSpec::Append(spqmd::IMDId *mdid, const CColRef *colref, ENullTreatment ent)
{
	COrderExpression *poe = SPQOS_NEW(m_mp) COrderExpression(mdid, colref, ent);
	m_pdrspqoe->Append(poe);
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::Matches
//
//	@doc:
//		Check for equality between order specs
//
//---------------------------------------------------------------------------
BOOL
COrderSpec::Matches(const COrderSpec *pos) const
{
	BOOL fMatch =
		m_pdrspqoe->Size() == pos->m_pdrspqoe->Size() && FSatisfies(pos);

	SPQOS_ASSERT_IMP(fMatch, pos->FSatisfies(this));

	return fMatch;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::FSatisfies
//
//	@doc:
//		Check if this order spec satisfies the given one
//
//---------------------------------------------------------------------------
BOOL
COrderSpec::FSatisfies(const COrderSpec *pos) const
{
	const ULONG arity = pos->m_pdrspqoe->Size();
	BOOL fSatisfies = (m_pdrspqoe->Size() >= arity);

	for (ULONG ul = 0; fSatisfies && ul < arity; ul++)
	{
		fSatisfies = (*m_pdrspqoe)[ul]->Matches((*(pos->m_pdrspqoe))[ul]);
	}

	return fSatisfies;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::AppendEnforcers
//
//	@doc:
//		Add required enforcers enforcers to dynamic array
//
//---------------------------------------------------------------------------
void
COrderSpec::AppendEnforcers(CMemoryPool *mp,
							CExpressionHandle &,  // exprhdl
							CReqdPropPlan *
#ifdef SPQOS_DEBUG
								prpp
#endif	// SPQOS_DEBUG
							,
							CExpressionArray *pdrspqexpr, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != prpp);
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(this == prpp->Peo()->PosRequired() &&
				"required plan properties don't match enforced order spec");

	AddRef();
	pexpr->AddRef();
	CExpression *pexprSort = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CPhysicalSort(mp, this), pexpr);
	pdrspqexpr->Append(pexprSort);
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
COrderSpec::HashValue() const
{
	ULONG ulHash = 0;
	ULONG arity = m_pdrspqoe->Size();

	for (ULONG ul = 0; ul < arity; ul++)
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		ulHash =
			spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(poe->Pcr()));
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::PosCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the order spec with remapped columns
//
//---------------------------------------------------------------------------
COrderSpec *
COrderSpec::PosCopyWithRemappedColumns(CMemoryPool *mp,
									   UlongToColRefMap *colref_mapping,
									   BOOL must_exist)
{
	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);

	const ULONG num_cols = m_pdrspqoe->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		IMDId *mdid = poe->GetMdIdSortOp();
		mdid->AddRef();

		const CColRef *colref = poe->Pcr();
		ULONG id = colref->Id();
		CColRef *pcrMapped = colref_mapping->Find(&id);
		if (NULL == pcrMapped)
		{
			if (must_exist)
			{
				CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
				// not found in hashmap, so create a new colref and add to hashmap
				pcrMapped = col_factory->PcrCopy(colref);

#ifdef SPQOS_DEBUG
				BOOL result =
#endif	// SPQOS_DEBUG
					colref_mapping->Insert(SPQOS_NEW(mp) ULONG(id), pcrMapped);
				SPQOS_ASSERT(result);
			}
			else
			{
				pcrMapped = const_cast<CColRef *>(colref);
			}
		}

		COrderSpec::ENullTreatment ent = poe->Ent();
		pos->Append(mdid, pcrMapped, ent);
	}

	return pos;
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::PosExcludeColumns
//
//	@doc:
//		Return a copy of the order spec after excluding the given columns
//
//---------------------------------------------------------------------------
COrderSpec *
COrderSpec::PosExcludeColumns(CMemoryPool *mp, CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pcrs);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);

	const ULONG num_cols = m_pdrspqoe->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		COrderExpression *poe = (*m_pdrspqoe)[ul];
		const CColRef *colref = poe->Pcr();

		if (pcrs->FMember(colref))
		{
			continue;
		}

		IMDId *mdid = poe->GetMdIdSortOp();
		mdid->AddRef();
		pos->Append(mdid, colref, poe->Ent());
	}

	return pos;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::ExtractCols
//
//	@doc:
//		Extract columns from order spec into the given column set
//
//---------------------------------------------------------------------------
void
COrderSpec::ExtractCols(CColRefSet *pcrs) const
{
	SPQOS_ASSERT(NULL != pcrs);

	const ULONG ulOrderExprs = m_pdrspqoe->Size();
	for (ULONG ul = 0; ul < ulOrderExprs; ul++)
	{
		pcrs->Include((*m_pdrspqoe)[ul]->Pcr());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::PcrsUsed
//
//	@doc:
//		Extract colref set from order components
//
//---------------------------------------------------------------------------
CColRefSet *
COrderSpec::PcrsUsed(CMemoryPool *mp) const
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	ExtractCols(pcrs);

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::GetColRefSet
//
//	@doc:
//		Extract colref set from order specs in the given array
//
//---------------------------------------------------------------------------
CColRefSet *
COrderSpec::GetColRefSet(CMemoryPool *mp, COrderSpecArray *pdrspqos)
{
	SPQOS_ASSERT(NULL != pdrspqos);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	const ULONG ulOrderSpecs = pdrspqos->Size();
	for (ULONG ulSpec = 0; ulSpec < ulOrderSpecs; ulSpec++)
	{
		COrderSpec *pos = (*pdrspqos)[ulSpec];
		pos->ExtractCols(pcrs);
	}

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::PdrspqosExclude
//
//	@doc:
//		Filter out array of order specs from order expressions using the
//		passed columns
//
//---------------------------------------------------------------------------
COrderSpecArray *
COrderSpec::PdrspqosExclude(CMemoryPool *mp, COrderSpecArray *pdrspqos,
						   CColRefSet *pcrsToExclude)
{
	SPQOS_ASSERT(NULL != pdrspqos);
	SPQOS_ASSERT(NULL != pcrsToExclude);

	if (0 == pcrsToExclude->Size())
	{
		// no columns to exclude
		pdrspqos->AddRef();
		return pdrspqos;
	}

	COrderSpecArray *pdrspqosNew = SPQOS_NEW(mp) COrderSpecArray(mp);
	const ULONG ulOrderSpecs = pdrspqos->Size();
	for (ULONG ulSpec = 0; ulSpec < ulOrderSpecs; ulSpec++)
	{
		COrderSpec *pos = (*pdrspqos)[ulSpec];
		COrderSpec *posNew = pos->PosExcludeColumns(mp, pcrsToExclude);
		pdrspqosNew->Append(posNew);
	}

	return pdrspqosNew;
}

//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::OsPrint
//
//	@doc:
//		Print order spec
//
//---------------------------------------------------------------------------
IOstream &
COrderSpec::OsPrint(IOstream &os) const
{
	const ULONG arity = m_pdrspqoe->Size();
	if (0 == arity)
	{
		os << "<empty>";
	}
	else
	{
		for (ULONG ul = 0; ul < arity; ul++)
		{
			(*m_pdrspqoe)[ul]->OsPrint(os) << " ";
		}
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::Equals
//
//	@doc:
//		 Matching function over order spec arrays
//
//---------------------------------------------------------------------------
BOOL
COrderSpec::Equals(const COrderSpecArray *pdrspqosFirst,
				   const COrderSpecArray *pdrspqosSecond)
{
	if (NULL == pdrspqosFirst || NULL == pdrspqosSecond)
	{
		return (NULL == pdrspqosFirst && NULL == pdrspqosSecond);
	}

	if (pdrspqosFirst->Size() != pdrspqosSecond->Size())
	{
		return false;
	}

	const ULONG size = pdrspqosFirst->Size();
	BOOL fMatch = true;
	for (ULONG ul = 0; fMatch && ul < size; ul++)
	{
		fMatch = (*pdrspqosFirst)[ul]->Matches((*pdrspqosSecond)[ul]);
	}

	return fMatch;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::HashValue
//
//	@doc:
//		 Combine hash values of a maximum number of entries
//
//---------------------------------------------------------------------------
ULONG
COrderSpec::HashValue(const COrderSpecArray *pdrspqos, ULONG ulMaxSize)
{
	SPQOS_ASSERT(NULL != pdrspqos);
	ULONG size = std::min(ulMaxSize, pdrspqos->Size());

	ULONG ulHash = 0;
	for (ULONG ul = 0; ul < size; ul++)
	{
		ulHash = spqos::CombineHashes(ulHash, (*pdrspqos)[ul]->HashValue());
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		COrderSpec::OsPrint
//
//	@doc:
//		 Print array of order spec objects
//
//---------------------------------------------------------------------------
IOstream &
COrderSpec::OsPrint(IOstream &os, const COrderSpecArray *pdrspqos)
{
	const ULONG size = pdrspqos->Size();
	os << "[";
	if (0 < size)
	{
		for (ULONG ul = 0; ul < size - 1; ul++)
		{
			(void) (*pdrspqos)[ul]->OsPrint(os);
			os << ", ";
		}

		(void) (*pdrspqos)[size - 1]->OsPrint(os);
	}

	return os << "]";
}


// EOF
