//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarArrayCmp.cpp
//
//	@doc:
//		Implementation of scalar array comparison operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarArrayCmp.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CPredicateUtils.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

const CHAR CScalarArrayCmp::m_rgszCmpType[EarrcmpSentinel][10] = {"Any", "All"};


//---------------------------------------------------------------------------
//	@function:
//		CScalarOp::CScalarArrayCmp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarArrayCmp::CScalarArrayCmp(CMemoryPool *mp, IMDId *mdid_op,
								 const CWStringConst *pstrOp,
								 EArrCmpType earrcmpt)
	: CScalar(mp),
	  m_mdid_op(mdid_op),
	  m_pscOp(pstrOp),
	  m_earrccmpt(earrcmpt),
	  m_returns_null_on_null_input(false)
{
	SPQOS_ASSERT(mdid_op->IsValid());
	SPQOS_ASSERT(EarrcmpSentinel > earrcmpt);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_returns_null_on_null_input =
		CMDAccessorUtils::FScalarOpReturnsNullOnNullInput(md_accessor,
														  m_mdid_op);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::GetMDName
//
//	@doc:
//		Comparison operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CScalarArrayCmp::Pstr() const
{
	return m_pscOp;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::MdIdOp
//
//	@doc:
//		Comparison operator mdid
//
//---------------------------------------------------------------------------
IMDId *
CScalarArrayCmp::MdIdOp() const
{
	return m_mdid_op;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		metadata id
//
//---------------------------------------------------------------------------
ULONG
CScalarArrayCmp::HashValue() const
{
	return spqos::CombineHashes(
		spqos::CombineHashes(COperator::HashValue(), m_mdid_op->HashValue()),
		m_earrccmpt);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarArrayCmp::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarArrayCmp *popCmp = CScalarArrayCmp::PopConvert(pop);

		// match if operator oid are identical
		return m_earrccmpt == popCmp->Earrcmpt() &&
			   m_mdid_op->Equals(popCmp->MdIdOp());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::MdidType
//
//	@doc:
//		Expression type
//
//---------------------------------------------------------------------------
IMDId *
CScalarArrayCmp::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	return md_accessor->PtMDType<IMDTypeBool>()->MDId();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarArrayCmp::Eber(ULongPtrArray *pdrspqulChildren) const
{
	if (m_returns_null_on_null_input)
	{
		return EberNullOnAnyNullChild(pdrspqulChildren);
	}

	return EberAny;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarArrayCmp::OsPrint(IOstream &os) const
{
	os << SzId() << " " << m_rgszCmpType[m_earrccmpt] << " (";
	os << Pstr()->GetBuffer();
	os << ")";

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayCmp::PexprExpand
//
//	@doc:
//		Expand array comparison expression into a conjunctive/disjunctive
//		expression
//
//---------------------------------------------------------------------------
CExpression *
CScalarArrayCmp::PexprExpand(CMemoryPool *mp, CExpression *pexprArrayCmp)
{
	SPQOS_ASSERT(NULL != pexprArrayCmp);
	SPQOS_ASSERT(EopScalarArrayCmp == pexprArrayCmp->Pop()->Eopid());

	COptimizerConfig *optimizer_config =
		COptCtxt::PoctxtFromTLS()->GetOptimizerConfig();
	ULONG array_expansion_threshold =
		optimizer_config->GetHint()->UlArrayExpansionThreshold();
	CExpression *pexprIdent = (*pexprArrayCmp)[0];
	CExpression *pexprArray = CUtils::PexprScalarArrayChild(pexprArrayCmp);
	CScalarArrayCmp *popArrayCmp =
		CScalarArrayCmp::PopConvert(pexprArrayCmp->Pop());
	ULONG ulArrayElems = 0;

	if (CUtils::FScalarArray(pexprArray))
	{
		ulArrayElems = CUtils::UlScalarArrayArity(pexprArray);
	}

	// if this condition is true, we know the right child of ArrayCmp is a constant.
	if (0 == ulArrayElems || ulArrayElems > array_expansion_threshold)
	{
		// if right child is not an actual array (e.g., Const of type array), return input
		// expression without expansion
		pexprArrayCmp->AddRef();
		return pexprArrayCmp;
	}

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < ulArrayElems; ul++)
	{
		CExpression *pexprArrayElem =
			CUtils::PScalarArrayExprChildAt(mp, pexprArray, ul);
		pexprIdent->AddRef();
		const CWStringConst *str_opname = popArrayCmp->Pstr();
		IMDId *mdid_op = popArrayCmp->MdIdOp();
		SPQOS_ASSERT(IMDId::IsValid(mdid_op));

		mdid_op->AddRef();

		CExpression *pexprCmp = CUtils::PexprScalarCmp(
			mp, pexprIdent, pexprArrayElem, *str_opname, mdid_op);
		pdrspqexpr->Append(pexprCmp);
	}
	SPQOS_ASSERT(0 < pdrspqexpr->Size());

	// deduplicate resulting array
	CExpressionArray *pdrspqexprDeduped = CUtils::PdrspqexprDedup(mp, pdrspqexpr);
	pdrspqexpr->Release();

	EArrCmpType earrcmpt = popArrayCmp->Earrcmpt();
	if (EarrcmpAny == earrcmpt)
	{
		return CPredicateUtils::PexprDisjunction(mp, pdrspqexprDeduped);
	}
	SPQOS_ASSERT(EarrcmpAll == earrcmpt);

	return CPredicateUtils::PexprConjunction(mp, pdrspqexprDeduped);
}



// EOF
