//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSearchStage.cpp
//
//	@doc:
//		Implementation of optimizer search stage
//---------------------------------------------------------------------------

#include "spqopt/search/CSearchStage.h"

#include "spqopt/xforms/CXformFactory.h"

using namespace spqopt;
using namespace spqos;

FORCE_GENERATE_DBGSTR(CSearchStage);

//---------------------------------------------------------------------------
//	@function:
//		CSearchStage::CSearchStage
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CSearchStage::CSearchStage(CXformSet *xform_set, ULONG ulTimeThreshold,
						   CCost costThreshold)
	: m_xforms(xform_set),
	  m_time_threshold(ulTimeThreshold),
	  m_cost_threshold(costThreshold),
	  m_pexprBest(NULL),
	  m_costBest(SPQOPT_INVALID_COST)
{
	SPQOS_ASSERT(NULL != xform_set);
	SPQOS_ASSERT(0 < xform_set->Size());

	// include all implementation rules in any search strategy
	m_xforms->Union(CXformFactory::Pxff()->PxfsImplementation());
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStage::~CSearchStage
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CSearchStage::~CSearchStage()
{
	m_xforms->Release();
	CRefCount::SafeRelease(m_pexprBest);
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStage::OsPrint
//
//	@doc:
//		Print job
//
//---------------------------------------------------------------------------
IOstream &
CSearchStage::OsPrint(IOstream &os) const
{
	os << "Search Stage" << std::endl
	   << "\ttime threshold: " << m_time_threshold
	   << ", cost threshold:" << m_cost_threshold
	   << ", best plan found: " << std::endl;

	if (NULL != m_pexprBest)
	{
		os << *m_pexprBest;
	}

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CSearchStage::SetBestExpr
//
//	@doc:
//		Set best plan found at the end of search stage
//
//---------------------------------------------------------------------------
void
CSearchStage::SetBestExpr(CExpression *pexpr)
{
	SPQOS_ASSERT_IMP(NULL != pexpr, pexpr->Pop()->FPhysical());

	m_pexprBest = pexpr;
	if (NULL != m_pexprBest)
	{
		m_costBest = m_pexprBest->Cost();
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CSearchStage::PdrspqssDefault
//
//	@doc:
//		Generate default search strategy;
//		one stage with all xforms and no time/cost thresholds
//
//---------------------------------------------------------------------------
CSearchStageArray *
CSearchStage::PdrspqssDefault(CMemoryPool *mp)
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	xform_set->Union(CXformFactory::Pxff()->PxfsExploration());
	CSearchStageArray *search_stage_array = SPQOS_NEW(mp) CSearchStageArray(mp);

	search_stage_array->Append(SPQOS_NEW(mp) CSearchStage(xform_set));

	return search_stage_array;
}

// EOF
