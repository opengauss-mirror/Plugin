//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalGbAgg.cpp
//
//	@doc:
//		Implementation of aggregate operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalGbAgg.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CGroupByStatsProcessor.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::CLogicalGbAgg
//
//	@doc:
//		ctor for xform pattern
//
//---------------------------------------------------------------------------
CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(true),
	  m_pdrspqcrArgDQA(NULL),
	  m_pdrspqcr(NULL),
	  m_pdrspqcrMinimal(NULL),
	  m_egbaggtype(COperator::EgbaggtypeSentinel),
	  m_aggStage(EasOthers)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::CLogicalGbAgg
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 COperator::EGbAggType egbaggtype)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(false),
	  m_pdrspqcrArgDQA(NULL),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(NULL),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(EasOthers)
{
	if (COperator::EgbaggtypeLocal == egbaggtype)
	{
		// final and intermediate aggregates have to remove duplicates for a given group
		m_fGeneratesDuplicates = true;
	}

	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);
	SPQOS_ASSERT(COperator::EgbaggtypeIntermediate != egbaggtype);

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}

CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 COperator::EGbAggType egbaggtype,
							 EAggStage aggStage)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(false),
	  m_pdrspqcrArgDQA(NULL),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(NULL),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(aggStage)
{
	if (COperator::EgbaggtypeLocal == egbaggtype)
	{
		// final and intermediate aggregates have to remove duplicates for a given group
		m_fGeneratesDuplicates = true;
	}

	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);
	SPQOS_ASSERT(COperator::EgbaggtypeIntermediate != egbaggtype);

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::CLogicalGbAgg
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 COperator::EGbAggType egbaggtype,
							 BOOL fGeneratesDuplicates,
							 CColRefArray *pdrspqcrArgDQA)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(fGeneratesDuplicates),
	  m_pdrspqcrArgDQA(pdrspqcrArgDQA),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(NULL),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(EasOthers)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);
	SPQOS_ASSERT_IMP(NULL == m_pdrspqcrArgDQA,
					COperator::EgbaggtypeIntermediate != egbaggtype);
	SPQOS_ASSERT_IMP(m_fGeneratesDuplicates,
					COperator::EgbaggtypeLocal == egbaggtype);

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}

CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 COperator::EGbAggType egbaggtype,
							 BOOL fGeneratesDuplicates,
							 CColRefArray *pdrspqcrArgDQA, EAggStage aggStage)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(fGeneratesDuplicates),
	  m_pdrspqcrArgDQA(pdrspqcrArgDQA),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(NULL),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(aggStage)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);
	SPQOS_ASSERT_IMP(NULL == m_pdrspqcrArgDQA,
					COperator::EgbaggtypeIntermediate != egbaggtype);
	SPQOS_ASSERT_IMP(m_fGeneratesDuplicates,
					COperator::EgbaggtypeLocal == egbaggtype);

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::CLogicalGbAgg
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 CColRefArray *pdrspqcrMinimal,
							 COperator::EGbAggType egbaggtype)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(true),
	  m_pdrspqcrArgDQA(NULL),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(pdrspqcrMinimal),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(EasOthers)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);
	SPQOS_ASSERT(COperator::EgbaggtypeIntermediate != egbaggtype);

	SPQOS_ASSERT_IMP(NULL != pdrspqcrMinimal,
					pdrspqcrMinimal->Size() <= colref_array->Size());

	if (NULL == pdrspqcrMinimal)
	{
		m_pdrspqcr->AddRef();
		m_pdrspqcrMinimal = m_pdrspqcr;
	}

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::CLogicalGbAgg
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGbAgg::CLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
							 CColRefArray *pdrspqcrMinimal,
							 COperator::EGbAggType egbaggtype,
							 BOOL fGeneratesDuplicates,
							 CColRefArray *pdrspqcrArgDQA)
	: CLogicalUnary(mp),
	  m_fGeneratesDuplicates(fGeneratesDuplicates),
	  m_pdrspqcrArgDQA(pdrspqcrArgDQA),
	  m_pdrspqcr(colref_array),
	  m_pdrspqcrMinimal(pdrspqcrMinimal),
	  m_egbaggtype(egbaggtype),
	  m_aggStage(EasOthers)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(COperator::EgbaggtypeSentinel > egbaggtype);

	SPQOS_ASSERT_IMP(NULL != pdrspqcrMinimal,
					pdrspqcrMinimal->Size() <= colref_array->Size());
	SPQOS_ASSERT_IMP(NULL == m_pdrspqcrArgDQA,
					COperator::EgbaggtypeIntermediate != egbaggtype);
	SPQOS_ASSERT_IMP(m_fGeneratesDuplicates,
					COperator::EgbaggtypeLocal == egbaggtype);

	if (NULL == pdrspqcrMinimal)
	{
		m_pdrspqcr->AddRef();
		m_pdrspqcrMinimal = m_pdrspqcr;
	}

	m_pcrsLocalUsed->Include(m_pdrspqcr);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::~CLogicalGbAgg
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CLogicalGbAgg::~CLogicalGbAgg()
{
	// safe release -- to allow for instances used in patterns
	CRefCount::SafeRelease(m_pdrspqcr);
	CRefCount::SafeRelease(m_pdrspqcrMinimal);
	CRefCount::SafeRelease(m_pdrspqcrArgDQA);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalGbAgg::PopCopyWithRemappedColumns(CMemoryPool *mp,
										  UlongToColRefMap *colref_mapping,
										  BOOL must_exist)
{
	CColRefArray *colref_array =
		CUtils::PdrspqcrRemap(mp, m_pdrspqcr, colref_mapping, must_exist);
	CColRefArray *pdrspqcrMinimal = NULL;
	if (NULL != m_pdrspqcrMinimal)
	{
		pdrspqcrMinimal = CUtils::PdrspqcrRemap(mp, m_pdrspqcrMinimal,
											  colref_mapping, must_exist);
	}

	CColRefArray *pdrspqcrArgDQA = NULL;
	if (NULL != m_pdrspqcrArgDQA)
	{
		pdrspqcrArgDQA = CUtils::PdrspqcrRemap(mp, m_pdrspqcrArgDQA,
											 colref_mapping, must_exist);
	}

	return SPQOS_NEW(mp)
		CLogicalGbAgg(mp, colref_array, pdrspqcrMinimal, Egbaggtype(),
					  m_fGeneratesDuplicates, pdrspqcrArgDQA);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAgg::DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// include the intersection of the grouping columns and the child's output
	pcrs->Include(Pdrspqcr());
	pcrs->Intersection(exprhdl.DeriveOutputColumns(0));

	// the scalar child defines additional columns
	pcrs->Union(exprhdl.DeriveDefinedColumns(1));

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAgg::DeriveOuterReferences(CMemoryPool *mp,
									 CExpressionHandle &exprhdl)
{
	CColRefSet *pcrsGrp = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsGrp->Include(m_pdrspqcr);

	CColRefSet *outer_refs =
		CLogical::DeriveOuterReferences(mp, exprhdl, pcrsGrp);
	pcrsGrp->Release();

	return outer_refs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::DerivePropertyConstraint
//
//	@doc:
//		Derive constraint property
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogicalGbAgg::DerivePropertyConstraint(CMemoryPool *mp,
										CExpressionHandle &exprhdl) const
{
	CColRefSet *pcrsGrouping = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsGrouping->Include(m_pdrspqcr);

	// get the constraints on the grouping columns only
	CPropConstraint *ppc =
		PpcDeriveConstraintRestrict(mp, exprhdl, pcrsGrouping);
	pcrsGrouping->Release();

	return ppc;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PcrsStat
//
//	@doc:
//		Compute required stats columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAgg::PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl,
						CColRefSet *pcrsInput, ULONG child_index) const
{
	return PcrsStatGbAgg(mp, exprhdl, pcrsInput, child_index, m_pdrspqcr);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PcrsStatGbAgg
//
//	@doc:
//		Compute required stats columns for a GbAgg
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAgg::PcrsStatGbAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
							 CColRefSet *pcrsInput, ULONG child_index,
							 CColRefArray *pdrspqcrGrp) const
{
	SPQOS_ASSERT(NULL != pdrspqcrGrp);
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// include grouping columns
	pcrs->Include(pdrspqcrGrp);

	// other columns used in aggregates
	pcrs->Union(exprhdl.DeriveUsedColumns(1));

	// if the grouping column is a computed column, then add its corresponding used columns
	// to required columns for statistics computation
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	const ULONG ulGrpCols = m_pdrspqcr->Size();
	for (ULONG ul = 0; ul < ulGrpCols; ul++)
	{
		CColRef *pcrGrpCol = (*m_pdrspqcr)[ul];
		const CColRefSet *pcrsUsed =
			col_factory->PcrsUsedInComputedCol(pcrGrpCol);
		if (NULL != pcrsUsed)
		{
			pcrs->Union(pcrsUsed);
		}
	}

	CColRefSet *pcrsRequired =
		PcrsReqdChildStats(mp, exprhdl, pcrsInput, pcrs, child_index);
	pcrs->Release();

	return pcrsRequired;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::DeriveNotNullColumns
//
//	@doc:
//		Derive not null columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGbAgg::DeriveNotNullColumns(CMemoryPool *mp,
									CExpressionHandle &exprhdl) const
{
	SPQOS_ASSERT(2 == exprhdl.Arity());

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// include grouping columns
	pcrs->Include(Pdrspqcr());

	// intersect with not nullable columns from relational child
	pcrs->Intersection(exprhdl.DeriveNotNullColumns(0));

	// TODO,  03/18/2012, add nullability info of computed columns

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalGbAgg::HashValue() const
{
	ULONG ulHash = COperator::HashValue();
	ULONG arity = m_pdrspqcr->Size();
	ULONG ulGbaggtype = (ULONG) m_egbaggtype;

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColRef *colref = (*m_pdrspqcr)[ul];
		ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(colref));
	}

	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue<ULONG>(&ulGbaggtype));

	return spqos::CombineHashes(ulHash,
							   spqos::HashValue<BOOL>(&m_fGeneratesDuplicates));
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalGbAgg::DeriveKeyCollection(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const
{
	CKeyCollection *pkc = NULL;

	// Gb produces a key only if it's global
	if (FGlobal())
	{
		if (COperator::EgbaggtypeLocal == m_egbaggtype &&
			!m_fGeneratesDuplicates)
		{
			return pkc;
		}

		if (0 < m_pdrspqcr->Size())
		{
			// grouping columns always constitute a key
			m_pdrspqcr->AddRef();
			pkc = SPQOS_NEW(mp) CKeyCollection(mp, m_pdrspqcr);
		}
		else
		{
			// scalar and single-group aggs produce one row that constitutes a key
			CColRefSet *pcrs = exprhdl.DeriveDefinedColumns(1);

			if (0 == pcrs->Size())
			{
				// aggregate defines no columns, e.g. select 1 from r group by a
				return NULL;
			}

			pcrs->AddRef();
			pkc = SPQOS_NEW(mp) CKeyCollection(mp, pcrs);
		}
	}

	return pkc;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalGbAgg::DeriveMaxCard(CMemoryPool *,	 //mp
							 CExpressionHandle &exprhdl) const
{
	// agg w/o grouping columns produces one row
	if (0 == m_pdrspqcr->Size())
	{
		return CMaxCard(1 /*ull*/);
	}

	// contradictions produce no rows
	if (exprhdl.DerivePropertyConstraint()->FContradiction())
	{
		return CMaxCard(0 /*ull*/);
	}

	return CMaxCard();
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalGbAgg::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalGbAgg *popAgg = reinterpret_cast<CLogicalGbAgg *>(pop);

	return FGeneratesDuplicates() == popAgg->FGeneratesDuplicates() &&
		   popAgg->Egbaggtype() == m_egbaggtype &&
		   CColRef::Equals(m_pdrspqcr, popAgg->m_pdrspqcr) &&
		   CColRef::Equals(m_pdrspqcrMinimal, popAgg->PdrspqcrMinimal()) &&
		   CColRef::Equals(m_pdrspqcrArgDQA, popAgg->PdrspqcrArgDQA());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalGbAgg::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfSimplifyGbAgg);
	(void) xform_set->ExchangeSet(CXform::ExfGbAggWithMDQA2Join);
	(void) xform_set->ExchangeSet(CXform::ExfCollapseGbAgg);
	(void) xform_set->ExchangeSet(CXform::ExfPushGbBelowJoin);
	(void) xform_set->ExchangeSet(CXform::ExfPushGbBelowUnion);
	(void) xform_set->ExchangeSet(CXform::ExfPushGbBelowUnionAll);
	(void) xform_set->ExchangeSet(CXform::ExfSplitGbAgg);
	(void) xform_set->ExchangeSet(CXform::ExfSplitDQA);
	(void) xform_set->ExchangeSet(CXform::ExfGbAgg2Apply);
	(void) xform_set->ExchangeSet(CXform::ExfGbAgg2HashAgg);
	(void) xform_set->ExchangeSet(CXform::ExfGbAgg2StreamAgg);
	(void) xform_set->ExchangeSet(CXform::ExfGbAgg2ScalarAgg);
	(void) xform_set->ExchangeSet(CXform::ExfEagerAgg);
	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalGbAgg::PstatsDerive(CMemoryPool *mp, IStatistics *child_stats,
							CColRefArray *pdrspqcrGroupingCols,
							ULongPtrArray *pdrspqulComputedCols, CBitSet *keys)
{
	const ULONG ulGroupingCols = pdrspqcrGroupingCols->Size();

	// extract grouping column ids
	ULongPtrArray *pdrspqulGroupingCols = SPQOS_NEW(mp) ULongPtrArray(mp);
	for (ULONG ul = 0; ul < ulGroupingCols; ul++)
	{
		CColRef *colref = (*pdrspqcrGroupingCols)[ul];
		pdrspqulGroupingCols->Append(SPQOS_NEW(mp) ULONG(colref->Id()));
	}

	IStatistics *stats = CGroupByStatsProcessor::CalcGroupByStats(
		mp, dynamic_cast<CStatistics *>(child_stats), pdrspqulGroupingCols,
		pdrspqulComputedCols, keys);

	// clean up
	pdrspqulGroupingCols->Release();

	return stats;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalGbAgg::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
							IStatisticsArray *	// not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);
	IStatistics *child_stats = exprhdl.Pstats(0);

	// extract computed columns
	ULongPtrArray *pdrspqulComputedCols = SPQOS_NEW(mp) ULongPtrArray(mp);
	exprhdl.DeriveDefinedColumns(1)->ExtractColIds(mp, pdrspqulComputedCols);

	IStatistics *stats = PstatsDerive(mp, child_stats, Pdrspqcr(),
									  pdrspqulComputedCols, NULL /*keys*/);

	pdrspqulComputedCols->Release();

	return stats;
}

BOOL
CLogicalGbAgg::IsTwoStageScalarDQA() const
{
	return (m_aggStage == EasTwoStageScalarDQA);
}

BOOL
CLogicalGbAgg::IsThreeStageScalarDQA() const
{
	return (m_aggStage == EasThreeStageScalarDQA);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalGbAgg::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << "( ";
	OsPrintGbAggType(os, m_egbaggtype);
	os << " )";
	os << " Grp Cols: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcr);
	os << "]"
	   << "[";
	OsPrintGbAggType(os, m_egbaggtype);
	os << "]";

	os << ", Minimal Grp Cols: [";
	if (NULL != m_pdrspqcrMinimal)
	{
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrMinimal);
	}
	os << "]";

	if (COperator::EgbaggtypeIntermediate == m_egbaggtype)
	{
		os << ", Distinct Cols:[";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrArgDQA);
		os << "]";
	}

	os << ", Generates Duplicates :[ " << FGeneratesDuplicates() << " ] ";

	if (IsTwoStageScalarDQA())
	{
		os << ", m_aggStage :[  Two Stage Scalar DQA  ] ";
	}

	if (IsThreeStageScalarDQA())
	{
		os << ", m_aggStage :[  Three Stage Scalar DQA  ] ";
	}

	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGbAgg::OsPrintGbAggType
//
//	@doc:
//		Helper function to print aggregate type
//
//---------------------------------------------------------------------------
IOstream &
CLogicalGbAgg::OsPrintGbAggType(IOstream &os, COperator::EGbAggType egbaggtype)
{
	switch (egbaggtype)
	{
		case COperator::EgbaggtypeGlobal:
			os << "Global";
			break;

		case COperator::EgbaggtypeIntermediate:
			os << "Intermediate";
			break;

		case COperator::EgbaggtypeLocal:
			os << "Local";
			break;

		default:
			SPQOS_ASSERT(!"Unsupported aggregate type");
	}
	return os;
}

// EOF
