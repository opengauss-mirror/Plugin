//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CXformImplementPartitionSelector.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformImplementPartitionSelector.h"

#include "spqos/base.h"

#include "spqopt/operators/CLogicalPartitionSelector.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CPhysicalPartitionSelectorDML.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformImplementPartitionSelector::CXformImplementPartitionSelector
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformImplementPartitionSelector::CXformImplementPartitionSelector(
	CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(SPQOS_NEW(mp) CExpression(
		  mp, SPQOS_NEW(mp) CLogicalPartitionSelector(mp),
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CPatternLeaf(mp))  // relational child
		  ))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformImplementPartitionSelector::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformImplementPartitionSelector::Transform(CXformContext *pxfctxt,
											CXformResult *pxfres,
											CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CLogicalPartitionSelector *popSelector =
		CLogicalPartitionSelector::PopConvert(pexpr->Pop());
	CExpression *pexprRelational = (*pexpr)[0];

	IMDId *mdid = popSelector->MDId();

	// addref all components
	pexprRelational->AddRef();
	mdid->AddRef();

	UlongToExprMap *phmulexprFilter = SPQOS_NEW(mp) UlongToExprMap(mp);

	const ULONG ulLevels = popSelector->UlPartLevels();
	for (ULONG ul = 0; ul < ulLevels; ul++)
	{
		CExpression *pexprFilter = popSelector->PexprPartFilter(ul);
		SPQOS_ASSERT(NULL != pexprFilter);
		pexprFilter->AddRef();
#ifdef SPQOS_DEBUG
		BOOL fInserted =
#endif
			phmulexprFilter->Insert(SPQOS_NEW(mp) ULONG(ul), pexprFilter);
		SPQOS_ASSERT(fInserted);
	}

	// assemble physical operator
	CPhysicalPartitionSelectorDML *popPhysicalPartitionSelector =
		SPQOS_NEW(mp) CPhysicalPartitionSelectorDML(mp, mdid, phmulexprFilter);

	CExpression *pexprPartitionSelector = SPQOS_NEW(mp)
		CExpression(mp, popPhysicalPartitionSelector, pexprRelational);

	// add alternative to results
	pxfres->Add(pexprPartitionSelector);
}

// EOF
