//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CWindowFrame.cpp
//
//	@doc:
//		Implementation of window frame
//---------------------------------------------------------------------------

#include "spqopt/base/CWindowFrame.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CScalarIdent.h"


using namespace spqopt;

FORCE_GENERATE_DBGSTR(CWindowFrame);

// string encoding of frame specification
const CHAR rgszFrameSpec[][10] = {"Rows", "Range"};
SPQOS_CPL_ASSERT(CWindowFrame::EfsSentinel == SPQOS_ARRAY_SIZE(rgszFrameSpec));

// string encoding of frame boundary
const CHAR rgszFrameBoundary[][40] = {"Unbounded Preceding",
									  "Bounded Preceding",
									  "Current",
									  "Unbounded Following",
									  "Bounded Following",
									  "Delayed Bounded Preceding",
									  "Delayed Bounded Following"};
SPQOS_CPL_ASSERT(CWindowFrame::EfbSentinel ==
				SPQOS_ARRAY_SIZE(rgszFrameBoundary));

// string encoding of frame exclusion strategy
const CHAR rgszFrameExclusionStrategy[][20] = {"None", "Nulls", "Current",
											   "MatchingOthers", "Ties"};
SPQOS_CPL_ASSERT(CWindowFrame::EfesSentinel ==
				SPQOS_ARRAY_SIZE(rgszFrameExclusionStrategy));

// empty window frame
const CWindowFrame CWindowFrame::m_wfEmpty;

//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::CWindowFrame
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CWindowFrame::CWindowFrame(CMemoryPool *mp, EFrameSpec efs,
						   EFrameBoundary efbLeading,
						   EFrameBoundary efbTrailing,
						   CExpression *pexprLeading,
						   CExpression *pexprTrailing,
						   EFrameExclusionStrategy efes)
	: m_efs(efs),
	  m_efbLeading(efbLeading),
	  m_efbTrailing(efbTrailing),
	  m_pexprLeading(pexprLeading),
	  m_pexprTrailing(pexprTrailing),
	  m_efes(efes),
	  m_pcrsUsed(NULL)
{
	SPQOS_ASSERT_IMP(EfbBoundedPreceding == m_efbLeading ||
						EfbBoundedFollowing == m_efbLeading,
					NULL != pexprLeading);
	SPQOS_ASSERT_IMP(EfbBoundedPreceding == m_efbTrailing ||
						EfbBoundedFollowing == m_efbTrailing,
					NULL != pexprTrailing);

	// include used columns by frame edges
	m_pcrsUsed = SPQOS_NEW(mp) CColRefSet(mp);
	if (NULL != pexprLeading)
	{
		m_pcrsUsed->Include(pexprLeading->DeriveUsedColumns());
	}

	if (NULL != pexprTrailing)
	{
		m_pcrsUsed->Include(pexprTrailing->DeriveUsedColumns());
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::CWindowFrame
//
//	@doc:
//		Private dummy ctor used for creating empty frame
//
//---------------------------------------------------------------------------
CWindowFrame::CWindowFrame()
	: m_efs(EfsRange),
	  m_efbLeading(EfbUnboundedPreceding),
	  m_efbTrailing(EfbCurrentRow),
	  m_pexprLeading(NULL),
	  m_pexprTrailing(NULL),
	  m_efes(EfesNone),
	  m_pcrsUsed(NULL)
{
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::~CWindowFrame
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CWindowFrame::~CWindowFrame()
{
	CRefCount::SafeRelease(m_pexprLeading);
	CRefCount::SafeRelease(m_pexprTrailing);
	CRefCount::SafeRelease(m_pcrsUsed);
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::Matches
//
//	@doc:
//		Check for equality between window frames
//
//---------------------------------------------------------------------------
BOOL
CWindowFrame::Matches(const CWindowFrame *pwf) const
{
	return m_efs == pwf->Efs() && m_efbLeading == pwf->EfbLeading() &&
		   m_efbTrailing == pwf->EfbTrailing() && m_efes == pwf->Efes() &&
		   CUtils::Equals(m_pexprLeading, pwf->PexprLeading()) &&
		   CUtils::Equals(m_pexprTrailing, pwf->PexprTrailing());
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::HashValue
//
//	@doc:
//		Hash of components
//
//---------------------------------------------------------------------------
ULONG
CWindowFrame::HashValue() const
{
	ULONG ulHash = 0;
	ulHash = spqos::CombineHashes(ulHash, m_efs);
	ulHash = spqos::CombineHashes(ulHash, m_efbLeading);
	ulHash = spqos::CombineHashes(ulHash, m_efbTrailing);
	ulHash = spqos::CombineHashes(ulHash, m_efes);
	if (NULL != m_pexprLeading)
	{
		ulHash =
			spqos::CombineHashes(ulHash, CExpression::HashValue(m_pexprLeading));
	}

	if (NULL != m_pexprTrailing)
	{
		ulHash = spqos::CombineHashes(ulHash,
									 CExpression::HashValue(m_pexprTrailing));
	}

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::PwfCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the window frame with remapped columns
//
//---------------------------------------------------------------------------
CWindowFrame *
CWindowFrame::PwfCopyWithRemappedColumns(CMemoryPool *mp,
										 UlongToColRefMap *colref_mapping,
										 BOOL must_exist)
{
	if (this == &m_wfEmpty)
	{
		this->AddRef();
		return this;
	}

	CExpression *pexprLeading = NULL;
	if (NULL != m_pexprLeading)
	{
		pexprLeading = m_pexprLeading->PexprCopyWithRemappedColumns(
			mp, colref_mapping, must_exist);
	}

	CExpression *pexprTrailing = NULL;
	if (NULL != m_pexprTrailing)
	{
		pexprTrailing = m_pexprTrailing->PexprCopyWithRemappedColumns(
			mp, colref_mapping, must_exist);
	}

	return SPQOS_NEW(mp) CWindowFrame(mp, m_efs, m_efbLeading, m_efbTrailing,
									 pexprLeading, pexprTrailing, m_efes);
}

//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::OsPrint
//
//	@doc:
//		Print window frame
//
//---------------------------------------------------------------------------
IOstream &
CWindowFrame::OsPrint(IOstream &os) const
{
	if (this == &m_wfEmpty)
	{
		os << "EMPTY FRAME";
		return os;
	}

	os << "[" << rgszFrameSpec[m_efs] << ", ";

	os << "Trail: " << rgszFrameBoundary[m_efbTrailing];
	if (NULL != m_pexprTrailing)
	{
		os << " " << *m_pexprTrailing;
	}

	os << ", Lead: " << rgszFrameBoundary[m_efbLeading];
	if (NULL != m_pexprLeading)
	{
		os << " " << *m_pexprLeading;
	}

	os << ", " << rgszFrameExclusionStrategy[m_efes];

	os << "]";

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::Equals
//
//	@doc:
//		 Matching function over frame arrays
//
//---------------------------------------------------------------------------
BOOL
CWindowFrame::Equals(const CWindowFrameArray *pdrspqwfFirst,
					 const CWindowFrameArray *pdrspqwfSecond)
{
	if (NULL == pdrspqwfFirst || NULL == pdrspqwfSecond)
	{
		return (NULL == pdrspqwfFirst && NULL == pdrspqwfSecond);
	}

	if (pdrspqwfFirst->Size() != pdrspqwfSecond->Size())
	{
		return false;
	}

	const ULONG size = pdrspqwfFirst->Size();
	BOOL fMatch = true;
	for (ULONG ul = 0; fMatch && ul < size; ul++)
	{
		fMatch = (*pdrspqwfFirst)[ul]->Matches((*pdrspqwfSecond)[ul]);
	}

	return fMatch;
}


//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::HashValue
//
//	@doc:
//		 Combine hash values of a maximum number of entries
//
//---------------------------------------------------------------------------
ULONG
CWindowFrame::HashValue(const CWindowFrameArray *pdrspqwf, ULONG ulMaxSize)
{
	SPQOS_ASSERT(NULL != pdrspqwf);
	const ULONG size = std::min(ulMaxSize, pdrspqwf->Size());

	ULONG ulHash = 0;
	for (ULONG ul = 0; ul < size; ul++)
	{
		ulHash = spqos::CombineHashes(ulHash, (*pdrspqwf)[ul]->HashValue());
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CWindowFrame::OsPrint
//
//	@doc:
//		 Print array of window frame objects
//
//---------------------------------------------------------------------------
IOstream &
CWindowFrame::OsPrint(IOstream &os, const CWindowFrameArray *pdrspqwf)
{
	os << "[";
	const ULONG size = pdrspqwf->Size();
	if (0 < size)
	{
		for (ULONG ul = 0; ul < size - 1; ul++)
		{
			(void) (*pdrspqwf)[ul]->OsPrint(os);
			os << ", ";
		}

		(void) (*pdrspqwf)[size - 1]->OsPrint(os);
	}

	return os << "]";
}


// EOF
