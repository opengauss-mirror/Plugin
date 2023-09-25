//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogical.cpp
//
//	@doc:
//		Implementation of base class of logical operators
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogical.h"

#include "spqos/base.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CConstraintConjunction.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CDrvdPropRelational.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/base/CReqdPropRelational.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CLogicalApply.h"
#include "spqopt/operators/CLogicalBitmapTableGet.h"
#include "spqopt/operators/CLogicalDynamicBitmapTableGet.h"
#include "spqopt/operators/CLogicalDynamicGet.h"
#include "spqopt/operators/CLogicalGet.h"
#include "spqopt/operators/CLogicalNAryJoin.h"
#include "spqopt/operators/CLogicalSelect.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/operators/CScalarIdent.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/md/IMDCheckConstraint.h"
#include "naucrates/md/IMDColumn.h"
#include "naucrates/md/IMDIndex.h"
#include "naucrates/statistics/CStatistics.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqnaucrates;
using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CLogical::CLogical
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogical::CLogical(CMemoryPool *mp) : COperator(mp)
{
	SPQOS_ASSERT(NULL != mp);
	m_pcrsLocalUsed = SPQOS_NEW(mp) CColRefSet(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::~CLogical
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogical::~CLogical()
{
	m_pcrsLocalUsed->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PdrspqcrCreateMapping
//
//	@doc:
//		Create output column mapping given a list of column descriptors and
//		a pointer to the operator creating that column
//
//---------------------------------------------------------------------------
CColRefArray *
CLogical::PdrspqcrCreateMapping(CMemoryPool *mp,
							   const CColumnDescriptorArray *pdrspqcoldesc,
							   ULONG ulOpSourceId, IMDId *mdid_table) const
{
	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	ULONG num_cols = pdrspqcoldesc->Size();

	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp, num_cols);
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		CName name(mp, pcoldesc->Name());
		CColRef *colref = col_factory->PcrCreate(
			pcoldesc, name, ulOpSourceId, false /* mark_as_used */, mdid_table);
		colref_array->Append(colref);
	}

	return colref_array;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PdrspqdrspqcrCreatePartCols
//
//	@doc:
//		Initialize array of partition columns from the array with their indexes
//
//---------------------------------------------------------------------------
CColRef2dArray *
CLogical::PdrspqdrspqcrCreatePartCols(CMemoryPool *mp, CColRefArray *colref_array,
									const ULongPtrArray *pdrspqulPart)
{
	SPQOS_ASSERT(NULL != colref_array && "Output columns cannot be NULL");
	SPQOS_ASSERT(NULL != pdrspqulPart);

	CColRef2dArray *pdrspqdrspqcrPart = SPQOS_NEW(mp) CColRef2dArray(mp);

	const ULONG ulPartCols = pdrspqulPart->Size();
	SPQOS_ASSERT(0 < ulPartCols);

	for (ULONG ul = 0; ul < ulPartCols; ul++)
	{
		ULONG ulCol = *((*pdrspqulPart)[ul]);

		CColRef *colref = (*colref_array)[ulCol];
		CColRefArray *pdrspqcrCurr = SPQOS_NEW(mp) CColRefArray(mp);
		// The partition columns are not explicity referenced in the query but we
		// still need to mark them as used since they are required during partition
		// elimination
		colref->MarkAsUsed();
		pdrspqcrCurr->Append(colref);
		pdrspqdrspqcrPart->Append(pdrspqcrCurr);
	}

	return pdrspqdrspqcrPart;
}


//		Compute an order spec based on an index

COrderSpec *
CLogical::PosFromIndex(CMemoryPool *mp, const IMDIndex *pmdindex,
					   CColRefArray *colref_array,
					   const CTableDescriptor *ptabdesc)
{
	// compute the order spec after getting the current position of the index key
	// from the table descriptor. Index keys are relative to the
	// relation. So consider a case where we had 20 columns in a table. We
	// create an index that covers col # 20 as one of its keys. Then we drop
	// columns 10 through 15. Now the index key still points to col #20 but the
	// column ref list in colref_array will only have 15 elements in it.
	//

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);

	// GiST & GIN indexes have no order, so return an empty order spec
	if (pmdindex->IndexType() == IMDIndex::EmdindGist ||
		pmdindex->IndexType() == IMDIndex::EmdindGin)
		return pos;

	const ULONG ulLenKeys = pmdindex->Keys();

	// get relation from the metadata accessor using metadata id
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdesc->MDId());

	for (ULONG ul = 0; ul < ulLenKeys; ul++)
	{
		// This is the postion of the index key column relative to the relation
		const ULONG ulPosRel = pmdindex->KeyAt(ul);

		// get the column and it's attno from the relation
		const IMDColumn *pmdcol = pmdrel->GetMdCol(ulPosRel);
		INT attno = pmdcol->AttrNum();

		// get the position of the index key column relative to the table descriptor
		const ULONG ulPosTabDesc = ptabdesc->GetAttributePosition(attno);
		CColRef *colref = (*colref_array)[ulPosTabDesc];

		IMDId *mdid =
			colref->RetrieveType()->GetMdidForCmpType(IMDType::EcmptL);
		mdid->AddRef();

		// TODO:  March 27th 2012; we hard-code NULL treatment
		// need to revisit
		pos->Append(mdid, colref, COrderSpec::EntLast);
	}

	return pos;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveOutputPassThru
//
//	@doc:
//		Common case of output derivation for unary operators or operators
//		that pass through the schema of only the outer child
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDeriveOutputPassThru(CExpressionHandle &exprhdl)
{
	// may have additional children that are ignored, e.g., scalar children
	SPQOS_ASSERT(1 <= exprhdl.Arity());

	CColRefSet *pcrs = exprhdl.DeriveOutputColumns(0);
	pcrs->AddRef();

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveNotNullPassThruOuter
//
//	@doc:
//		Common case of deriving not null columns by passing through
//		not null columns from the outer child
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDeriveNotNullPassThruOuter(CExpressionHandle &exprhdl)
{
	// may have additional children that are ignored, e.g., scalar children
	SPQOS_ASSERT(1 <= exprhdl.Arity());

	CColRefSet *pcrs = exprhdl.DeriveNotNullColumns(0);
	pcrs->AddRef();

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveOutputCombineLogical
//
//	@doc:
//		Common case of output derivation by combining the schemas of all
//		children
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDeriveOutputCombineLogical(CMemoryPool *mp,
										 CExpressionHandle &exprhdl)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// union columns from the first N-1 children
	ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CColRefSet *pcrsChild = exprhdl.DeriveOutputColumns(ul);
		SPQOS_ASSERT(pcrs->IsDisjoint(pcrsChild) &&
					"Input columns are not disjoint");

		pcrs->Union(pcrsChild);
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveNotNullCombineLogical
//
//	@doc:
//		Common case of combining not null columns from all logical
//		children
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDeriveNotNullCombineLogical(CMemoryPool *mp,
										  CExpressionHandle &exprhdl)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	// union not nullable columns from the first N-1 children
	ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CColRefSet *pcrsChild = exprhdl.DeriveNotNullColumns(ul);
		SPQOS_ASSERT(pcrs->IsDisjoint(pcrsChild) &&
					"Input columns are not disjoint");

		pcrs->Union(pcrsChild);
	}

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpartinfoPassThruOuter
//
//	@doc:
//		Common case of common case of passing through partition consumer array
//
//---------------------------------------------------------------------------
CPartInfo *
CLogical::PpartinfoPassThruOuter(CExpressionHandle &exprhdl)
{
	CPartInfo *ppartinfo = exprhdl.DerivePartitionInfo(0);
	SPQOS_ASSERT(NULL != ppartinfo);
	ppartinfo->AddRef();
	return ppartinfo;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PkcCombineKeys
//
//	@doc:
//		Common case of combining keys from first n - 1 children
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogical::PkcCombineKeys(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity - 1; ul++)
	{
		CKeyCollection *pkc = exprhdl.DeriveKeyCollection(ul);
		if (NULL == pkc)
		{
			// if a child has no key, the operator has no key
			pcrs->Release();
			return NULL;
		}

		CColRefArray *colref_array = pkc->PdrspqcrKey(mp);
		pcrs->Include(colref_array);
		colref_array->Release();
	}

	return SPQOS_NEW(mp) CKeyCollection(mp, pcrs);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PkcKeysBaseTable
//
//	@doc:
//		Helper function for computing the keys in a base table
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogical::PkcKeysBaseTable(CMemoryPool *mp, const CBitSetArray *pdrspqbsKeys,
						   const CColRefArray *pdrspqcrOutput)
{
	const ULONG ulKeys = pdrspqbsKeys->Size();

	if (0 == ulKeys)
	{
		return NULL;
	}

	CKeyCollection *pkc = SPQOS_NEW(mp) CKeyCollection(mp);

	for (ULONG ul = 0; ul < ulKeys; ul++)
	{
		CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

		CBitSet *pbs = (*pdrspqbsKeys)[ul];
		CBitSetIter bsiter(*pbs);

		while (bsiter.Advance())
		{
			pcrs->Include((*pdrspqcrOutput)[bsiter.Bit()]);
		}

		pkc->Add(pcrs);
	}

	return pkc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpartinfoDeriveCombine
//
//	@doc:
//		Common case of combining partition info of all logical children
//
//---------------------------------------------------------------------------
CPartInfo *
CLogical::PpartinfoDeriveCombine(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	const ULONG arity = exprhdl.Arity();
	SPQOS_ASSERT(0 < arity);

	CPartInfo *ppartinfo = SPQOS_NEW(mp) CPartInfo(mp);

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CPartInfo *ppartinfoChild = NULL;
		if (exprhdl.FScalarChild(ul))
		{
			ppartinfoChild = exprhdl.DeriveScalarPartitionInfo(ul);
		}
		else
		{
			ppartinfoChild = exprhdl.DerivePartitionInfo(ul);
		}
		SPQOS_ASSERT(NULL != ppartinfoChild);
		CPartInfo *ppartinfoCombined =
			CPartInfo::PpartinfoCombine(mp, ppartinfo, ppartinfoChild);
		ppartinfo->Release();
		ppartinfo = ppartinfoCombined;
	}

	return ppartinfo;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveOuterReferences
//
//	@doc:
//		Derive outer references
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::DeriveOuterReferences(CMemoryPool *mp, CExpressionHandle &exprhdl,
								CColRefSet *pcrsUsedAdditional)
{
	ULONG arity = exprhdl.Arity();
	CColRefSet *outer_refs = SPQOS_NEW(mp) CColRefSet(mp);

	// collect output columns from relational children
	// and used columns from scalar children
	CColRefSet *pcrsOutput = SPQOS_NEW(mp) CColRefSet(mp);

	CColRefSet *pcrsUsed = SPQOS_NEW(mp) CColRefSet(mp);
	for (ULONG i = 0; i < arity; i++)
	{
		if (exprhdl.FScalarChild(i))
		{
			pcrsUsed->Union(exprhdl.DeriveUsedColumns(i));
		}
		else
		{
			// add outer references from relational children
			outer_refs->Union(exprhdl.DeriveOuterReferences(i));
			pcrsOutput->Union(exprhdl.DeriveOutputColumns(i));
		}
	}

	if (NULL != pcrsUsedAdditional)
	{
		pcrsUsed->Include(pcrsUsedAdditional);
	}

	// outer references are columns used by scalar child
	// but are not included in the output columns of relational children
	outer_refs->Union(pcrsUsed);
	outer_refs->Exclude(pcrsOutput);

	pcrsOutput->Release();
	pcrsUsed->Release();
	return outer_refs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveOuterIndexGet
//
//	@doc:
//		Derive outer references for index get and dynamic index get operators
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDeriveOuterIndexGet(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	ULONG arity = exprhdl.Arity();
	CColRefSet *outer_refs = SPQOS_NEW(mp) CColRefSet(mp);

	CColRefSet *pcrsOutput = DeriveOutputColumns(mp, exprhdl);

	CColRefSet *pcrsUsed = SPQOS_NEW(mp) CColRefSet(mp);
	for (ULONG i = 0; i < arity; i++)
	{
		SPQOS_ASSERT(exprhdl.FScalarChild(i));
		pcrsUsed->Union(exprhdl.DeriveUsedColumns(i));
	}

	// outer references are columns used by scalar children
	// but are not included in the output columns of relational children
	outer_refs->Union(pcrsUsed);
	outer_refs->Exclude(pcrsOutput);

	pcrsOutput->Release();
	pcrsUsed->Release();
	return outer_refs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDeriveCorrelatedApply
//
//	@doc:
//		Derive columns from the inner child of a correlated-apply expression
//		that can be used above the apply expression
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::DeriveCorrelatedApplyColumns(CMemoryPool *mp,
									   CExpressionHandle &exprhdl) const
{
	SPQOS_ASSERT(this == exprhdl.Pop());

	ULONG arity = exprhdl.Arity();
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	if (CUtils::FCorrelatedApply(exprhdl.Pop()))
	{
		// add inner columns of correlated-apply expression
		pcrs->Include(CLogicalApply::PopConvert(exprhdl.Pop())->PdrgPcrInner());
	}

	// combine correlated-apply columns from logical children
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!exprhdl.FScalarChild(ul))
		{
			CDrvdPropRelational *pdprel = exprhdl.GetRelationalProperties(ul);
			pcrs->Union(pdprel->GetCorrelatedApplyColumns());
		}
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PkcDeriveKeysPassThru
//
//	@doc:
//		Addref and return keys of n-th child
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogical::PkcDeriveKeysPassThru(CExpressionHandle &exprhdl, ULONG ulChild)
{
	CKeyCollection *pkcLeft =
		exprhdl.GetRelationalProperties(ulChild)->GetKeyCollection();

	// key collection may be NULL
	if (NULL != pkcLeft)
	{
		pkcLeft->AddRef();
	}

	return pkcLeft;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PkcDeriveKeys
//
//	@doc:
//		Derive key collections
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogical::DeriveKeyCollection(CMemoryPool *,	   // mp
							  CExpressionHandle &  // exprhdl
) const
{
	// no keys found by default
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpcDeriveConstraintFromPredicates
//
//	@doc:
//		Derive constraint property when expression has relational children and
//		scalar children (predicates)
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogical::PpcDeriveConstraintFromPredicates(CMemoryPool *mp,
											CExpressionHandle &exprhdl)
{
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	// collect constraint properties from relational children
	// and predicates from scalar children
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (exprhdl.FScalarChild(ul))
		{
			CExpression *pexprScalar = exprhdl.PexprScalarExactChild(ul);

			// make sure it is a predicate... boolop, cmp, nulltest,
			// or a list of join predicates for an NAry join
			if (NULL == pexprScalar || !CUtils::FPredicate(pexprScalar))
			{
				continue;
			}
			SPQOS_ASSERT(COperator::EopScalarNAryJoinPredList !=
						pexprScalar->Pop()->Eopid());
			CColRefSetArray *pdrspqcrsChild = NULL;
			CConstraint *pcnstr = CConstraint::PcnstrFromScalarExpr(
				mp, pexprScalar, &pdrspqcrsChild);

			if (NULL != pcnstr)
			{
				pdrspqcnstr->Append(pcnstr);

				// merge with the equivalence classes we have so far
				CColRefSetArray *pdrspqcrsMerged =
					CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs,
													  pdrspqcrsChild);
				pdrspqcrs->Release();
				pdrspqcrs = pdrspqcrsMerged;
			}
			CRefCount::SafeRelease(pdrspqcrsChild);
		}
		else
		{
			CPropConstraint *ppc = exprhdl.DerivePropertyConstraint(ul);

			// equivalence classes coming from child
			CColRefSetArray *pdrspqcrsChild = ppc->PdrspqcrsEquivClasses();

			// merge with the equivalence classes we have so far
			CColRefSetArray *pdrspqcrsMerged =
				CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsChild);
			pdrspqcrs->Release();
			pdrspqcrs = pdrspqcrsMerged;

			// constraint coming from child
			CConstraint *pcnstr = ppc->Pcnstr();
			if (NULL != pcnstr)
			{
				pcnstr->AddRef();
				pdrspqcnstr->Append(pcnstr);
			}
		}
	}

	CConstraint *pcnstrNew = CConstraint::PcnstrConjunction(mp, pdrspqcnstr);

	return SPQOS_NEW(mp) CPropConstraint(mp, pdrspqcrs, pcnstrNew);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpcDeriveConstraintFromTable
//
//	@doc:
//		Derive constraint property from a table/index get
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogical::PpcDeriveConstraintFromTable(CMemoryPool *mp,
									   const CTableDescriptor *ptabdesc,
									   const CColRefArray *pdrspqcrOutput)
{
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	const CColumnDescriptorArray *pdrspqcoldesc = ptabdesc->Pdrspqcoldesc();
	const ULONG num_cols = pdrspqcoldesc->Size();

	CColRefArray *pdrspqcrNonSystem = SPQOS_NEW(mp) CColRefArray(mp);

	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		CColRef *colref = (*pdrspqcrOutput)[ul];
		// we are only interested in non-system columns that are defined as
		// being NOT NULL

		if (pcoldesc->IsSystemColumn())
		{
			continue;
		}

		pdrspqcrNonSystem->Append(colref);

		if (pcoldesc->IsNullable() || colref->GetUsage() == CColRef::EUnused)
		{
			continue;
		}

		// add a "not null" constraint and an equivalence class
		CConstraint *pcnstr = CConstraintInterval::PciUnbounded(
			mp, colref, false /*fIncludesNull*/);

		if (pcnstr == NULL)
		{
			continue;
		}
		pdrspqcnstr->Append(pcnstr);

		CColRefSet *pcrsEquiv = SPQOS_NEW(mp) CColRefSet(mp);
		pcrsEquiv->Include(colref);
		pdrspqcrs->Append(pcrsEquiv);
	}

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(ptabdesc->MDId());

	const ULONG ulCheckConstraint = pmdrel->CheckConstraintCount();
	for (ULONG ul = 0; ul < ulCheckConstraint; ul++)
	{
		IMDId *pmdidCheckConstraint = pmdrel->CheckConstraintMDidAt(ul);

		const IMDCheckConstraint *pmdCheckConstraint =
			md_accessor->RetrieveCheckConstraints(pmdidCheckConstraint);

		// extract the check constraint expression
		CExpression *pexprCheckConstraint =
			pmdCheckConstraint->GetCheckConstraintExpr(mp, md_accessor,
													   pdrspqcrNonSystem);
		SPQOS_ASSERT(NULL != pexprCheckConstraint);
		SPQOS_ASSERT(CUtils::FPredicate(pexprCheckConstraint));

		CColRefSetArray *pdrspqcrsChild = NULL;

		// Check constraints are satisfied if the check expression evaluates to
		// true or NULL, so infer NULLs as true here.
		CConstraint *pcnstr = CConstraint::PcnstrFromScalarExpr(
			mp, pexprCheckConstraint, &pdrspqcrsChild,
			true /* infer_nulls_as */);
		if (NULL != pcnstr)
		{
			pdrspqcnstr->Append(pcnstr);

			// merge with the equivalence classes we have so far
			CColRefSetArray *pdrspqcrsMerged =
				CUtils::PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsChild);
			pdrspqcrs->Release();
			pdrspqcrs = pdrspqcrsMerged;
		}
		CRefCount::SafeRelease(pdrspqcrsChild);
		pexprCheckConstraint->Release();
	}

	pdrspqcrNonSystem->Release();

	return SPQOS_NEW(mp) CPropConstraint(
		mp, pdrspqcrs, CConstraint::PcnstrConjunction(mp, pdrspqcnstr));
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpcDeriveConstraintFromTableWithPredicates
//
//	@doc:
//		Derive constraint property from a table/index get with predicates
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogical::PpcDeriveConstraintFromTableWithPredicates(
	CMemoryPool *mp, CExpressionHandle &exprhdl,
	const CTableDescriptor *ptabdesc, const CColRefArray *pdrspqcrOutput)
{
	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);
	CPropConstraint *ppcTable =
		PpcDeriveConstraintFromTable(mp, ptabdesc, pdrspqcrOutput);
	CConstraint *pcnstrTable = ppcTable->Pcnstr();
	if (NULL != pcnstrTable)
	{
		pcnstrTable->AddRef();
		pdrspqcnstr->Append(pcnstrTable);
	}
	CColRefSetArray *pdrspqcrsEquivClassesTable =
		ppcTable->PdrspqcrsEquivClasses();

	CPropConstraint *ppcnstrCond =
		PpcDeriveConstraintFromPredicates(mp, exprhdl);
	CConstraint *pcnstrCond = ppcnstrCond->Pcnstr();

	if (NULL != pcnstrCond)
	{
		pcnstrCond->AddRef();
		pdrspqcnstr->Append(pcnstrCond);
	}
	else if (NULL == pcnstrTable)
	{
		ppcTable->Release();
		pdrspqcnstr->Release();

		return ppcnstrCond;
	}

	CColRefSetArray *pdrspqcrsCond = ppcnstrCond->PdrspqcrsEquivClasses();
	CColRefSetArray *pdrspqcrs = CUtils::PdrspqcrsMergeEquivClasses(
		mp, pdrspqcrsEquivClassesTable, pdrspqcrsCond);
	CPropConstraint *ppc = SPQOS_NEW(mp) CPropConstraint(
		mp, pdrspqcrs, CConstraint::PcnstrConjunction(mp, pdrspqcnstr));

	ppcnstrCond->Release();
	ppcTable->Release();

	return ppc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpcDeriveConstraintPassThru
//
//	@doc:
//		Shorthand to addref and pass through constraint from a given child
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogical::PpcDeriveConstraintPassThru(CExpressionHandle &exprhdl, ULONG ulChild)
{
	// return constraint property of child
	CPropConstraint *ppc = exprhdl.DerivePropertyConstraint(ulChild);
	if (NULL != ppc)
	{
		ppc->AddRef();
	}
	return ppc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PpcDeriveConstraintRestrict
//
//	@doc:
//		Derive constraint property only on the given columns
//
//---------------------------------------------------------------------------
CPropConstraint *
CLogical::PpcDeriveConstraintRestrict(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CColRefSet *pcrsOutput)
{
	// constraint property from relational child
	CPropConstraint *ppc = exprhdl.DerivePropertyConstraint(0);
	CColRefSetArray *pdrspqcrs = ppc->PdrspqcrsEquivClasses();

	// construct new array of equivalence classes
	CColRefSetArray *pdrspqcrsNew = SPQOS_NEW(mp) CColRefSetArray(mp);

	const ULONG length = pdrspqcrs->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefSet *pcrsEquiv = SPQOS_NEW(mp) CColRefSet(mp);
		pcrsEquiv->Include((*pdrspqcrs)[ul]);
		pcrsEquiv->Intersection(pcrsOutput);

		if (0 < pcrsEquiv->Size())
		{
			pdrspqcrsNew->Append(pcrsEquiv);
		}
		else
		{
			pcrsEquiv->Release();
		}
	}

	CConstraint *pcnstrChild = ppc->Pcnstr();
	if (NULL == pcnstrChild)
	{
		return SPQOS_NEW(mp) CPropConstraint(mp, pdrspqcrsNew, NULL);
	}

	CConstraintArray *pdrspqcnstr = SPQOS_NEW(mp) CConstraintArray(mp);

	// include only constraints on given columns
	CColRefSetIter crsi(*pcrsOutput);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CConstraint *pcnstrCol = pcnstrChild->Pcnstr(mp, colref);
		if (NULL == pcnstrCol)
		{
			continue;
		}

		if (pcnstrCol->IsConstraintUnbounded())
		{
			pcnstrCol->Release();
			continue;
		}

		pdrspqcnstr->Append(pcnstrCol);
	}

	CConstraint *pcnstr = CConstraint::PcnstrConjunction(mp, pdrspqcnstr);

	return SPQOS_NEW(mp) CPropConstraint(mp, pdrspqcrsNew, pcnstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveFunctionProperties
//
//	@doc:
//		Derive function properties
//
//---------------------------------------------------------------------------
CFunctionProp *
CLogical::DeriveFunctionProperties(CMemoryPool *mp,
								   CExpressionHandle &exprhdl) const
{
	IMDFunction::EFuncStbl efs =
		EfsDeriveFromChildren(exprhdl, IMDFunction::EfsImmutable);

	return SPQOS_NEW(mp) CFunctionProp(
		efs, exprhdl.FChildrenHaveVolatileFuncScan(), false /*fScan*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveTableDescriptor
//
//	@doc:
//		Derive table descriptor for tables used by operator
//
//---------------------------------------------------------------------------
CTableDescriptor *
CLogical::DeriveTableDescriptor(CMemoryPool *, CExpressionHandle &) const
{
	//currently return null unless there is a single table being used. Later we may want
	//to make this return a set of table descriptors and pass them up all operators
	return NULL;
}
//---------------------------------------------------------------------------
//	@function:
//		CLogical::PfpDeriveFromScalar
//
//	@doc:
//		Derive function properties using data access property of scalar child
//
//---------------------------------------------------------------------------
CFunctionProp *
CLogical::PfpDeriveFromScalar(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	SPQOS_CHECK_ABORT;

	// collect stability from all children
	IMDFunction::EFuncStbl efs =
		EfsDeriveFromChildren(exprhdl, IMDFunction::EfsImmutable);

	return SPQOS_NEW(mp) CFunctionProp(
		efs, exprhdl.FChildrenHaveVolatileFuncScan(), false /*fScan*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogical::DeriveMaxCard(CMemoryPool *,		 // mp
						CExpressionHandle &	 // exprhdl
) const
{
	// unbounded by default
	return CMaxCard();
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveJoinDepth
//
//	@doc:
//		Derive join depth
//
//---------------------------------------------------------------------------
ULONG
CLogical::DeriveJoinDepth(CMemoryPool *,  // mp
						  CExpressionHandle &exprhdl) const
{
	const ULONG arity = exprhdl.Arity();

	// sum-up join depth of all relational children
	ULONG ulDepth = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!exprhdl.FScalarChild(ul))
		{
			ulDepth = ulDepth + exprhdl.DeriveJoinDepth(ul);
		}
	}

	return ulDepth;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::MaxcardDef
//
//	@doc:
//		Default max card for join and apply operators
//
//---------------------------------------------------------------------------
CMaxCard
CLogical::MaxcardDef(CExpressionHandle &exprhdl)
{
	const ULONG arity = exprhdl.Arity();

	CMaxCard maxcard = exprhdl.DeriveMaxCard(0);
	for (ULONG ul = 1; ul < arity - 1; ul++)
	{
		if (!exprhdl.FScalarChild(ul))
		{
			maxcard *= exprhdl.DeriveMaxCard(ul);
		}
	}

	return maxcard;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::DeriveMaxCard
//
//	@doc:
//		Derive max card given scalar child and constraint property. If a
//		contradiction is detected then return maxcard of zero, otherwise
//		use the given default maxcard
//
//---------------------------------------------------------------------------
CMaxCard
CLogical::Maxcard(CExpressionHandle &exprhdl, ULONG ulScalarIndex,
				  CMaxCard maxcard)
{
	// in case of a false condition (when the operator is not Full / Left Outer Join) or a contradiction, maxcard should be zero
	CExpression *pexprScalar = exprhdl.PexprScalarExactChild(ulScalarIndex);

	if (NULL != pexprScalar)
	{
		if (COperator::EopScalarNAryJoinPredList == pexprScalar->Pop()->Eopid())
		{
			// look at the inner join predicates only
			pexprScalar = (*pexprScalar)[0];
		}

		if ((CUtils::FScalarConstFalse(pexprScalar) &&
			 COperator::EopLogicalFullOuterJoin != exprhdl.Pop()->Eopid() &&
			 COperator::EopLogicalLeftOuterJoin != exprhdl.Pop()->Eopid() &&
			 COperator::EopLogicalRightOuterJoin != exprhdl.Pop()->Eopid()) ||
			exprhdl.DerivePropertyConstraint()->FContradiction())
		{
			return CMaxCard(0 /*ull*/);
		}
	}

	return maxcard;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsReqdChildStats
//
//	@doc:
//		Helper for compute required stat columns of the n-th child
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsReqdChildStats(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsInput,
	CColRefSet *pcrsUsed,  // columns used by scalar child(ren)
	ULONG child_index)
{
	SPQOS_ASSERT(child_index < exprhdl.Arity() - 1);
	SPQOS_CHECK_ABORT;

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Union(pcrsInput);
	pcrs->Union(pcrsUsed);

	// intersect with the output columns of relational child
	pcrs->Intersection(exprhdl.DeriveOutputColumns(child_index));

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsStatsPassThru
//
//	@doc:
//		Helper for common case of passing through required stat columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsStatsPassThru(CColRefSet *pcrsInput)
{
	SPQOS_ASSERT(NULL != pcrsInput);
	SPQOS_CHECK_ABORT;

	pcrsInput->AddRef();
	return pcrsInput;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PstatsPassThruOuter
//
//	@doc:
//		Helper for common case of passing through derived stats
//
//---------------------------------------------------------------------------
IStatistics *
CLogical::PstatsPassThruOuter(CExpressionHandle &exprhdl)
{
	SPQOS_CHECK_ABORT;

	IStatistics *stats = exprhdl.Pstats(0);
	stats->AddRef();

	return stats;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PstatsBaseTable
//
//	@doc:
//		Helper for deriving statistics on a base table
//
//---------------------------------------------------------------------------
IStatistics *
CLogical::PstatsBaseTable(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CTableDescriptor *ptabdesc,
	CColRefSet *
		pcrsHistExtra  // additional columns required for stats, not required by parent
)
{
	CReqdPropRelational *prprel =
		CReqdPropRelational::GetReqdRelationalProps(exprhdl.Prp());
	CColRefSet *pcrsHist = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsHist->Include(prprel->PcrsStat());
	if (NULL != pcrsHistExtra)
	{
		pcrsHist->Include(pcrsHistExtra);
	}

	CColRefSet *pcrsOutput = exprhdl.DeriveOutputColumns();
	CColRefSet *pcrsWidth = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsWidth->Include(pcrsOutput);
	pcrsWidth->Exclude(pcrsHist);

	const COptCtxt *poctxt = COptCtxt::PoctxtFromTLS();
	CMDAccessor *md_accessor = poctxt->Pmda();
	CStatisticsConfig *stats_config =
		poctxt->GetOptimizerConfig()->GetStatsConf();

	IStatistics *stats = md_accessor->Pstats(mp, ptabdesc->MDId(), pcrsHist,
											 pcrsWidth, stats_config);

	// clean up
	pcrsWidth->Release();
	pcrsHist->Release();

	return stats;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PstatsDeriveDummy
//
//	@doc:
//		Derive dummy statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogical::PstatsDeriveDummy(CMemoryPool *mp, CExpressionHandle &exprhdl,
							CDouble rows) const
{
	SPQOS_CHECK_ABORT;

	// return a dummy stats object that has a histogram for every
	// required-stats column
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);
	CReqdPropRelational *prprel =
		CReqdPropRelational::GetReqdRelationalProps(exprhdl.Prp());
	CColRefSet *pcrs = prprel->PcrsStat();
	ULongPtrArray *colids = SPQOS_NEW(mp) ULongPtrArray(mp);
	pcrs->ExtractColIds(mp, colids);

	IStatistics *stats = CStatistics::MakeDummyStats(mp, colids, rows);

	// clean up
	colids->Release();

	return stats;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PexprPartPred
//
//	@doc:
//		Compute partition predicate to pass down to n-th child
//
//---------------------------------------------------------------------------
CExpression *
CLogical::PexprPartPred(CMemoryPool *,		  //mp,
						CExpressionHandle &,  //exprhdl,
						CExpression *,		  //pexprInput,
						ULONG				  //child_index
) const
{
	SPQOS_CHECK_ABORT;

	// the default behavior is to never pass down any partition predicates
	return NULL;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PdpCreate
//
//	@doc:
//		Create base container of derived properties
//
//---------------------------------------------------------------------------
CDrvdProp *
CLogical::PdpCreate(CMemoryPool *mp) const
{
	return SPQOS_NEW(mp) CDrvdPropRelational(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PrpCreate
//
//	@doc:
//		Create base container of required properties
//
//---------------------------------------------------------------------------
CReqdProp *
CLogical::PrpCreate(CMemoryPool *mp) const
{
	return SPQOS_NEW(mp) CReqdPropRelational();
}


//---------------------------------------------------------------------------
//	@function:
//		CLogical::PtabdescFromTableGet
//
//	@doc:
//		Returns the table descriptor for (Dynamic)(BitmapTable)Get operators
//
//---------------------------------------------------------------------------
CTableDescriptor *
CLogical::PtabdescFromTableGet(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);
	switch (pop->Eopid())
	{
		case CLogical::EopLogicalGet:
			return CLogicalGet::PopConvert(pop)->Ptabdesc();
		case CLogical::EopLogicalDynamicGet:
			return CLogicalDynamicGet::PopConvert(pop)->Ptabdesc();
		case CLogical::EopLogicalBitmapTableGet:
			return CLogicalBitmapTableGet::PopConvert(pop)->Ptabdesc();
		case CLogical::EopLogicalDynamicBitmapTableGet:
			return CLogicalDynamicBitmapTableGet::PopConvert(pop)->Ptabdesc();
		case CLogical::EopLogicalSelect:
			return CLogicalSelect::PopConvert(pop)->Ptabdesc();
		default:
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PdrspqcrOutputFromLogicalGet
//
//	@doc:
//		Extract the output columns from a logical get or dynamic get operator
//
//---------------------------------------------------------------------------
CColRefArray *
CLogical::PdrspqcrOutputFromLogicalGet(CLogical *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(COperator::EopLogicalGet == pop->Eopid() ||
				COperator::EopLogicalDynamicGet == pop->Eopid());

	if (COperator::EopLogicalGet == pop->Eopid())
	{
		return CLogicalGet::PopConvert(pop)->PdrspqcrOutput();
	}

	return CLogicalDynamicGet::PopConvert(pop)->PdrspqcrOutput();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::NameFromLogicalGet
//
//	@doc:
//		Extract the name from a logical get or dynamic get operator
//
//---------------------------------------------------------------------------
const CName &
CLogical::NameFromLogicalGet(CLogical *pop)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(COperator::EopLogicalGet == pop->Eopid() ||
				COperator::EopLogicalDynamicGet == pop->Eopid());

	if (COperator::EopLogicalGet == pop->Eopid())
	{
		return CLogicalGet::PopConvert(pop)->Name();
	}

	return CLogicalDynamicGet::PopConvert(pop)->Name();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogical::PcrsDist
//
//	@doc:
//		Return the set of distribution columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogical::PcrsDist(CMemoryPool *mp, const CTableDescriptor *ptabdesc,
				   const CColRefArray *pdrspqcrOutput)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	const CColumnDescriptorArray *pdrspqcoldesc = ptabdesc->Pdrspqcoldesc();
	const CColumnDescriptorArray *pdrspqcoldescDist =
		ptabdesc->PdrspqcoldescDist();
	SPQOS_ASSERT(NULL != pdrspqcoldesc);
	SPQOS_ASSERT(NULL != pdrspqcoldescDist);
	SPQOS_ASSERT(pdrspqcrOutput->Size() == pdrspqcoldesc->Size());


	// mapping base table columns to corresponding column references
	IntToColRefMap *phmicr = SPQOS_NEW(mp) IntToColRefMap(mp);
	const ULONG num_cols = pdrspqcoldesc->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		CColRef *colref = (*pdrspqcrOutput)[ul];

		phmicr->Insert(SPQOS_NEW(mp) INT(pcoldesc->AttrNum()), colref);
	}

	CColRefSet *pcrsDist = SPQOS_NEW(mp) CColRefSet(mp);
	const ULONG ulDistCols = pdrspqcoldescDist->Size();
	for (ULONG ul2 = 0; ul2 < ulDistCols; ul2++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldescDist)[ul2];
		const INT attno = pcoldesc->AttrNum();
		CColRef *pcrMapped = phmicr->Find(&attno);
		SPQOS_ASSERT(NULL != pcrMapped);
		pcrsDist->Include(pcrMapped);
	}

	// clean up
	phmicr->Release();

	return pcrsDist;
}

// EOF
