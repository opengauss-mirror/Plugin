//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformUpdate2DML.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformUpdate2DML.h"

#include "spqos/base.h"

#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalPartitionSelector.h"
#include "spqopt/operators/CLogicalSplit.h"
#include "spqopt/operators/CLogicalUpdate.h"
#include "spqopt/operators/CPatternLeaf.h"
#include "spqopt/operators/CScalarDMLAction.h"
#include "spqopt/operators/CScalarProjectElement.h"
#include "spqopt/operators/CScalarProjectList.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/xforms/CXformUtils.h"
#include "naucrates/md/IMDTypeInt4.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformUpdate2DML::CXformUpdate2DML
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformUpdate2DML::CXformUpdate2DML(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(
			  mp, SPQOS_NEW(mp) CLogicalUpdate(mp),
			  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CPatternLeaf(mp))))
{
}

//---------------------------------------------------------------------------
//	@function:
//		CXformUpdate2DML::Exfp
//
//	@doc:
//		Compute promise of xform
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformUpdate2DML::Exfp(CExpressionHandle &	// exprhdl
) const
{
	return CXform::ExfpHigh;
}

//---------------------------------------------------------------------------
//	@function:
//		CXformUpdate2DML::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformUpdate2DML::Transform(CXformContext *pxfctxt, CXformResult *pxfres,
							CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CLogicalUpdate *popUpdate = CLogicalUpdate::PopConvert(pexpr->Pop());
	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components for alternative

	CTableDescriptor *ptabdesc = popUpdate->Ptabdesc();
	CColRefArray *pdrspqcrDelete = popUpdate->PdrspqcrDelete();
	CColRefArray *pdrspqcrInsert = popUpdate->PdrspqcrInsert();
	CColRef *pcrCtid = popUpdate->PcrCtid();
	CColRef *pcrSegmentId = popUpdate->PcrSegmentId();
	CColRef *pcrTupleOid = popUpdate->PcrTupleOid();

	// child of update operator
	CExpression *pexprChild = (*pexpr)[0];
	pexprChild->AddRef();

	IMDId *rel_mdid = ptabdesc->MDId();
	if (CXformUtils::FTriggersExist(CLogicalDML::EdmlUpdate, ptabdesc,
									true /*fBefore*/))
	{
		rel_mdid->AddRef();
		pdrspqcrDelete->AddRef();
		pdrspqcrInsert->AddRef();
		pexprChild = CXformUtils::PexprRowTrigger(
			mp, pexprChild, CLogicalDML::EdmlUpdate, rel_mdid, true /*fBefore*/,
			pdrspqcrDelete, pdrspqcrInsert);
	}

	// generate the action column and split operator
	COptCtxt *poctxt = COptCtxt::PoctxtFromTLS();
	CMDAccessor *md_accessor = poctxt->Pmda();
	CColumnFactory *col_factory = poctxt->Pcf();

	pdrspqcrDelete->AddRef();
	pdrspqcrInsert->AddRef();

	const IMDType *pmdtype = md_accessor->PtMDType<IMDTypeInt4>();
	CColRef *pcrAction = col_factory->PcrCreate(pmdtype, default_type_modifier);

	CExpression *pexprProjElem = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarProjectElement(mp, pcrAction),
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarDMLAction(mp)));

	CExpression *pexprProjList = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pexprProjElem);
	CExpression *pexprSplit = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalSplit(mp, pdrspqcrDelete, pdrspqcrInsert, pcrCtid,
								   pcrSegmentId, pcrAction, pcrTupleOid),
		pexprChild, pexprProjList);

	// add assert checking that no NULL values are inserted for nullable columns or no check constraints are violated
	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	CExpression *pexprAssertConstraints;
	if (optimizer_config->GetHint()->FEnforceConstraintsOnDML())
	{
		pexprAssertConstraints = CXformUtils::PexprAssertConstraints(
			mp, pexprSplit, ptabdesc, pdrspqcrInsert);
	}
	else
	{
		pexprAssertConstraints = pexprSplit;
	}

	const ULONG num_cols = pdrspqcrInsert->Size();

	CBitSet *pbsModified = SPQOS_NEW(mp) CBitSet(mp, ptabdesc->ColumnCount());
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *pcrInsert = (*pdrspqcrInsert)[ul];
		CColRef *pcrDelete = (*pdrspqcrDelete)[ul];
		if (pcrInsert != pcrDelete)
		{
			// delete columns refer to the original tuple's descriptor, if it's different
			// from the corresponding insert column, then we're modifying the column
			// at that position
			pbsModified->ExchangeSet(ul);
		}
	}
	// create logical DML
	ptabdesc->AddRef();
	pdrspqcrDelete->AddRef();
	CExpression *pexprDML = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalDML(mp, CLogicalDML::EdmlUpdate, ptabdesc,
								 pdrspqcrDelete, pbsModified, pcrAction, pcrCtid,
								 pcrSegmentId, pcrTupleOid),
		pexprAssertConstraints);

	// TODO:  - Oct 30, 2012; detect and handle AFTER triggers on update

	pxfres->Add(pexprDML);
}

// EOF
