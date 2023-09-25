//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 - 2012 EMC Corp.
//
//	@filename:
//		CLogicalSetOp.cpp
//
//	@doc:
//		Implementation of setops
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalSetOp.h"

#include "spqos/base.h"
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CConstraintNegation.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::CLogicalSetOp
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalSetOp::CLogicalSetOp(CMemoryPool *mp)
	: CLogical(mp),
	  m_pdrspqcrOutput(NULL),
	  m_pdrspqdrspqcrInput(NULL),
	  m_pcrsOutput(NULL),
	  m_pdrspqcrsInput(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::CLogicalSetOp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalSetOp::CLogicalSetOp(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
							 CColRef2dArray *pdrspqdrspqcrInput)
	: CLogical(mp),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_pdrspqdrspqcrInput(pdrspqdrspqcrInput),
	  m_pcrsOutput(NULL),
	  m_pdrspqcrsInput(NULL)
{
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	SPQOS_ASSERT(NULL != pdrspqdrspqcrInput);

	BuildColumnSets(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::CLogicalSetOp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalSetOp::CLogicalSetOp(CMemoryPool *mp, CColRefArray *pdrspqcrOutput,
							 CColRefArray *pdrspqcrLeft,
							 CColRefArray *pdrspqcrRight)
	: CLogical(mp), m_pdrspqcrOutput(pdrspqcrOutput), m_pdrspqdrspqcrInput(NULL)
{
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	SPQOS_ASSERT(NULL != pdrspqcrLeft);
	SPQOS_ASSERT(NULL != pdrspqcrRight);

	m_pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp, 2);

	m_pdrspqdrspqcrInput->Append(pdrspqcrLeft);
	m_pdrspqdrspqcrInput->Append(pdrspqcrRight);

	BuildColumnSets(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::~CLogicalSetOp
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalSetOp::~CLogicalSetOp()
{
	CRefCount::SafeRelease(m_pdrspqcrOutput);
	CRefCount::SafeRelease(m_pdrspqdrspqcrInput);
	CRefCount::SafeRelease(m_pcrsOutput);
	CRefCount::SafeRelease(m_pdrspqcrsInput);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::BuildColumnSets
//
//	@doc:
//		Build set representation of input/output columns for faster
//		set operations
//
//---------------------------------------------------------------------------
void
CLogicalSetOp::BuildColumnSets(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != m_pdrspqcrOutput);
	SPQOS_ASSERT(NULL != m_pdrspqdrspqcrInput);
	SPQOS_ASSERT(NULL == m_pcrsOutput);
	SPQOS_ASSERT(NULL == m_pdrspqcrsInput);

	m_pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);
	m_pdrspqcrsInput = SPQOS_NEW(mp) CColRefSetArray(mp);
	const ULONG ulChildren = m_pdrspqdrspqcrInput->Size();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		CColRefSet *pcrsInput =
			SPQOS_NEW(mp) CColRefSet(mp, (*m_pdrspqdrspqcrInput)[ul]);
		m_pdrspqcrsInput->Append(pcrsInput);

		m_pcrsLocalUsed->Include(pcrsInput);
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSetOp::DeriveOutputColumns(CMemoryPool *,  // mp
								   CExpressionHandle &
#ifdef SPQOS_DEBUG
									   exprhdl
#endif	// SPQOS_DEBUG
)
{
#ifdef SPQOS_DEBUG
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColRefSet *pcrsChildOutput = exprhdl.DeriveOutputColumns(ul);
		CColRefSet *pcrsInput = (*m_pdrspqcrsInput)[ul];
		SPQOS_ASSERT(pcrsChildOutput->ContainsAll(pcrsInput) &&
					"Unexpected outer references in SetOp input");
	}
#endif	// SPQOS_DEBUG

	m_pcrsOutput->AddRef();

	return m_pcrsOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalSetOp::DeriveKeyCollection(CMemoryPool *mp,
								   CExpressionHandle &	// exprhdl
) const
{
	// TODO: 3/3/2012 - ; we can do better by remapping the keys between
	// all children and check if they align

	// True set ops return sets, hence, all output columns are keys
	m_pdrspqcrOutput->AddRef();
	return SPQOS_NEW(mp) CKeyCollection(mp, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::DerivePartitionInfo
//
//	@doc:
//		Derive partition consumer info
//
//---------------------------------------------------------------------------
CPartInfo *
CLogicalSetOp::DerivePartitionInfo(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const
{
	const ULONG arity = exprhdl.Arity();
	SPQOS_ASSERT(0 < arity);

	// start with the part info of the first child
	CPartInfo *ppartinfo = exprhdl.DerivePartitionInfo(0);
	ppartinfo->AddRef();

	for (ULONG ul = 1; ul < arity; ul++)
	{
		CPartInfo *ppartinfoChild = exprhdl.DerivePartitionInfo(ul);
		SPQOS_ASSERT(NULL != ppartinfoChild);

		CColRefArray *pdrspqcrInput = (*m_pdrspqdrspqcrInput)[ul];
		SPQOS_ASSERT(pdrspqcrInput->Size() == m_pdrspqcrOutput->Size());

		CPartInfo *ppartinfoRemapped =
			ppartinfoChild->PpartinfoWithRemappedKeys(mp, pdrspqcrInput,
													  m_pdrspqcrOutput);
		CPartInfo *ppartinfoCombined =
			CPartInfo::PpartinfoCombine(mp, ppartinfo, ppartinfoRemapped);
		ppartinfoRemapped->Release();

		ppartinfo->Release();
		ppartinfo = ppartinfoCombined;
	}

	return ppartinfo;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalSetOp::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalSetOp *popSetOp = CLogicalSetOp::PopConvert(pop);
	CColRef2dArray *pdrspqdrspqcrInput = popSetOp->PdrspqdrspqcrInput();
	const ULONG arity = pdrspqdrspqcrInput->Size();

	if (arity != m_pdrspqdrspqcrInput->Size() ||
		!m_pdrspqcrOutput->Equals(popSetOp->PdrspqcrOutput()))
	{
		return false;
	}

	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!(*m_pdrspqdrspqcrInput)[ul]->Equals((*pdrspqdrspqcrInput)[ul]))
		{
			return false;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PdrspqcrsOutputEquivClasses
//
//	@doc:
//		Get output equivalence classes
//
//---------------------------------------------------------------------------
CColRefSetArray *
CLogicalSetOp::PdrspqcrsOutputEquivClasses(CMemoryPool *mp,
										  CExpressionHandle &exprhdl,
										  BOOL fIntersect) const
{
	const ULONG ulChildren = exprhdl.Arity();
	CColRefSetArray *pdrspqcrs = PdrspqcrsInputMapped(mp, exprhdl, 0 /*ulChild*/);

	for (ULONG ul = 1; ul < ulChildren; ul++)
	{
		CColRefSetArray *pdrspqcrsChild = PdrspqcrsInputMapped(mp, exprhdl, ul);
		CColRefSetArray *pdrspqcrsMerged = NULL;

		if (fIntersect)
		{
			// merge with the equivalence classes we have so far
			pdrspqcrsMerged =
				CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsChild);
		}
		else
		{
			// in case of a union, an equivalence class must be coming from all
			// children to be part of the output
			pdrspqcrsMerged = CUtils::PdrspqcrsIntersectEquivClasses(
				mp, pdrspqcrs, pdrspqcrsChild);
		}
		pdrspqcrsChild->Release();
		pdrspqcrs->Release();
		pdrspqcrs = pdrspqcrsMerged;
	}

	return pdrspqcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PdrspqcrsInputMapped
//
//	@doc:
//		Get equivalence classes from one input child, mapped to output columns
//
//---------------------------------------------------------------------------
CColRefSetArray *
CLogicalSetOp::PdrspqcrsInputMapped(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   ULONG ulChild) const
{
	CColRefSetArray *pdrspqcrsInput =
		exprhdl.DerivePropertyConstraint(ulChild)->PdrspqcrsEquivClasses();
	const ULONG length = pdrspqcrsInput->Size();

	CColRefSet *pcrsChildInput = (*m_pdrspqcrsInput)[ulChild];
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
		pcrs->Include((*pdrspqcrsInput)[ul]);
		pcrs->Intersection(pcrsChildInput);

		if (0 == pcrs->Size())
		{
			pcrs->Release();
			continue;
		}

		// replace each input column with its corresponding output column
		pcrs->Replace((*m_pdrspqdrspqcrInput)[ulChild], m_pdrspqcrOutput);

		pdrspqcrs->Append(pcrs);
	}

	return pdrspqcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PdrspqcnstrColumn
//
//	@doc:
//		Get constraints for a given output column from all children
//
//---------------------------------------------------------------------------
CConstraintArray *
CLogicalSetOp::PdrspqcnstrColumn(CMemoryPool *mp, CExpressionHandle &exprhdl,
								ULONG ulColIndex, ULONG ulStart) const
{
	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	CColRef *colref = (*m_pdrspqcrOutput)[ulColIndex];
	if (!CUtils::FConstrainableType(colref->RetrieveType()->MDId()))
	{
		return pdrspqcnstr;
	}

	const ULONG ulChildren = exprhdl.Arity();
	for (ULONG ul = ulStart; ul < ulChildren; ul++)
	{
		CConstraint *pcnstr = PcnstrColumn(mp, exprhdl, ulColIndex, ul);
		if (NULL == pcnstr)
		{
			pcnstr =
				CConstraintInterval::PciUnbounded(mp, colref, true /*is_null*/);
		}
		SPQOS_ASSERT(NULL != pcnstr);
		pdrspqcnstr->Append(pcnstr);
	}

	return pdrspqcnstr;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PcnstrColumn
//
//	@doc:
//		Get constraint for a given output column from a given children
//
//---------------------------------------------------------------------------
CConstraint *
CLogicalSetOp::PcnstrColumn(CMemoryPool *mp, CExpressionHandle &exprhdl,
							ULONG ulColIndex, ULONG ulChild) const
{
	SPQOS_ASSERT(ulChild < exprhdl.Arity());

	// constraint from child
	CConstraint *pcnstrChild =
		exprhdl.DerivePropertyConstraint(ulChild)->Pcnstr();
	if (NULL == pcnstrChild)
	{
		return NULL;
	}

	// part of constraint on the current input column
	CConstraint *pcnstrCol =
		pcnstrChild->Pcnstr(mp, (*(*m_pdrspqdrspqcrInput)[ulChild])[ulColIndex]);
	if (NULL == pcnstrCol)
	{
		return NULL;
	}

	// make a copy of this constraint but for the output column instead
	CConstraint *pcnstrOutput =
		pcnstrCol->PcnstrRemapForColumn(mp, (*m_pdrspqcrOutput)[ulColIndex]);
	pcnstrCol->Release();
	return pcnstrOutput;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PpcDeriveConstraintSetop
//
//	@doc:
//		Derive constraint property for difference, intersect, and union
//		operators
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalSetOp::PpcDeriveConstraintSetop(CMemoryPool *mp,
										CExpressionHandle &exprhdl,
										BOOL fIntersect) const
{
	const ULONG num_cols = m_pdrspqcrOutput->Size();

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		// get constraints for this column from all children
		CConstraintArray *pdrspqcnstrCol =
			PdrspqcnstrColumn(mp, exprhdl, ul, 0 /*ulStart*/);

		CConstraint *pcnstrCol = NULL;
		if (fIntersect)
		{
			pcnstrCol = CConstraint::PcnstrConjunction(mp, pdrspqcnstrCol);
		}
		else
		{
			pcnstrCol = CConstraint::PcnstrDisjunction(mp, pdrspqcnstrCol);
		}

		if (NULL != pcnstrCol)
		{
			pdrspqcnstr->Append(pcnstrCol);
		}
	}

	CConstraint *pcnstrAll = CConstraint::PcnstrConjunction(mp, pdrspqcnstr);

	CColRefSetArray *pdrspqcrs =
		PdrspqcrsOutputEquivClasses(mp, exprhdl, fIntersect);

	return SPQOS_NEW(mp) CPropConstraint(mp, pdrspqcrs, pcnstrAll);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::PcrsStat
//
//	@doc:
//		Compute required stats columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSetOp::PcrsStat(CMemoryPool *,		  // mp
						CExpressionHandle &,  //exprhdl,
						CColRefSet *,		  //pcrsInput
						ULONG child_index) const
{
	CColRefSet *pcrs = (*m_pdrspqcrsInput)[child_index];
	pcrs->AddRef();

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSetOp::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalSetOp::OsPrint(IOstream &os) const
{
	os << SzId() << " Output: (";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << ")";

	os << ", Input: [";
	const ULONG ulChildren = m_pdrspqdrspqcrInput->Size();
	for (ULONG ul = 0; ul < ulChildren; ul++)
	{
		os << "(";
		CUtils::OsPrintDrgPcr(os, (*m_pdrspqdrspqcrInput)[ul]);
		os << ")";

		if (ul < ulChildren - 1)
		{
			os << ", ";
		}
	}
	os << "]";

	return os;
}

// EOF
