//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalSequenceProject.cpp
//
//	@doc:
//		Implementation of sequence project operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalSequenceProject.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDistributionSpec.h"
#include "spqopt/base/CDistributionSpecHashed.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::CLogicalSequenceProject
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalSequenceProject::CLogicalSequenceProject(CMemoryPool *mp,
												 CDistributionSpec *pds,
												 COrderSpecArray *pdrspqos,
												 CWindowFrameArray *pdrspqwf)
	: CLogicalUnary(mp),
	  m_pds(pds),
	  m_pdrspqos(pdrspqos),
	  m_pdrspqwf(pdrspqwf),
	  m_fHasOrderSpecs(false),
	  m_fHasFrameSpecs(false)
{
	SPQOS_ASSERT(NULL != pds);
	SPQOS_ASSERT(NULL != pdrspqos);
	SPQOS_ASSERT(NULL != pdrspqwf);
	SPQOS_ASSERT(CDistributionSpec::EdtHashed == pds->Edt() ||
				CDistributionSpec::EdtSingleton == pds->Edt());

	// set flags indicating that current operator has non-empty order specs/frame specs
	SetHasOrderSpecs(mp);
	SetHasFrameSpecs(mp);

	// include columns used by Partition By, Order By, and window frame edges
	CColRefSet *pcrsSort = COrderSpec::GetColRefSet(mp, m_pdrspqos);
	m_pcrsLocalUsed->Include(pcrsSort);
	pcrsSort->Release();

	if (CDistributionSpec::EdtHashed == m_pds->Edt())
	{
		CColRefSet *pcrsHashed =
			CDistributionSpecHashed::PdsConvert(m_pds)->PcrsUsed(mp);
		m_pcrsLocalUsed->Include(pcrsHashed);
		pcrsHashed->Release();
	}

	const ULONG ulFrames = m_pdrspqwf->Size();
	for (ULONG ul = 0; ul < ulFrames; ul++)
	{
		CWindowFrame *pwf = (*m_pdrspqwf)[ul];
		if (!CWindowFrame::IsEmpty(pwf))
		{
			m_pcrsLocalUsed->Include(pwf->PcrsUsed());
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::CLogicalSequenceProject
//
//	@doc:
//		Ctor for patterns
//
//---------------------------------------------------------------------------
CLogicalSequenceProject::CLogicalSequenceProject(CMemoryPool *mp)
	: CLogicalUnary(mp),
	  m_pds(NULL),
	  m_pdrspqos(NULL),
	  m_pdrspqwf(NULL),
	  m_fHasOrderSpecs(false),
	  m_fHasFrameSpecs(false)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::~CLogicalSequenceProject
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalSequenceProject::~CLogicalSequenceProject()
{
	CRefCount::SafeRelease(m_pds);
	CRefCount::SafeRelease(m_pdrspqos);
	CRefCount::SafeRelease(m_pdrspqwf);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalSequenceProject::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CDistributionSpec *pds =
		m_pds->PdsCopyWithRemappedColumns(mp, colref_mapping, must_exist);

	COrderSpecArray *pdrspqos = SPQOS_NEW(mp) COrderSpecArray(mp);
	const ULONG ulOrderSpec = m_pdrspqos->Size();
	for (ULONG ul = 0; ul < ulOrderSpec; ul++)
	{
		COrderSpec *pos =
			((*m_pdrspqos)[ul])
				->PosCopyWithRemappedColumns(mp, colref_mapping, must_exist);
		pdrspqos->Append(pos);
	}

	CWindowFrameArray *pdrspqwf = SPQOS_NEW(mp) CWindowFrameArray(mp);
	const ULONG ulWindowFrames = m_pdrspqwf->Size();
	for (ULONG ul = 0; ul < ulWindowFrames; ul++)
	{
		CWindowFrame *pwf =
			((*m_pdrspqwf)[ul])
				->PwfCopyWithRemappedColumns(mp, colref_mapping, must_exist);
		pdrspqwf->Append(pwf);
	}

	return SPQOS_NEW(mp) CLogicalSequenceProject(mp, pds, pdrspqos, pdrspqwf);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::SetHasOrderSpecs
//
//	@doc:
//		Set the flag indicating that SeqPrj has specified order specs
//
//---------------------------------------------------------------------------
void
CLogicalSequenceProject::SetHasOrderSpecs(CMemoryPool *mp)
{
	SPQOS_ASSERT(NULL != m_pdrspqos);

	const ULONG ulOrderSpecs = m_pdrspqos->Size();
	if (0 == ulOrderSpecs)
	{
		// if no order specs are given, we add one empty order spec
		m_pdrspqos->Append(SPQOS_NEW(mp) COrderSpec(mp));
	}
	BOOL fHasOrderSpecs = false;
	for (ULONG ul = 0; !fHasOrderSpecs && ul < ulOrderSpecs; ul++)
	{
		fHasOrderSpecs = !(*m_pdrspqos)[ul]->IsEmpty();
	}
	m_fHasOrderSpecs = fHasOrderSpecs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::SetHasFrameSpecs
//
//	@doc:
//		Set the flag indicating that SeqPrj has specified frame specs
//
//---------------------------------------------------------------------------
void
CLogicalSequenceProject::SetHasFrameSpecs(CMemoryPool *	 // mp
)
{
	SPQOS_ASSERT(NULL != m_pdrspqwf);

	const ULONG ulFrameSpecs = m_pdrspqwf->Size();
	if (0 == ulFrameSpecs)
	{
		// if no frame specs are given, we add one empty frame
		CWindowFrame *pwf =
			const_cast<CWindowFrame *>(CWindowFrame::PwfEmpty());
		pwf->AddRef();
		m_pdrspqwf->Append(pwf);
	}
	BOOL fHasFrameSpecs = false;
	for (ULONG ul = 0; !fHasFrameSpecs && ul < ulFrameSpecs; ul++)
	{
		fHasFrameSpecs = !CWindowFrame::IsEmpty((*m_pdrspqwf)[ul]);
	}
	m_fHasFrameSpecs = fHasFrameSpecs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSequenceProject::DeriveOutputColumns(CMemoryPool *mp,
											 CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	pcrs->Union(exprhdl.DeriveOutputColumns(0));

	// the scalar child defines additional columns
	pcrs->Union(exprhdl.DeriveDefinedColumns(1));

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalSequenceProject::DeriveOuterReferences(CMemoryPool *mp,
											   CExpressionHandle &exprhdl)
{
	CColRefSet *outer_refs =
		CLogical::DeriveOuterReferences(mp, exprhdl, m_pcrsLocalUsed);

	return outer_refs;
}

//---------------------------------------------------------------------------
// CLogicalSequenceProject::FHasLocalReferencesTo
//
// Return true if Partition/Order or window frame edges reference one of
// the provided ColRefs
//
//---------------------------------------------------------------------------
BOOL
CLogicalSequenceProject::FHasLocalReferencesTo(
	const CColRefSet *outerRefsToCheck) const
{
	return !outerRefsToCheck->IsDisjoint(m_pcrsLocalUsed);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalSequenceProject::DeriveKeyCollection(CMemoryPool *,	 // mp
											 CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalSequenceProject::DeriveMaxCard(CMemoryPool *,  // mp
									   CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::Matches
//
//	@doc:
//		Matching function
//
//---------------------------------------------------------------------------
BOOL
CLogicalSequenceProject::Matches(COperator *pop) const
{
	SPQOS_ASSERT(NULL != pop);
	if (Eopid() == pop->Eopid())
	{
		CLogicalSequenceProject *popLogicalSequenceProject =
			CLogicalSequenceProject::PopConvert(pop);
		return m_pds->Matches(popLogicalSequenceProject->Pds()) &&
			   CWindowFrame::Equals(m_pdrspqwf,
									popLogicalSequenceProject->Pdrspqwf()) &&
			   COrderSpec::Equals(m_pdrspqos,
								  popLogicalSequenceProject->Pdrspqos());
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::HashValue
//
//	@doc:
//		Hashing function
//
//---------------------------------------------------------------------------
ULONG
CLogicalSequenceProject::HashValue() const
{
	ULONG ulHash = 0;
	ulHash = spqos::CombineHashes(ulHash, m_pds->HashValue());
	ulHash = spqos::CombineHashes(
		ulHash, CWindowFrame::HashValue(m_pdrspqwf, 3 /*ulMaxSize*/));
	ulHash = spqos::CombineHashes(
		ulHash, COrderSpec::HashValue(m_pdrspqos, 3 /*ulMaxSize*/));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalSequenceProject::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfSequenceProject2Apply);
	(void) xform_set->ExchangeSet(CXform::ExfImplementSequenceProject);

	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::PstatsDerive
//
//	@doc:
//		Derive statistics based on filter predicates
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalSequenceProject::PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *  // stats_ctxt
) const
{
	return PstatsDeriveProject(mp, exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalSequenceProject::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << "Partition By Keys:";
	(void) m_pds->OsPrint(os);
	os << ", ";
	os << "Order Spec:";
	(void) COrderSpec::OsPrint(os, m_pdrspqos);
	os << ", ";
	os << "WindowFrame Spec:";
	(void) CWindowFrame::OsPrint(os, m_pdrspqwf);

	return os << ")";
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalSequenceProject::PopRemoveLocalOuterRefs
//
//	@doc:
//		Filter out outer references from Order By/ Partition By
//		clauses, and return a new operator
//
//---------------------------------------------------------------------------
CLogicalSequenceProject *
CLogicalSequenceProject::PopRemoveLocalOuterRefs(CMemoryPool *mp,
												 CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(this == exprhdl.Pop());

	CColRefSet *outer_refs = exprhdl.DeriveOuterReferences();
	CDistributionSpec *pds = m_pds;
	if (CDistributionSpec::EdtHashed == m_pds->Edt())
	{
		pds =
			CDistributionSpecHashed::PdsConvert(m_pds)->PdshashedExcludeColumns(
				mp, outer_refs);
		if (NULL == pds)
		{
			// if the hashed distribution spec is empty after excluding the outer ref columns,
			// eliminate Partition clause by creating a singleton spec
			pds = SPQOS_NEW(mp) CDistributionSpecSingleton();
		}
	}
	else
	{
		pds->AddRef();
	}

	COrderSpecArray *pdrspqos =
		COrderSpec::PdrspqosExclude(mp, m_pdrspqos, outer_refs);

	// for window frame edges, outer references cannot be removed since this can change
	// the semantics of frame edge from delayed-bounding to unbounded,
	// we re-use the frame edges without changing here
	m_pdrspqwf->AddRef();

	return SPQOS_NEW(mp) CLogicalSequenceProject(mp, pds, pdrspqos, m_pdrspqwf);
}

// EOF
