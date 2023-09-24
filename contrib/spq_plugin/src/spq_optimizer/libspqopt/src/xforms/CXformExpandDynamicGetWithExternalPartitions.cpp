//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformExpandDynamicGetWithExternalPartitions.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformExpandDynamicGetWithExternalPartitions.h"

#include "spqos/base.h"

#include "spqopt/exception.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CLogicalMultiExternalGet.h"
#include "spqopt/operators/CLogicalUnionAll.h"
#include "spqopt/xforms/CXformUtils.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformExpandDynamicGetWithExternalPartitions::CXformExpandDynamicGetWithExternalPartitions
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformExpandDynamicGetWithExternalPartitions::
	CXformExpandDynamicGetWithExternalPartitions(CMemoryPool *mp)
	: CXformExploration(
		  // pattern
		  SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalDynamicGet(mp)))
{
}


// Converts a CLogicalDynamicGet on a partitioned table containing external partitions
// into a UNION ALL over two partial scans:
// - Partial CLogicalDynamicGet with part constraints of non-external partitions
// - Partial CLogicalMultiExternalGet with part constraints for all external partitions
void
CXformExpandDynamicGetWithExternalPartitions::Transform(
	CXformContext *pxfctxt, CXformResult *pxfres, CExpression *pexpr) const
{
	SPQOS_ASSERT(NULL != pxfctxt);
	SPQOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	SPQOS_ASSERT(FCheckPattern(pexpr));

	CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();

	CLogicalDynamicGet *popGet = CLogicalDynamicGet::PopConvert(pexpr->Pop());
	CTableDescriptor *ptabdesc = popGet->Ptabdesc();
	const IMDRelation *relation = mda->RetrieveRel(ptabdesc->MDId());

	if (!relation->HasExternalPartitions() || popGet->IsPartial())
	{
		// no external partitions or already a partial dynamic get;
		// do not try to split further
		return;
	}

	CMemoryPool *mp = pxfctxt->Pmp();

	// Iterate over all the External Scans to determine partial scan contraints
	CColRef2dArray *pdrspqdrspqcrPartKeys = popGet->PdrspqdrspqcrPart();

	// capture constraints of all external and remaining (non-external) scans in
	// ppartcnstrCovered & ppartcnstrRest respectively
	CPartConstraint *ppartcnstrCovered = NULL;
	CPartConstraint *ppartcnstrRest = NULL;

	IMdIdArray *external_part_mdids = relation->GetExternalPartitions();
	SPQOS_ASSERT(external_part_mdids->Size() > 0);
	for (ULONG ul = 0; ul < external_part_mdids->Size(); ul++)
	{
		IMDId *extpart_mdid = (*external_part_mdids)[ul];
		const IMDRelation *extpart = mda->RetrieveRel(extpart_mdid);
		SPQOS_ASSERT(NULL != extpart->MDPartConstraint());

		CPartConstraint *ppartcnstr = CUtils::PpartcnstrFromMDPartCnstr(
			mp, mda, pdrspqdrspqcrPartKeys, extpart->MDPartConstraint(),
			popGet->PdrspqcrOutput());
		SPQOS_ASSERT(NULL != ppartcnstr);

		CPartConstraint *ppartcnstrNewlyCovered =
			CXformUtils::PpartcnstrDisjunction(mp, ppartcnstrCovered,
											   ppartcnstr);

		if (NULL == ppartcnstrNewlyCovered)
		{
			// FIXME: Can this happen here?
			CRefCount::SafeRelease(ppartcnstr);
			continue;
		}
		CRefCount::SafeRelease(ppartcnstrCovered);
		CRefCount::SafeRelease(ppartcnstr);
		ppartcnstrCovered = ppartcnstrNewlyCovered;
	}
	CPartConstraint *ppartcnstrRel = CUtils::PpartcnstrFromMDPartCnstr(
		mp, mda, popGet->PdrspqdrspqcrPart(), relation->MDPartConstraint(),
		popGet->PdrspqcrOutput());
	ppartcnstrRest = ppartcnstrRel->PpartcnstrRemaining(mp, ppartcnstrCovered);

	// PpartcnstrRemaining() returns NULL if ppartcnstrCovered has no constraint
	// on the first level and contraints on higher levels are bounded (see
	// CPartConstraint::FCanNegate()), which is the case for external partitions
	// on multi-level partitioned tables,
	// FIXME: Support multi-level external partitions
	if (ppartcnstrRest == NULL)
	{
		// FIXME: Just return here instead? OR fall back earlier in the translator?
		SPQOS_RAISE(
			spqopt::ExmaSPQOPT, spqopt::ExmiUnsupportedOp,
			SPQOS_WSZ_LIT(
				"Query over external partitions in multi-level partitioned table"));
	}

	// Create new partial DynamicGet node with part constraints from ppartcnstrRest
	CName *pnameDG = SPQOS_NEW(mp) CName(mp, popGet->Name());
	ptabdesc->AddRef();
	popGet->PdrspqcrOutput()->AddRef();
	popGet->PdrspqdrspqcrPart()->AddRef();
	CLogicalDynamicGet *popPartialDynamicGet = SPQOS_NEW(mp) CLogicalDynamicGet(
		mp, pnameDG, ptabdesc, popGet->ScanId(), popGet->PdrspqcrOutput(),
		popGet->PdrspqdrspqcrPart(),
		COptCtxt::PoctxtFromTLS()->UlPartIndexNextVal(), true, /* is_partial */
		ppartcnstrRest, ppartcnstrRel);

	CExpression *pexprPartialDynamicGet =
		SPQOS_NEW(mp) CExpression(mp, popPartialDynamicGet);

	// Create new MultiExternalGet node capturing all the external scans with part constraints
	// from ppartcnstrCovered
	CName *pnameMEG = SPQOS_NEW(mp) CName(mp, popGet->Name());
	CColRefArray *pdrspqcrNew = CUtils::PdrspqcrCopy(mp, popGet->PdrspqcrOutput());
	ptabdesc->AddRef();
	external_part_mdids->AddRef();

	CLogicalMultiExternalGet *popMultiExternalGet = SPQOS_NEW(mp)
		CLogicalMultiExternalGet(mp, external_part_mdids, pnameMEG, ptabdesc,
								 popGet->ScanId(), pdrspqcrNew);
	popMultiExternalGet->SetSecondaryScanId(
		COptCtxt::PoctxtFromTLS()->UlPartIndexNextVal());
	popMultiExternalGet->SetPartial();
	popMultiExternalGet->SetPartConstraint(ppartcnstrCovered);
	CExpression *pexprMultiExternalGet =
		SPQOS_NEW(mp) CExpression(mp, popMultiExternalGet);

	// Create a UNION ALL node above the two Gets
	CColRef2dArray *pdrspqdrspqcrInput = SPQOS_NEW(mp) CColRef2dArray(mp);

	popPartialDynamicGet->PdrspqcrOutput()->AddRef();
	pdrspqdrspqcrInput->Append(popPartialDynamicGet->PdrspqcrOutput());
	popMultiExternalGet->PdrspqcrOutput()->AddRef();
	pdrspqdrspqcrInput->Append(popMultiExternalGet->PdrspqcrOutput());

	CExpressionArray *pdrspqexprInput = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprInput->Append(pexprPartialDynamicGet);
	pdrspqexprInput->Append(pexprMultiExternalGet);

	popGet->PdrspqcrOutput()->AddRef();
	CExpression *pexprResult = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CLogicalUnionAll(mp, popGet->PdrspqcrOutput(),
									  pdrspqdrspqcrInput, popGet->ScanId()),
		pdrspqexprInput);

	// add alternative to transformation result
	pxfres->Add(pexprResult);
}


// EOF
