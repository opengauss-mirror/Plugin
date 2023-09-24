//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CUtils.cpp
//
//	@doc:
//		Implementation of general utility functions
//---------------------------------------------------------------------------
#include "spqopt/base/CUtils.h"

#include "spqos/common/clibwrapper.h"
#include "spqos/common/syslibwrapper.h"
#include "spqos/io/CFileDescriptor.h"
#include "spqos/io/COstreamString.h"
#include "spqos/memory/CAutoMemoryPool.h"
#include "spqos/string/CWStringDynamic.h"
#include "spqos/task/CWorker.h"

#include "spqopt/base/CCastUtils.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CConstraintInterval.h"
#include "spqopt/base/CDistributionSpecRandom.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/CPartIndexMap.h"
#include "spqopt/exception.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionPreprocessor.h"
#include "spqopt/operators/CLogicalCTEConsumer.h"
#include "spqopt/operators/CLogicalCTEProducer.h"
#include "spqopt/operators/CPhysicalMotionRandom.h"
#include "spqopt/optimizer/COptimizerConfig.h"
#include "spqopt/search/CMemo.h"
#include "spqopt/translate/CTranslatorExprToDXLUtils.h"
#include "naucrates/base/IDatumBool.h"
#include "naucrates/base/IDatumInt2.h"
#include "naucrates/base/IDatumInt4.h"
#include "naucrates/base/IDatumInt8.h"
#include "naucrates/base/IDatumOid.h"
#include "naucrates/exception.h"
#include "naucrates/md/CMDArrayCoerceCastSPQDB.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/CMDIdScCmp.h"
#include "naucrates/md/IMDAggregate.h"
#include "naucrates/md/IMDCast.h"
#include "naucrates/md/IMDScCmp.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/md/IMDTypeBool.h"
#include "naucrates/md/IMDTypeGeneric.h"
#include "naucrates/md/IMDTypeInt2.h"
#include "naucrates/md/IMDTypeInt4.h"
#include "naucrates/md/IMDTypeInt8.h"
#include "naucrates/md/IMDTypeOid.h"
#include "naucrates/traceflags/traceflags.h"

using namespace spqopt;
using namespace spqmd;

#ifdef SPQOS_DEBUG

// buffer of 16M characters for print wrapper
const ULONG ulBufferCapacity = 16 * 1024 * 1024;
static WCHAR wszBuffer[ulBufferCapacity];

// global wrapper for debug print of expression
void
PrintExpr(void *pv)
{
	spqopt::CUtils::PrintExpression((spqopt::CExpression *) pv);
}

// debug print expression
void
CUtils::PrintExpression(CExpression *pexpr)
{
	CWStringStatic str(wszBuffer, ulBufferCapacity);
	COstreamString oss(&str);

	if (NULL == pexpr)
	{
		oss << std::endl << "(null)" << std::endl;
	}
	else
	{
		oss << std::endl << pexpr << std::endl << *pexpr << std::endl;
	}

	SPQOS_TRACE(str.GetBuffer());
	str.Reset();
}

// global wrapper for debug print of memo
void
PrintMemo(void *pv)
{
	spqopt::CUtils::PrintMemo((spqopt::CMemo *) pv);
}

// debug print Memo structure
void
CUtils::PrintMemo(CMemo *pmemo)
{
	CWStringStatic str(wszBuffer, ulBufferCapacity);
	COstreamString oss(&str);

	if (NULL == pmemo)
	{
		oss << std::endl << "(null)" << std::endl;
	}
	else
	{
		oss << std::endl << pmemo << std::endl;
		(void) pmemo->OsPrint(oss);
		oss << std::endl;
	}

	SPQOS_TRACE(str.GetBuffer());
	str.Reset();
}

#endif	// SPQOS_DEBUG

// helper function to print a column descriptor array
IOstream &
CUtils::OsPrintDrgPcoldesc(IOstream &os, CColumnDescriptorArray *pdrspqcoldesc,
						   ULONG ulLengthMax)
{
	ULONG length = std::min(pdrspqcoldesc->Size(), ulLengthMax);
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColumnDescriptor *pcoldesc = (*pdrspqcoldesc)[ul];
		pcoldesc->OsPrint(os);

		// separator
		os << (ul == length - 1 ? "" : ", ");
	}

	return os;
}


// generate a ScalarIdent expression
CExpression *
CUtils::PexprScalarIdent(CMemoryPool *mp, const CColRef *colref)
{
	SPQOS_ASSERT(NULL != colref);

	return SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarIdent(mp, colref));
}

// generate a ScalarProjectElement expression
CExpression *
CUtils::PexprScalarProjectElement(CMemoryPool *mp, CColRef *colref,
								  CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(NULL != pexpr);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectElement(mp, colref), pexpr);
}

// generate a comparison expression over two columns
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, const CColRef *pcrLeft,
					   const CColRef *pcrRight, CWStringConst strOp,
					   IMDId *mdid_op)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pcrRight);

	IMDType::ECmpType cmp_type = ParseCmpType(mdid_op);
	if (IMDType::EcmptOther != cmp_type)
	{
		IMDId *left_mdid = pcrLeft->RetrieveType()->MDId();
		IMDId *right_mdid = pcrRight->RetrieveType()->MDId();

		if (CMDAccessorUtils::FCmpOrCastedCmpExists(left_mdid, right_mdid,
													cmp_type))
		{
			CExpression *pexprScCmp =
				PexprScalarCmp(mp, pcrLeft, pcrRight, cmp_type);
			mdid_op->Release();

			return pexprScCmp;
		}
	}

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, mdid_op, SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer()),
			ParseCmpType(mdid_op)),
		PexprScalarIdent(mp, pcrLeft), PexprScalarIdent(mp, pcrRight));
}

// Generate a comparison expression between a column and an expression
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, const CColRef *pcrLeft,
					   CExpression *pexprRight, CWStringConst strOp,
					   IMDId *mdid_op)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	IMDType::ECmpType cmp_type = ParseCmpType(mdid_op);
	if (IMDType::EcmptOther != cmp_type)
	{
		IMDId *left_mdid = pcrLeft->RetrieveType()->MDId();
		IMDId *right_mdid = CScalar::PopConvert(pexprRight->Pop())->MdidType();

		if (CMDAccessorUtils::FCmpOrCastedCmpExists(left_mdid, right_mdid,
													cmp_type))
		{
			CExpression *pexprScCmp =
				PexprScalarCmp(mp, pcrLeft, pexprRight, cmp_type);
			mdid_op->Release();

			return pexprScCmp;
		}
	}

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, mdid_op, SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer()),
			ParseCmpType(mdid_op)),
		PexprScalarIdent(mp, pcrLeft), pexprRight);
}

// Generate a comparison expression between two columns
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, const CColRef *pcrLeft,
					   const CColRef *pcrRight, IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pcrRight);
	SPQOS_ASSERT(IMDType::EcmptOther > cmp_type);

	CExpression *pexprLeft = PexprScalarIdent(mp, pcrLeft);
	CExpression *pexprRight = PexprScalarIdent(mp, pcrRight);

	return PexprScalarCmp(mp, pexprLeft, pexprRight, cmp_type);
}

// Generate a comparison expression over a column and an expression
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, const CColRef *pcrLeft,
					   CExpression *pexprRight, IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pexprRight);
	SPQOS_ASSERT(IMDType::EcmptOther > cmp_type);

	CExpression *pexprLeft = PexprScalarIdent(mp, pcrLeft);

	return PexprScalarCmp(mp, pexprLeft, pexprRight, cmp_type);
}

// Generate a comparison expression between an expression and a column
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, CExpression *pexprLeft,
					   const CColRef *pcrRight, IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pcrRight);
	SPQOS_ASSERT(IMDType::EcmptOther > cmp_type);

	CExpression *pexprRight = PexprScalarIdent(mp, pcrRight);

	return PexprScalarCmp(mp, pexprLeft, pexprRight, cmp_type);
}

// Generate a comparison expression over an expression and a column
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, CExpression *pexprLeft,
					   const CColRef *pcrRight, CWStringConst strOp,
					   IMDId *mdid_op)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pcrRight);

	IMDType::ECmpType cmp_type = ParseCmpType(mdid_op);
	if (IMDType::EcmptOther != cmp_type)
	{
		IMDId *left_mdid = CScalar::PopConvert(pexprLeft->Pop())->MdidType();
		IMDId *right_mdid = pcrRight->RetrieveType()->MDId();

		if (CMDAccessorUtils::FCmpOrCastedCmpExists(left_mdid, right_mdid,
													cmp_type))
		{
			CExpression *pexprScCmp =
				PexprScalarCmp(mp, pexprLeft, pcrRight, cmp_type);
			mdid_op->Release();

			return pexprScCmp;
		}
	}

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, mdid_op, SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer()),
			cmp_type),
		pexprLeft, PexprScalarIdent(mp, pcrRight));
}

// Generate a comparison expression over two expressions
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, CExpression *pexprLeft,
					   CExpression *pexprRight, CWStringConst strOp,
					   IMDId *mdid_op)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	IMDType::ECmpType cmp_type = ParseCmpType(mdid_op);
	if (IMDType::EcmptOther != cmp_type)
	{
		IMDId *left_mdid = CScalar::PopConvert(pexprLeft->Pop())->MdidType();
		IMDId *right_mdid = CScalar::PopConvert(pexprRight->Pop())->MdidType();

		if (CMDAccessorUtils::FCmpOrCastedCmpExists(left_mdid, right_mdid,
													cmp_type))
		{
			CExpression *pexprScCmp =
				PexprScalarCmp(mp, pexprLeft, pexprRight, cmp_type);
			mdid_op->Release();

			return pexprScCmp;
		}
	}

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, mdid_op, SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer()),
			cmp_type),
		pexprLeft, pexprRight);
}

// Generate a comparison expression over two expressions
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, CExpression *pexprLeft,
					   CExpression *pexprRight, IMDId *mdid_scop)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);
	SPQOS_ASSERT(NULL != mdid_scop);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpression *pexprNewLeft = pexprLeft;
	CExpression *pexprNewRight = pexprRight;

	SPQOS_ASSERT(pexprNewLeft != NULL);
	SPQOS_ASSERT(pexprNewRight != NULL);

	CMDAccessorUtils::ApplyCastsForScCmp(mp, md_accessor, pexprNewLeft,
										 pexprNewRight, mdid_scop);

	mdid_scop->AddRef();
	const IMDScalarOp *op = md_accessor->RetrieveScOp(mdid_scop);
	const CMDName mdname = op->Mdname();
	CWStringConst strCmpOpName(mdname.GetMDName()->GetBuffer());

	CExpression *pexprResult = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CScalarCmp(mp, mdid_scop,
					   SPQOS_NEW(mp) CWStringConst(mp, strCmpOpName.GetBuffer()),
					   op->ParseCmpType()),
		pexprNewLeft, pexprNewRight);

	return pexprResult;
}

// Generate a comparison expression over two expressions
CExpression *
CUtils::PexprScalarCmp(CMemoryPool *mp, CExpression *pexprLeft,
					   CExpression *pexprRight, IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);
	SPQOS_ASSERT(IMDType::EcmptOther > cmp_type);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpression *pexprNewLeft = pexprLeft;
	CExpression *pexprNewRight = pexprRight;

	IMDId *op_mdid = CMDAccessorUtils::GetScCmpMdIdConsiderCasts(
		md_accessor, pexprNewLeft, pexprNewRight, cmp_type);
	CMDAccessorUtils::ApplyCastsForScCmp(mp, md_accessor, pexprNewLeft,
										 pexprNewRight, op_mdid);

	SPQOS_ASSERT(pexprNewLeft != NULL);
	SPQOS_ASSERT(pexprNewRight != NULL);

	op_mdid->AddRef();
	const IMDScalarOp *op = md_accessor->RetrieveScOp(op_mdid);
	const CMDName mdname = op->Mdname();
	CWStringConst strCmpOpName(mdname.GetMDName()->GetBuffer());

	CExpression *pexprResult = SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, op_mdid,
			SPQOS_NEW(mp) CWStringConst(mp, strCmpOpName.GetBuffer()), cmp_type),
		pexprNewLeft, pexprNewRight);

	return pexprResult;
}

// Generate an equality comparison expression over two columns
CExpression *
CUtils::PexprScalarEqCmp(CMemoryPool *mp, const CColRef *pcrLeft,
						 const CColRef *pcrRight)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pcrRight);

	return PexprScalarCmp(mp, pcrLeft, pcrRight, IMDType::EcmptEq);
}

// Generate an equality comparison expression over two expressions
CExpression *
CUtils::PexprScalarEqCmp(CMemoryPool *mp, CExpression *pexprLeft,
						 CExpression *pexprRight)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	return PexprScalarCmp(mp, pexprLeft, pexprRight, IMDType::EcmptEq);
}

// Generate an equality comparison expression over a column reference and an expression
CExpression *
CUtils::PexprScalarEqCmp(CMemoryPool *mp, const CColRef *pcrLeft,
						 CExpression *pexprRight)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	return PexprScalarCmp(mp, pcrLeft, pexprRight, IMDType::EcmptEq);
}

// Generate an equality comparison expression over an expression and a column reference
CExpression *
CUtils::PexprScalarEqCmp(CMemoryPool *mp, CExpression *pexprLeft,
						 const CColRef *pcrRight)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pcrRight);

	return PexprScalarCmp(mp, pexprLeft, pcrRight, IMDType::EcmptEq);
}

// returns number of children or constants of it is all constants
ULONG
CUtils::UlScalarArrayArity(CExpression *pexprArray)
{
	SPQOS_ASSERT(FScalarArray(pexprArray));

	ULONG arity = pexprArray->Arity();
	if (0 == arity)
	{
		CScalarArray *popScalarArray =
			CScalarArray::PopConvert(pexprArray->Pop());
		CScalarConstArray *pdrgPconst = popScalarArray->PdrgPconst();
		arity = pdrgPconst->Size();
	}
	return arity;
}

// returns constant operator of a scalar array expression
CScalarConst *
CUtils::PScalarArrayConstChildAt(CExpression *pexprArray, ULONG ul)
{
	// if the CScalarArray is already collapsed and the consts are stored in the
	// operator itself, we return the constant from the const array.
	if (FScalarArrayCollapsed(pexprArray))
	{
		CScalarArray *popScalarArray =
			CScalarArray::PopConvert(pexprArray->Pop());
		CScalarConstArray *pdrgPconst = popScalarArray->PdrgPconst();
		CScalarConst *pScalarConst = (*pdrgPconst)[ul];
		return pScalarConst;
	}
	// otherwise, we convert the child expression's operator if that's possible.
	else
	{
		return CScalarConst::PopConvert((*pexprArray)[ul]->Pop());
	}
}

// returns constant expression of a scalar array expression
CExpression *
CUtils::PScalarArrayExprChildAt(CMemoryPool *mp, CExpression *pexprArray,
								ULONG ul)
{
	ULONG arity = pexprArray->Arity();
	if (0 == arity)
	{
		CScalarArray *popScalarArray =
			CScalarArray::PopConvert(pexprArray->Pop());
		CScalarConstArray *pdrgPconst = popScalarArray->PdrgPconst();
		CScalarConst *pScalarConst = (*pdrgPconst)[ul];
		pScalarConst->AddRef();
		return SPQOS_NEW(mp) CExpression(mp, pScalarConst);
	}
	else
	{
		CExpression *pexprConst = (*pexprArray)[ul];
		pexprConst->AddRef();
		return pexprConst;
	}
}

// returns the scalar array expression child of CScalarArrayComp
CExpression *
CUtils::PexprScalarArrayChild(CExpression *pexprScalarArrayCmp)
{
	CExpression *pexprArray = (*pexprScalarArrayCmp)[1];
	if (FScalarArrayCoerce(pexprArray))
	{
		pexprArray = (*pexprArray)[0];
	}
	return pexprArray;
}

// returns if the scalar array has all constant elements or ScalarIdents
BOOL
CUtils::FScalarConstAndScalarIdentArray(CExpression *pexprArray)
{
	for (ULONG i = 0; i < pexprArray->Arity(); ++i)
	{
		CExpression *pexprChild = (*pexprArray)[i];

		if (!FScalarIdent(pexprChild) && !FScalarConst(pexprChild) &&
			!(CCastUtils::FScalarCast(pexprChild) &&
			  FScalarIdent((*pexprChild)[0])))
		{
			return false;
		}
	}

	return true;
}

// returns if the scalar array has all constant elements or children
BOOL
CUtils::FScalarConstArray(CExpression *pexprArray)
{
	const ULONG arity = pexprArray->Arity();

	BOOL fAllConsts = FScalarArray(pexprArray);
	for (ULONG ul = 0; fAllConsts && ul < arity; ul++)
	{
		fAllConsts = CUtils::FScalarConst((*pexprArray)[ul]);
	}

	return fAllConsts;
}

// returns if the scalar const is an array
BOOL
CUtils::FIsConstArray(CExpression *pexpr)
{
	CScalarConst *popScalarConst = CScalarConst::PopConvert(pexpr->Pop());
	CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDType *expr_type = mda->RetrieveType(popScalarConst->MdidType());
	return !IMDId::IsValid(expr_type->GetArrayTypeMdid());
}

// returns if the scalar constant array has already been collapased
BOOL
CUtils::FScalarArrayCollapsed(CExpression *pexprArray)
{
	const ULONG ulExprArity = pexprArray->Arity();
	const ULONG ulConstArity = UlScalarArrayArity(pexprArray);

	return ulExprArity == 0 && ulConstArity > 0;
}

// If it's a scalar array of all CScalarConst, collapse it into a single
// expression but keep the constants in the operator.
CExpression *
CUtils::PexprCollapseConstArray(CMemoryPool *mp, CExpression *pexprArray)
{
	SPQOS_ASSERT(NULL != pexprArray);

	const ULONG arity = pexprArray->Arity();

	// do not collapse already collapsed array, otherwise we lose the
	// collapsed constants.
	if (FScalarConstArray(pexprArray) && !FScalarArrayCollapsed(pexprArray))
	{
		CScalarConstArray *pdrgPconst = SPQOS_NEW(mp) CScalarConstArray(mp);
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CScalarConst *popConst =
				CScalarConst::PopConvert((*pexprArray)[ul]->Pop());
			popConst->AddRef();
			pdrgPconst->Append(popConst);
		}

		CScalarArray *psArray = CScalarArray::PopConvert(pexprArray->Pop());
		IMDId *elem_type_mdid = psArray->PmdidElem();
		IMDId *array_type_mdid = psArray->PmdidArray();
		elem_type_mdid->AddRef();
		array_type_mdid->AddRef();

		CScalarArray *pConstArray =
			SPQOS_NEW(mp) CScalarArray(mp, elem_type_mdid, array_type_mdid,
									  psArray->FMultiDimensional(), pdrgPconst);
		return SPQOS_NEW(mp) CExpression(mp, pConstArray);
	}

	// process children
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexprChild =
			PexprCollapseConstArray(mp, (*pexprArray)[ul]);
		pdrspqexpr->Append(pexprChild);
	}

	COperator *pop = pexprArray->Pop();
	pop->AddRef();
	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// generate an ArrayCmp expression given an array of CScalarConst's
CExpression *
CUtils::PexprScalarArrayCmp(CMemoryPool *mp,
							CScalarArrayCmp::EArrCmpType earrcmptype,
							IMDType::ECmpType ecmptype,
							CExpressionArray *pexprScalarChildren,
							const CColRef *colref)
{
	SPQOS_ASSERT(pexprScalarChildren != NULL);
	SPQOS_ASSERT(0 < pexprScalarChildren->Size());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// get column type mdid and mdid of the array type corresponding to that type
	IMDId *pmdidColType = colref->RetrieveType()->MDId();
	IMDId *pmdidArrType = colref->RetrieveType()->GetArrayTypeMdid();
	IMDId *pmdidCmpOp = colref->RetrieveType()->GetMdidForCmpType(ecmptype);

	if (!IMDId::IsValid(pmdidColType) || !IMDId::IsValid(pmdidArrType) ||
		!IMDId::IsValid(pmdidCmpOp))
	{
		// cannot construct an ArrayCmp expression if any of these are invalid
		return NULL;
	}

	pmdidCmpOp->AddRef();
	const CMDName mdname = md_accessor->RetrieveScOp(pmdidCmpOp)->Mdname();
	CWStringConst strOp(mdname.GetMDName()->GetBuffer());

	if (pexprScalarChildren->Size() == 1)
	{
		(*pexprScalarChildren)[0]->AddRef();
		CExpression *scalarCmp = CUtils::PexprScalarCmp(
			mp, colref, (*pexprScalarChildren)[0], strOp, pmdidCmpOp);
		pexprScalarChildren->Release();
		return scalarCmp;
	}

	pmdidColType->AddRef();
	pmdidArrType->AddRef();
	CExpression *pexprArray = SPQOS_NEW(mp)
		CExpression(mp,
					SPQOS_NEW(mp) CScalarArray(mp, pmdidColType, pmdidArrType,
											  false /*is_multidimenstional*/),
					pexprScalarChildren);

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarArrayCmp(
			mp, pmdidCmpOp, SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer()),
			earrcmptype),
		CUtils::PexprScalarIdent(mp, colref), pexprArray);
}

// generate a comparison against Zero
CExpression *
CUtils::PexprCmpWithZero(CMemoryPool *mp, CExpression *pexprLeft,
						 IMDId *mdid_type_left, IMDType::ECmpType ecmptype)
{
	SPQOS_ASSERT(pexprLeft->Pop()->FScalar());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type_left);
	SPQOS_ASSERT(pmdtype->GetDatumType() == IMDType::EtiInt8 &&
				"left expression must be of type int8");

	IMDId *mdid_op = pmdtype->GetMdidForCmpType(ecmptype);
	mdid_op->AddRef();
	const CMDName mdname = md_accessor->RetrieveScOp(mdid_op)->Mdname();
	CWStringConst strOpName(mdname.GetMDName()->GetBuffer());

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarCmp(
			mp, mdid_op, SPQOS_NEW(mp) CWStringConst(mp, strOpName.GetBuffer()),
			ecmptype),
		pexprLeft, CUtils::PexprScalarConstInt8(mp, 0 /*val*/));
}

// generate an Is Distinct From expression
CExpression *
CUtils::PexprIDF(CMemoryPool *mp, CExpression *pexprLeft,
				 CExpression *pexprRight)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpression *pexprNewLeft = pexprLeft;
	CExpression *pexprNewRight = pexprRight;

	IMDId *pmdidEqOp = CMDAccessorUtils::GetScCmpMdIdConsiderCasts(
		md_accessor, pexprNewLeft, pexprNewRight, IMDType::EcmptEq);
	CMDAccessorUtils::ApplyCastsForScCmp(mp, md_accessor, pexprNewLeft,
										 pexprNewRight, pmdidEqOp);
	pmdidEqOp->AddRef();
	const CMDName mdname = md_accessor->RetrieveScOp(pmdidEqOp)->Mdname();
	CWStringConst strEqOpName(mdname.GetMDName()->GetBuffer());

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarIsDistinctFrom(
			mp, pmdidEqOp,
			SPQOS_NEW(mp) CWStringConst(mp, strEqOpName.GetBuffer())),
		pexprNewLeft, pexprNewRight);
}

// generate an Is Distinct From expression
CExpression *
CUtils::PexprIDF(CMemoryPool *mp, CExpression *pexprLeft,
				 CExpression *pexprRight, IMDId *mdid_scop)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpression *pexprNewLeft = pexprLeft;
	CExpression *pexprNewRight = pexprRight;

	CMDAccessorUtils::ApplyCastsForScCmp(mp, md_accessor, pexprNewLeft,
										 pexprNewRight, mdid_scop);
	mdid_scop->AddRef();
	const CMDName mdname = md_accessor->RetrieveScOp(mdid_scop)->Mdname();
	CWStringConst strEqOpName(mdname.GetMDName()->GetBuffer());

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp) CScalarIsDistinctFrom(
			mp, mdid_scop,
			SPQOS_NEW(mp) CWStringConst(mp, strEqOpName.GetBuffer())),
		pexprNewLeft, pexprNewRight);
}

// generate an Is NOT Distinct From expression for two columns; the two columns must have the same type
CExpression *
CUtils::PexprINDF(CMemoryPool *mp, const CColRef *pcrLeft,
				  const CColRef *pcrRight)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pcrRight);

	return PexprINDF(mp, PexprScalarIdent(mp, pcrLeft),
					 PexprScalarIdent(mp, pcrRight));
}

// Generate an Is NOT Distinct From expression for scalar expressions;
// the two expressions must have the same type
CExpression *
CUtils::PexprINDF(CMemoryPool *mp, CExpression *pexprLeft,
				  CExpression *pexprRight)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	return PexprNegate(mp, PexprIDF(mp, pexprLeft, pexprRight));
}

CExpression *
CUtils::PexprINDF(CMemoryPool *mp, CExpression *pexprLeft,
				  CExpression *pexprRight, IMDId *mdid_scop)
{
	SPQOS_ASSERT(NULL != pexprLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	return PexprNegate(mp, PexprIDF(mp, pexprLeft, pexprRight, mdid_scop));
}

// Generate an Is Null expression
CExpression *
CUtils::PexprIsNull(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarNullTest(mp), pexpr);
}

// Generate an Is Not Null expression
CExpression *
CUtils::PexprIsNotNull(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return PexprNegate(mp, PexprIsNull(mp, pexpr));
}

// Generate an Is Not False expression
CExpression *
CUtils::PexprIsNotFalse(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return PexprIDF(mp, pexpr, PexprScalarConstBool(mp, false /*value*/));
}

// Find if a scalar expression uses a nullable column from the
// output of a logical expression
BOOL
CUtils::FUsesNullableCol(CMemoryPool *mp, CExpression *pexprScalar,
						 CExpression *pexprLogical)
{
	SPQOS_ASSERT(pexprScalar->Pop()->FScalar());
	SPQOS_ASSERT(pexprLogical->Pop()->FLogical());

	CColRefSet *pcrsNotNull = pexprLogical->DeriveNotNullColumns();
	CColRefSet *pcrsUsed = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsUsed->Include(pexprScalar->DeriveUsedColumns());
	pcrsUsed->Intersection(pexprLogical->DeriveOutputColumns());

	BOOL fUsesNullableCol = !pcrsNotNull->ContainsAll(pcrsUsed);
	pcrsUsed->Release();

	return fUsesNullableCol;
}

// Extract the partition key at the given level from the given array of partition keys
CColRef *
CUtils::PcrExtractPartKey(CColRef2dArray *pdrspqdrspqcr, ULONG ulLevel)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcr);
	SPQOS_ASSERT(ulLevel < pdrspqdrspqcr->Size());

	CColRefArray *pdrspqcrPartKey = (*pdrspqdrspqcr)[ulLevel];
	SPQOS_ASSERT(1 == pdrspqcrPartKey->Size() &&
				"Composite keys not currently supported");

	return (*pdrspqcrPartKey)[0];
}

// check for existence of subqueries
BOOL
CUtils::FHasSubquery(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	return pexpr->DeriveHasSubquery();
}

// check for existence of CTE anchor
BOOL
CUtils::FHasCTEAnchor(CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopLogicalCTEAnchor == pexpr->Pop()->Eopid())
	{
		return true;
	}

	for (ULONG ul = 0; ul < pexpr->Arity(); ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];
		if (FHasCTEAnchor(pexprChild))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@class:
//		CUtils::FHasSubqueryOrApply
//
//	@doc:
//		Check existence of subqueries or Apply operators in deep expression tree
//
//---------------------------------------------------------------------------
BOOL
CUtils::FHasSubqueryOrApply(CExpression *pexpr, BOOL fCheckRoot)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (fCheckRoot)
	{
		COperator *pop = pexpr->Pop();
		if (FApply(pop) && !FCorrelatedApply(pop))
		{
			return true;
		}

		if (pop->FScalar() && pexpr->DeriveHasSubquery())
		{
			return true;
		}
	}

	const ULONG arity = pexpr->Arity();
	BOOL fSubqueryOrApply = false;
	for (ULONG ul = 0; !fSubqueryOrApply && ul < arity; ul++)
	{
		fSubqueryOrApply = FHasSubqueryOrApply((*pexpr)[ul]);
	}

	return fSubqueryOrApply;
}

//---------------------------------------------------------------------------
//	@class:
//		CUtils::FHasCorrelatedApply
//
//	@doc:
//		Check existence of Correlated Apply operators in deep expression tree
//
//---------------------------------------------------------------------------
BOOL
CUtils::FHasCorrelatedApply(CExpression *pexpr, BOOL fCheckRoot)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (fCheckRoot && FCorrelatedApply(pexpr->Pop()))
	{
		return true;
	}

	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (FHasCorrelatedApply((*pexpr)[ul]))
		{
			return true;
		}
	}

	return false;
}

// check for existence of outer references
BOOL
CUtils::HasOuterRefs(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(pexpr->Pop()->FLogical());

	return 0 < pexpr->DeriveOuterReferences()->Size();
}

// check if a given operator is a logical join
BOOL
CUtils::FLogicalJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CLogicalJoin *popJoin = NULL;
	if (pop->FLogical())
	{
		// attempt casting to logical join,
		// dynamic cast returns NULL if operator is not join
		popJoin = dynamic_cast<CLogicalJoin *>(pop);
	}

	return (NULL != popJoin);
}

// check if a given operator is a logical set operation
BOOL
CUtils::FLogicalSetOp(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CLogicalSetOp *popSetOp = NULL;
	if (pop->FLogical())
	{
		// attempt casting to logical SetOp,
		// dynamic cast returns NULL if operator is not SetOp
		popSetOp = dynamic_cast<CLogicalSetOp *>(pop);
	}

	return (NULL != popSetOp);
}

// check if a given operator is a logical unary operator
BOOL
CUtils::FLogicalUnary(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CLogicalUnary *popUnary = NULL;
	if (pop->FLogical())
	{
		// attempt casting to logical unary,
		// dynamic cast returns NULL if operator is not unary
		popUnary = dynamic_cast<CLogicalUnary *>(pop);
	}

	return (NULL != popUnary);
}

// check if a given operator is a hash join
BOOL
CUtils::FHashJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CPhysicalHashJoin *popHJN = NULL;
	if (pop->FPhysical())
	{
		popHJN = dynamic_cast<CPhysicalHashJoin *>(pop);
	}

	return (NULL != popHJN);
}

// check if a given operator is a correlated nested loops join
BOOL
CUtils::FCorrelatedNLJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	BOOL fCorrelatedNLJ = false;
	if (FNLJoin(pop))
	{
		fCorrelatedNLJ = dynamic_cast<CPhysicalNLJoin *>(pop)->FCorrelated();
	}

	return fCorrelatedNLJ;
}

// check if a given operator is a nested loops join
BOOL
CUtils::FNLJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CPhysicalNLJoin *popNLJ = NULL;
	if (pop->FPhysical())
	{
		popNLJ = dynamic_cast<CPhysicalNLJoin *>(pop);
	}

	return (NULL != popNLJ);
}

// check if a given operator is a logical join
BOOL
CUtils::FPhysicalJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	return FHashJoin(pop) || FNLJoin(pop);
}

// check if a given operator is a physical left outer join
BOOL
CUtils::FPhysicalLeftOuterJoin(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	return COperator::EopPhysicalLeftOuterNLJoin == pop->Eopid() ||
		   COperator::EopPhysicalLeftOuterIndexNLJoin == pop->Eopid() ||
		   COperator::EopPhysicalLeftOuterHashJoin == pop->Eopid() ||
		   COperator::EopPhysicalCorrelatedLeftOuterNLJoin == pop->Eopid();
}

// check if a given operator is a physical agg
BOOL
CUtils::FPhysicalScan(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CPhysicalScan *popScan = NULL;
	if (pop->FPhysical())
	{
		// attempt casting to physical scan,
		// dynamic cast returns NULL if operator is not a scan
		popScan = dynamic_cast<CPhysicalScan *>(pop);
	}

	return (NULL != popScan);
}

// check if a given operator is a physical agg
BOOL
CUtils::FPhysicalAgg(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CPhysicalAgg *popAgg = NULL;
	if (pop->FPhysical())
	{
		// attempt casting to physical agg,
		// dynamic cast returns NULL if operator is not an agg
		popAgg = dynamic_cast<CPhysicalAgg *>(pop);
	}

	return (NULL != popAgg);
}

// check if a given operator is a physical motion
BOOL
CUtils::FPhysicalMotion(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CPhysicalMotion *popMotion = NULL;
	if (pop->FPhysical())
	{
		// attempt casting to physical motion,
		// dynamic cast returns NULL if operator is not a motion
		popMotion = dynamic_cast<CPhysicalMotion *>(pop);
	}

	return (NULL != popMotion);
}

// check if a given operator is an FEnforcer
BOOL
CUtils::FEnforcer(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	COperator::EOperatorId op_id = pop->Eopid();
	return COperator::EopPhysicalSort == op_id ||
		   COperator::EopPhysicalSpool == op_id ||
		   COperator::EopPhysicalPartitionSelector == op_id ||
		   FPhysicalMotion(pop);
}

// check if a given operator is an Apply
BOOL
CUtils::FApply(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	CLogicalApply *popApply = NULL;
	if (pop->FLogical())
	{
		// attempt casting to logical apply,
		// dynamic cast returns NULL if operator is not an Apply operator
		popApply = dynamic_cast<CLogicalApply *>(pop);
	}

	return (NULL != popApply);
}

// check if a given operator is a correlated Apply
BOOL
CUtils::FCorrelatedApply(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	BOOL fCorrelatedApply = false;
	if (FApply(pop))
	{
		fCorrelatedApply = CLogicalApply::PopConvert(pop)->FCorrelated();
	}

	return fCorrelatedApply;
}

// check if a given operator is left semi apply
BOOL
CUtils::FLeftSemiApply(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	BOOL fLeftSemiApply = false;
	if (FApply(pop))
	{
		fLeftSemiApply = CLogicalApply::PopConvert(pop)->FLeftSemiApply();
	}

	return fLeftSemiApply;
}

// check if a given operator is left anti semi apply
BOOL
CUtils::FLeftAntiSemiApply(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	BOOL fLeftAntiSemiApply = false;
	if (FApply(pop))
	{
		fLeftAntiSemiApply =
			CLogicalApply::PopConvert(pop)->FLeftAntiSemiApply();
	}

	return fLeftAntiSemiApply;
}

// check if a given operator is a subquery
BOOL
CUtils::FSubquery(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	COperator::EOperatorId op_id = pop->Eopid();
	return pop->FScalar() && (COperator::EopScalarSubquery == op_id ||
							  COperator::EopScalarSubqueryExists == op_id ||
							  COperator::EopScalarSubqueryNotExists == op_id ||
							  COperator::EopScalarSubqueryAll == op_id ||
							  COperator::EopScalarSubqueryAny == op_id);
}

// check if a given operator is existential subquery
BOOL
CUtils::FExistentialSubquery(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	COperator::EOperatorId op_id = pop->Eopid();
	return pop->FScalar() && (COperator::EopScalarSubqueryExists == op_id ||
							  COperator::EopScalarSubqueryNotExists == op_id);
}

// check if a given operator is quantified subquery
BOOL
CUtils::FQuantifiedSubquery(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	COperator::EOperatorId op_id = pop->Eopid();
	return pop->FScalar() && (COperator::EopScalarSubqueryAll == op_id ||
							  COperator::EopScalarSubqueryAny == op_id);
}

// check if given expression is a Project on ConstTable with one
// scalar subquery in Project List
BOOL
CUtils::FProjectConstTableWithOneScalarSubq(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopLogicalProject == pexpr->Pop()->Eopid() &&
		COperator::EopLogicalConstTableGet == (*pexpr)[0]->Pop()->Eopid())
	{
		CExpression *pexprScalar = (*pexpr)[1];
		SPQOS_ASSERT(COperator::EopScalarProjectList ==
					pexprScalar->Pop()->Eopid());

		if (1 == pexprScalar->Arity() &&
			FProjElemWithScalarSubq((*pexprScalar)[0]))
		{
			return true;
		}
	}

	return false;
}

// check if given expression is a Project Element with scalar subquery
BOOL
CUtils::FProjElemWithScalarSubq(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return (COperator::EopScalarProjectElement == pexpr->Pop()->Eopid() &&
			COperator::EopScalarSubquery == (*pexpr)[0]->Pop()->Eopid());
}

// check if given expression is a scalar subquery with a ConstTableGet as the only child
BOOL
CUtils::FScalarSubqWithConstTblGet(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopScalarSubquery == pexpr->Pop()->Eopid() &&
		COperator::EopLogicalConstTableGet == (*pexpr)[0]->Pop()->Eopid() &&
		1 == pexpr->Arity())
	{
		return true;
	}

	return false;
}

// check if a limit expression has 0 offset
BOOL
CUtils::FHasZeroOffset(CExpression *pexpr)
{
	SPQOS_ASSERT(COperator::EopLogicalLimit == pexpr->Pop()->Eopid() ||
				COperator::EopPhysicalLimit == pexpr->Pop()->Eopid());

	return FScalarConstIntZero((*pexpr)[1]);
}

// check if an expression is a 0 integer
BOOL
CUtils::FScalarConstIntZero(CExpression *pexprOffset)
{
	if (COperator::EopScalarConst != pexprOffset->Pop()->Eopid())
	{
		return false;
	}

	CScalarConst *popScalarConst = CScalarConst::PopConvert(pexprOffset->Pop());
	IDatum *datum = popScalarConst->GetDatum();

	switch (datum->GetDatumType())
	{
		case IMDType::EtiInt2:
			return (0 == dynamic_cast<IDatumInt2 *>(datum)->Value());
		case IMDType::EtiInt4:
			return (0 == dynamic_cast<IDatumInt4 *>(datum)->Value());
		case IMDType::EtiInt8:
			return (0 == dynamic_cast<IDatumInt8 *>(datum)->Value());
		default:
			return false;
	}
}

// deduplicate an array of expressions
CExpressionArray *
CUtils::PdrspqexprDedup(CMemoryPool *mp, CExpressionArray *pdrspqexpr)
{
	const ULONG size = pdrspqexpr->Size();
	CExpressionArray *pdrspqexprDedup = SPQOS_NEW(mp) CExpressionArray(mp);
	ExprHashSet *phsexpr = SPQOS_NEW(mp) ExprHashSet(mp);

	for (ULONG ul = 0; ul < size; ul++)
	{
		CExpression *pexpr = (*pdrspqexpr)[ul];
		pexpr->AddRef();
		if (phsexpr->Insert(pexpr))
		{
			pexpr->AddRef();
			pdrspqexprDedup->Append(pexpr);
		}
		else
		{
			pexpr->Release();
		}
	}

	phsexpr->Release();
	return pdrspqexprDedup;
}

// deep equality of expression arrays
BOOL
CUtils::Equals(const CExpressionArray *pdrspqexprLeft,
			   const CExpressionArray *pdrspqexprRight)
{
	SPQOS_CHECK_STACK_SIZE;

	// NULL arrays are equal
	if (NULL == pdrspqexprLeft || NULL == pdrspqexprRight)
	{
		return NULL == pdrspqexprLeft && NULL == pdrspqexprRight;
	}

	// start with pointers comparison
	if (pdrspqexprLeft == pdrspqexprRight)
	{
		return true;
	}

	const ULONG length = pdrspqexprLeft->Size();
	BOOL fEqual = (length == pdrspqexprRight->Size());

	for (ULONG ul = 0; ul < length && fEqual; ul++)
	{
		const CExpression *pexprLeft = (*pdrspqexprLeft)[ul];
		const CExpression *pexprRight = (*pdrspqexprRight)[ul];
		fEqual = Equals(pexprLeft, pexprRight);
	}

	return fEqual;
}

// deep equality of expression trees
BOOL
CUtils::Equals(const CExpression *pexprLeft, const CExpression *pexprRight)
{
	SPQOS_CHECK_STACK_SIZE;

	// NULL expressions are equal
	if (NULL == pexprLeft || NULL == pexprRight)
	{
		return NULL == pexprLeft && NULL == pexprRight;
	}

	// start with pointers comparison
	if (pexprLeft == pexprRight)
	{
		return true;
	}

	// compare number of children and root operators
	if (pexprLeft->Arity() != pexprRight->Arity() ||
		!pexprLeft->Pop()->Matches(pexprRight->Pop()))
	{
		return false;
	}

	if (0 < pexprLeft->Arity() && pexprLeft->Pop()->FInputOrderSensitive())
	{
		return FMatchChildrenOrdered(pexprLeft, pexprRight);
	}

	return FMatchChildrenUnordered(pexprLeft, pexprRight);
}

// check if two expressions have the same children in any order
BOOL
CUtils::FMatchChildrenUnordered(const CExpression *pexprLeft,
								const CExpression *pexprRight)
{
	BOOL fEqual = true;
	const ULONG arity = pexprLeft->Arity();
	SPQOS_ASSERT(pexprRight->Arity() == arity);

	for (ULONG ul = 0; fEqual && ul < arity; ul++)
	{
		CExpression *pexpr = (*pexprLeft)[ul];
		fEqual = (UlOccurrences(pexpr, pexprLeft->PdrgPexpr()) ==
				  UlOccurrences(pexpr, pexprRight->PdrgPexpr()));
	}

	return fEqual;
}

// check if two expressions have the same children in the same order
BOOL
CUtils::FMatchChildrenOrdered(const CExpression *pexprLeft,
							  const CExpression *pexprRight)
{
	BOOL fEqual = true;
	const ULONG arity = pexprLeft->Arity();
	SPQOS_ASSERT(pexprRight->Arity() == arity);

	for (ULONG ul = 0; fEqual && ul < arity; ul++)
	{
		// child must be at the same position in the other expression
		fEqual = Equals((*pexprLeft)[ul], (*pexprRight)[ul]);
	}

	return fEqual;
}

// return the number of occurrences of the given expression in the given array of expressions
ULONG
CUtils::UlOccurrences(const CExpression *pexpr, CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	ULONG count = 0;

	const ULONG size = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		if (Equals(pexpr, (*pdrspqexpr)[ul]))
		{
			count++;
		}
	}

	return count;
}

// compare expression against an array of expressions
BOOL
CUtils::FEqualAny(const CExpression *pexpr, const CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	const ULONG size = pdrspqexpr->Size();
	BOOL fEqual = false;
	for (ULONG ul = 0; !fEqual && ul < size; ul++)
	{
		fEqual = Equals(pexpr, (*pdrspqexpr)[ul]);
	}

	return fEqual;
}

// check if first expression array contains all expressions in second array
BOOL
CUtils::Contains(const CExpressionArray *pdrspqexprFst,
				 const CExpressionArray *pdrspqexprSnd)
{
	SPQOS_ASSERT(NULL != pdrspqexprFst);
	SPQOS_ASSERT(NULL != pdrspqexprSnd);

	if (pdrspqexprFst == pdrspqexprSnd)
	{
		return true;
	}

	if (pdrspqexprFst->Size() < pdrspqexprSnd->Size())
	{
		return false;
	}

	const ULONG size = pdrspqexprSnd->Size();
	BOOL fContains = true;
	for (ULONG ul = 0; fContains && ul < size; ul++)
	{
		fContains = FEqualAny((*pdrspqexprSnd)[ul], pdrspqexprFst);
	}

	return fContains;
}

// generate a Not expression on top of the given expression
CExpression *
CUtils::PexprNegate(CMemoryPool *mp, CExpression *pexpr)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexpr->Append(pexpr);

	return PexprScalarBoolOp(mp, CScalarBoolOp::EboolopNot, pdrspqexpr);
}

// generate a ScalarOp expression over a column and an expression
CExpression *
CUtils::PexprScalarOp(CMemoryPool *mp, const CColRef *pcrLeft,
					  CExpression *pexprRight, const CWStringConst strOp,
					  IMDId *mdid_op, IMDId *return_type_mdid)
{
	SPQOS_ASSERT(NULL != pcrLeft);
	SPQOS_ASSERT(NULL != pexprRight);

	return SPQOS_NEW(mp) CExpression(
		mp,
		SPQOS_NEW(mp)
			CScalarOp(mp, mdid_op, return_type_mdid,
					  SPQOS_NEW(mp) CWStringConst(mp, strOp.GetBuffer())),
		PexprScalarIdent(mp, pcrLeft), pexprRight);
}

// generate a ScalarBoolOp expression
CExpression *
CUtils::PexprScalarBoolOp(CMemoryPool *mp, CScalarBoolOp::EBoolOperator eboolop,
						  CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(0 < pdrspqexpr->Size());

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarBoolOp(mp, eboolop), pdrspqexpr);
}

// generate a boolean scalar constant expression
CExpression *
CUtils::PexprScalarConstBool(CMemoryPool *mp, BOOL fval, BOOL is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// create a BOOL datum
	const IMDTypeBool *pmdtypebool = md_accessor->PtMDType<IMDTypeBool>();
	IDatumBool *datum = pmdtypebool->CreateBoolDatum(mp, fval, is_null);

	CExpression *pexpr = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, (IDatum *) datum));

	return pexpr;
}

// generate an int4 scalar constant expression
CExpression *
CUtils::PexprScalarConstInt4(CMemoryPool *mp, INT val)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// create a int4 datum
	const IMDTypeInt4 *pmdtypeint4 = md_accessor->PtMDType<IMDTypeInt4>();
	IDatumInt4 *datum = pmdtypeint4->CreateInt4Datum(mp, val, false);

	CExpression *pexpr = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, (IDatum *) datum));

	return pexpr;
}

// generate an int8 scalar constant expression
CExpression *
CUtils::PexprScalarConstInt8(CMemoryPool *mp, LINT val, BOOL is_null)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// create a int8 datum
	const IMDTypeInt8 *pmdtypeint8 = md_accessor->PtMDType<IMDTypeInt8>();
	IDatumInt8 *datum = pmdtypeint8->CreateInt8Datum(mp, val, is_null);

	CExpression *pexpr = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, (IDatum *) datum));

	return pexpr;
}

// generate an oid scalar constant expression
CExpression *
CUtils::PexprScalarConstOid(CMemoryPool *mp, OID oid_val)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// create a oid datum
	const IMDTypeOid *pmdtype = md_accessor->PtMDType<IMDTypeOid>();
	IDatumOid *datum = pmdtype->CreateOidDatum(mp, oid_val, false /*is_null*/);

	CExpression *pexpr = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, (IDatum *) datum));

	return pexpr;
}

// generate a NULL constant of a given type
CExpression *
CUtils::PexprScalarConstNull(CMemoryPool *mp, const IMDType *typ,
							 INT type_modifier)
{
	IDatum *datum = NULL;
	IMDId *mdid = typ->MDId();
	mdid->AddRef();

	switch (typ->GetDatumType())
	{
		case IMDType::EtiInt2:
		{
			const IMDTypeInt2 *pmdtypeint2 =
				static_cast<const IMDTypeInt2 *>(typ);
			datum = pmdtypeint2->CreateInt2Datum(mp, 0, true);
		}
		break;

		case IMDType::EtiInt4:
		{
			const IMDTypeInt4 *pmdtypeint4 =
				static_cast<const IMDTypeInt4 *>(typ);
			datum = pmdtypeint4->CreateInt4Datum(mp, 0, true);
		}
		break;

		case IMDType::EtiInt8:
		{
			const IMDTypeInt8 *pmdtypeint8 =
				static_cast<const IMDTypeInt8 *>(typ);
			datum = pmdtypeint8->CreateInt8Datum(mp, 0, true);
		}
		break;

		case IMDType::EtiBool:
		{
			const IMDTypeBool *pmdtypebool =
				static_cast<const IMDTypeBool *>(typ);
			datum = pmdtypebool->CreateBoolDatum(mp, false, true);
		}
		break;

		case IMDType::EtiOid:
		{
			const IMDTypeOid *pmdtypeoid = static_cast<const IMDTypeOid *>(typ);
			datum = pmdtypeoid->CreateOidDatum(mp, 0, true);
		}
		break;

		case IMDType::EtiGeneric:
		{
			const IMDTypeGeneric *pmdtypegeneric =
				static_cast<const IMDTypeGeneric *>(typ);
			datum = pmdtypegeneric->CreateGenericNullDatum(mp, type_modifier);
		}
		break;

		default:
			// shouldn't come here
			SPQOS_RTL_ASSERT(!"Invalid operator type");
	}

	return SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarConst(mp, datum));
}

// get column reference defined by project element
CColRef *
CUtils::PcrFromProjElem(CExpression *pexprPrEl)
{
	return (CScalarProjectElement::PopConvert(pexprPrEl->Pop()))->Pcr();
}

// generate an aggregate function operator
CScalarAggFunc *
CUtils::PopAggFunc(
	CMemoryPool *mp, IMDId *pmdidAggFunc, const CWStringConst *pstrAggFunc,
	BOOL is_distinct, EAggfuncStage eaggfuncstage, BOOL fSplit,
	IMDId *
		pmdidResolvedReturnType,  // return type to be used if original return type is ambiguous
	EAggfuncKind aggkind)
{
	SPQOS_ASSERT(NULL != pmdidAggFunc);
	SPQOS_ASSERT(NULL != pstrAggFunc);
	SPQOS_ASSERT_IMP(NULL != pmdidResolvedReturnType,
					pmdidResolvedReturnType->IsValid());

	return SPQOS_NEW(mp)
		CScalarAggFunc(mp, pmdidAggFunc, pmdidResolvedReturnType, pstrAggFunc,
					   is_distinct, eaggfuncstage, fSplit, aggkind);
}

// generate an aggregate function
CExpression *
CUtils::PexprAggFunc(CMemoryPool *mp, IMDId *pmdidAggFunc,
					 const CWStringConst *pstrAggFunc, const CColRef *colref,
					 BOOL is_distinct, EAggfuncStage eaggfuncstage, BOOL fSplit)
{
	SPQOS_ASSERT(NULL != pstrAggFunc);
	SPQOS_ASSERT(NULL != colref);

	// generate aggregate function
	CScalarAggFunc *popScAggFunc =
		PopAggFunc(mp, pmdidAggFunc, pstrAggFunc, is_distinct, eaggfuncstage,
				   fSplit, NULL, EaggfunckindNormal);

	CExpression *pexprScalarIdent = PexprScalarIdent(mp, colref);
	CExpressionArray *pdrspqexprArgs = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprArgs->Append(pexprScalarIdent);

	return SPQOS_NEW(mp)
		CExpression(mp, popScAggFunc, PexprAggFuncArgs(mp, pdrspqexprArgs));
}

// generate arguments of an aggregate function
CExpressionArray *
CUtils::PexprAggFuncArgs(CMemoryPool *mp, CExpressionArray *pdrspqexprArgs)
{
	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);

	pdrspqexpr->Append(SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CScalarValuesList(mp), pdrspqexprArgs));

	pdrspqexpr->Append(SPQOS_NEW(mp)
						  CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									  SPQOS_NEW(mp) CExpressionArray(mp)));

	pdrspqexpr->Append(SPQOS_NEW(mp)
						  CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									  SPQOS_NEW(mp) CExpressionArray(mp)));

	pdrspqexpr->Append(SPQOS_NEW(mp)
						  CExpression(mp, SPQOS_NEW(mp) CScalarValuesList(mp),
									  SPQOS_NEW(mp) CExpressionArray(mp)));
	return pdrspqexpr;
}


// generate a count(*) expression
CExpression *
CUtils::PexprCountStar(CMemoryPool *mp)
{
	// TODO,  04/26/2012, create count(*) expressions in a system-independent
	// way using MDAccessor

	CMDIdSPQDB *mdid =
		SPQOS_NEW(mp) CMDIdSPQDB(IMDId::EmdidGeneral, SPQDB_COUNT_STAR);
	CWStringConst *str = SPQOS_NEW(mp) CWStringConst(SPQOS_WSZ_LIT("count"));

	CExpressionArray *pdrspqexprChildren = SPQOS_NEW(mp) CExpressionArray(mp);

	CScalarAggFunc *popScAggFunc =
		PopAggFunc(mp, mdid, str, false /*is_distinct*/,
				   EaggfuncstageGlobal /*eaggfuncstage*/, false /*fSplit*/,
				   NULL, EaggfunckindNormal);

	CExpression *pexprCountStar = SPQOS_NEW(mp)
		CExpression(mp, popScAggFunc, PexprAggFuncArgs(mp, pdrspqexprChildren));

	return pexprCountStar;
}

// generate a GbAgg with count(*) function over the given expression
CExpression *
CUtils::PexprCountStar(CMemoryPool *mp, CExpression *pexprLogical)
{
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// generate a count(*) expression
	CExpression *pexprCountStar = PexprCountStar(mp);

	// generate a computed column with count(*) type
	CScalarAggFunc *popScalarAggFunc =
		CScalarAggFunc::PopConvert(pexprCountStar->Pop());
	IMDId *mdid_type = popScalarAggFunc->MdidType();
	INT type_modifier = popScalarAggFunc->TypeModifier();
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);
	CColRef *pcrComputed = col_factory->PcrCreate(pmdtype, type_modifier);
	CExpression *pexprPrjElem =
		PexprScalarProjectElement(mp, pcrComputed, pexprCountStar);
	CExpression *pexprPrjList = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pexprPrjElem);

	// generate a Gb expression
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	return PexprLogicalGbAggGlobal(mp, colref_array, pexprLogical,
								   pexprPrjList);
}

// generate a GbAgg with count(*) and sum(col) over the given expression
CExpression *
CUtils::PexprCountStarAndSum(CMemoryPool *mp, const CColRef *colref,
							 CExpression *pexprLogical)
{
	SPQOS_ASSERT(pexprLogical->DeriveOutputColumns()->FMember(colref));

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// generate a count(*) expression
	CExpression *pexprCountStar = PexprCountStar(mp);

	// generate a computed column with count(*) type
	CScalarAggFunc *popScalarAggFunc =
		CScalarAggFunc::PopConvert(pexprCountStar->Pop());
	IMDId *mdid_type = popScalarAggFunc->MdidType();
	INT type_modifier = popScalarAggFunc->TypeModifier();
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);
	CColRef *pcrComputed = col_factory->PcrCreate(pmdtype, type_modifier);
	CExpression *pexprPrjElemCount =
		PexprScalarProjectElement(mp, pcrComputed, pexprCountStar);

	// generate sum(col) expression
	CExpression *pexprSum = PexprSum(mp, colref);
	CScalarAggFunc *popScalarSumFunc =
		CScalarAggFunc::PopConvert(pexprSum->Pop());
	const IMDType *pmdtypeSum =
		md_accessor->RetrieveType(popScalarSumFunc->MdidType());
	CColRef *pcrSum =
		col_factory->PcrCreate(pmdtypeSum, popScalarSumFunc->TypeModifier());
	CExpression *pexprPrjElemSum =
		PexprScalarProjectElement(mp, pcrSum, pexprSum);
	CExpression *pexprPrjList =
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pexprPrjElemCount, pexprPrjElemSum);

	// generate a Gb expression
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	return PexprLogicalGbAggGlobal(mp, colref_array, pexprLogical,
								   pexprPrjList);
}

// return True if passed expression is a Project Element defined on count(*)/count(Any) agg
BOOL
CUtils::FCountAggProjElem(
	CExpression *pexprPrjElem,
	CColRef **ppcrCount	 // output: count(*)/count(Any) column
)
{
	SPQOS_ASSERT(NULL != pexprPrjElem);
	SPQOS_ASSERT(NULL != ppcrCount);

	COperator *pop = pexprPrjElem->Pop();
	if (COperator::EopScalarProjectElement != pop->Eopid())
	{
		return false;
	}

	if (COperator::EopScalarAggFunc == (*pexprPrjElem)[0]->Pop()->Eopid())
	{
		CScalarAggFunc *popAggFunc =
			CScalarAggFunc::PopConvert((*pexprPrjElem)[0]->Pop());
		if (popAggFunc->FCountStar() || popAggFunc->FCountAny())
		{
			*ppcrCount = CScalarProjectElement::PopConvert(pop)->Pcr();
			return true;
		}
	}

	return FHasCountAgg((*pexprPrjElem)[0], ppcrCount);
}

// check if the given expression has a count(*)/count(Any) agg, return the top-most found count column
BOOL
CUtils::FHasCountAgg(CExpression *pexpr,
					 CColRef **ppcrCount  // output: count(*)/count(Any) column
)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != ppcrCount);

	if (COperator::EopScalarProjectElement == pexpr->Pop()->Eopid())
	{
		// base case, count(*)/count(Any) must appear below a project element
		return FCountAggProjElem(pexpr, ppcrCount);
	}

	// recursively process children
	BOOL fHasCountAgg = false;
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; !fHasCountAgg && ul < arity; ul++)
	{
		fHasCountAgg = FHasCountAgg((*pexpr)[ul], ppcrCount);
	}

	return fHasCountAgg;
}


static BOOL
FCountAggMatchingColumn(CExpression *pexprPrjElem, const CColRef *colref)
{
	CColRef *pcrCount = NULL;
	return CUtils::FCountAggProjElem(pexprPrjElem, &pcrCount) &&
		   colref == pcrCount;
}


BOOL
CUtils::FHasCountAggMatchingColumn(const CExpression *pexpr,
								   const CColRef *colref,
								   const CLogicalGbAgg **ppgbAgg)
{
	COperator *pop = pexpr->Pop();
	// base case, we have a logical agg operator
	if (COperator::EopLogicalGbAgg == pop->Eopid())
	{
		const CExpression *const pexprProjectList = (*pexpr)[1];
		SPQOS_ASSERT(COperator::EopScalarProjectList ==
					pexprProjectList->Pop()->Eopid());
		const ULONG arity = pexprProjectList->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *const pexprPrjElem = (*pexprProjectList)[ul];
			if (FCountAggMatchingColumn(pexprPrjElem, colref))
			{
				const CLogicalGbAgg *pgbAgg = CLogicalGbAgg::PopConvert(pop);
				*ppgbAgg = pgbAgg;
				return true;
			}
		}
	}
	// recurse
	else
	{
		const ULONG arity = pexpr->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			const CExpression *pexprChild = (*pexpr)[ul];
			if (FHasCountAggMatchingColumn(pexprChild, colref, ppgbAgg))
			{
				return true;
			}
		}
	}
	return false;
}

// generate a sum(col) expression
CExpression *
CUtils::PexprSum(CMemoryPool *mp, const CColRef *colref)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	return PexprAgg(mp, md_accessor, IMDType::EaggSum, colref,
					false /*is_distinct*/);
}

// generate a GbAgg with sum(col) expressions for all columns in the passed array
CExpression *
CUtils::PexprGbAggSum(CMemoryPool *mp, CExpression *pexprLogical,
					  CColRefArray *pdrspqcrSum)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG size = pdrspqcrSum->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRef *colref = (*pdrspqcrSum)[ul];
		CExpression *pexprSum = PexprSum(mp, colref);
		CScalarAggFunc *popScalarAggFunc =
			CScalarAggFunc::PopConvert(pexprSum->Pop());
		const IMDType *pmdtypeSum =
			md_accessor->RetrieveType(popScalarAggFunc->MdidType());
		CColRef *pcrSum = col_factory->PcrCreate(
			pmdtypeSum, popScalarAggFunc->TypeModifier());
		CExpression *pexprPrjElemSum =
			PexprScalarProjectElement(mp, pcrSum, pexprSum);
		pdrspqexpr->Append(pexprPrjElemSum);
	}

	CExpression *pexprPrjList = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexpr);

	// generate a Gb expression
	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	return PexprLogicalGbAggGlobal(mp, colref_array, pexprLogical,
								   pexprPrjList);
}

// generate a count(<distinct> col) expression
CExpression *
CUtils::PexprCount(CMemoryPool *mp, const CColRef *colref, BOOL is_distinct)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	return PexprAgg(mp, md_accessor, IMDType::EaggCount, colref, is_distinct);
}

// generate a min(col) expression
CExpression *
CUtils::PexprMin(CMemoryPool *mp, CMDAccessor *md_accessor,
				 const CColRef *colref)
{
	return PexprAgg(mp, md_accessor, IMDType::EaggMin, colref,
					false /*is_distinct*/);
}

// generate an aggregate expression of the specified type
CExpression *
CUtils::PexprAgg(CMemoryPool *mp, CMDAccessor *md_accessor,
				 IMDType::EAggType agg_type, const CColRef *colref,
				 BOOL is_distinct)
{
	SPQOS_ASSERT(IMDType::EaggGeneric > agg_type);
	SPQOS_ASSERT(colref->RetrieveType()->GetMdidForAggType(agg_type)->IsValid());

	const IMDAggregate *pmdagg = md_accessor->RetrieveAgg(
		colref->RetrieveType()->GetMdidForAggType(agg_type));

	IMDId *agg_mdid = pmdagg->MDId();
	agg_mdid->AddRef();
	CWStringConst *str = SPQOS_NEW(mp)
		CWStringConst(mp, pmdagg->Mdname().GetMDName()->GetBuffer());

	return PexprAggFunc(mp, agg_mdid, str, colref, is_distinct,
						EaggfuncstageGlobal /*fGlobal*/, false /*fSplit*/);
}

// generate a select expression
CExpression *
CUtils::PexprLogicalSelect(CMemoryPool *mp, CExpression *pexpr,
						   CExpression *pexprPredicate)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pexprPredicate);

	CTableDescriptor *ptabdesc = NULL;
	if (pexpr->Pop()->Eopid() == CLogical::EopLogicalSelect ||
		pexpr->Pop()->Eopid() == CLogical::EopLogicalGet ||
		pexpr->Pop()->Eopid() == CLogical::EopLogicalDynamicGet)
	{
		ptabdesc = pexpr->DeriveTableDescriptor();
		// there are some cases where we don't populate LogicalSelect currently
		SPQOS_ASSERT_IMP(pexpr->Pop()->Eopid() != CLogical::EopLogicalSelect,
						NULL != ptabdesc);
	}
	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalSelect(mp, ptabdesc), pexpr, pexprPredicate);
}

// if predicate is True return logical expression, otherwise return a new select node
CExpression *
CUtils::PexprSafeSelect(CMemoryPool *mp, CExpression *pexprLogical,
						CExpression *pexprPred)
{
	SPQOS_ASSERT(NULL != pexprLogical);
	SPQOS_ASSERT(NULL != pexprPred);

	if (FScalarConstTrue(pexprPred))
	{
		// caller must have add-refed the predicate before coming here
		pexprPred->Release();
		return pexprLogical;
	}

	return PexprLogicalSelect(mp, pexprLogical, pexprPred);
}

// generate a select expression, if child is another Select expression
// collapse both Selects into one expression
CExpression *
CUtils::PexprCollapseSelect(CMemoryPool *mp, CExpression *pexpr,
							CExpression *pexprPredicate)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pexprPredicate);

	if (COperator::EopLogicalSelect == pexpr->Pop()->Eopid() &&
		2 ==
			pexpr
				->Arity()  // perform collapsing only when we have a full Select tree
	)
	{
		// collapse Selects into one expression
		(*pexpr)[0]->AddRef();
		CExpression *pexprChild = (*pexpr)[0];
		CExpression *pexprScalar =
			CPredicateUtils::PexprConjunction(mp, pexprPredicate, (*pexpr)[1]);

		return SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp),
										pexprChild, pexprScalar);
	}

	pexpr->AddRef();
	pexprPredicate->AddRef();

	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalSelect(mp), pexpr, pexprPredicate);
}

// generate a project expression
CExpression *
CUtils::PexprLogicalProject(CMemoryPool *mp, CExpression *pexpr,
							CExpression *pexprPrjList, BOOL fNewComputedCol)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pexprPrjList);
	SPQOS_ASSERT(COperator::EopScalarProjectList ==
				pexprPrjList->Pop()->Eopid());

	if (fNewComputedCol)
	{
		CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
		const ULONG arity = pexprPrjList->Arity();
		for (ULONG ul = 0; ul < arity; ul++)
		{
			CExpression *pexprPrEl = (*pexprPrjList)[ul];
			col_factory->AddComputedToUsedColsMap(pexprPrEl);
		}
	}
	return SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CLogicalProject(mp), pexpr, pexprPrjList);
}

// generate a sequence project expression
CExpression *
CUtils::PexprLogicalSequenceProject(CMemoryPool *mp, CDistributionSpec *pds,
									COrderSpecArray *pdrspqos,
									CWindowFrameArray *pdrspqwf,
									CExpression *pexpr,
									CExpression *pexprPrjList)
{
	SPQOS_ASSERT(NULL != pds);
	SPQOS_ASSERT(NULL != pdrspqos);
	SPQOS_ASSERT(NULL != pdrspqwf);
	SPQOS_ASSERT(pdrspqwf->Size() == pdrspqos->Size());
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pexprPrjList);
	SPQOS_ASSERT(COperator::EopScalarProjectList ==
				pexprPrjList->Pop()->Eopid());

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalSequenceProject(mp, pds, pdrspqos, pdrspqwf),
		pexpr, pexprPrjList);
}

// construct a projection of NULL constants using the given column
// names and types on top of the given expression
CExpression *
CUtils::PexprLogicalProjectNulls(CMemoryPool *mp, CColRefArray *colref_array,
								 CExpression *pexpr,
								 UlongToColRefMap *colref_mapping)
{
	IDatumArray *pdrspqdatum =
		CTranslatorExprToDXLUtils::PdrspqdatumNulls(mp, colref_array);
	CExpression *pexprProjList =
		PexprScalarProjListConst(mp, colref_array, pdrspqdatum, colref_mapping);
	pdrspqdatum->Release();

	return PexprLogicalProject(mp, pexpr, pexprProjList,
							   false /*fNewComputedCol*/);
}

// construct a project list using the column names and types of the given
// array of column references, and the given datums
CExpression *
CUtils::PexprScalarProjListConst(CMemoryPool *mp, CColRefArray *colref_array,
								 IDatumArray *pdrspqdatum,
								 UlongToColRefMap *colref_mapping)
{
	SPQOS_ASSERT(colref_array->Size() == pdrspqdatum->Size());

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CScalarProjectList *pscprl = SPQOS_NEW(mp) CScalarProjectList(mp);

	const ULONG arity = colref_array->Size();
	if (0 == arity)
	{
		return SPQOS_NEW(mp) CExpression(mp, pscprl);
	}

	CExpressionArray *pdrspqexprProjElems = SPQOS_NEW(mp) CExpressionArray(mp);
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColRef *colref = (*colref_array)[ul];

		IDatum *datum = (*pdrspqdatum)[ul];
		datum->AddRef();
		CScalarConst *popScConst = SPQOS_NEW(mp) CScalarConst(mp, datum);
		CExpression *pexprScConst = SPQOS_NEW(mp) CExpression(mp, popScConst);

		CColRef *new_colref = col_factory->PcrCreate(
			colref->RetrieveType(), colref->TypeModifier(), colref->Name());
		if (NULL != colref_mapping)
		{
#ifdef SPQOS_DEBUG
			BOOL fInserted =
#endif
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(colref->Id()),
									   new_colref);
			SPQOS_ASSERT(fInserted);
		}

		CScalarProjectElement *popScPrEl =
			SPQOS_NEW(mp) CScalarProjectElement(mp, new_colref);
		CExpression *pexprScPrEl =
			SPQOS_NEW(mp) CExpression(mp, popScPrEl, pexprScConst);

		pdrspqexprProjElems->Append(pexprScPrEl);
	}

	return SPQOS_NEW(mp) CExpression(mp, pscprl, pdrspqexprProjElems);
}

// generate a project expression with one additional project element
CExpression *
CUtils::PexprAddProjection(CMemoryPool *mp, CExpression *pexpr,
						   CExpression *pexprProjected)
{
	SPQOS_ASSERT(pexpr->Pop()->FLogical());
	SPQOS_ASSERT(pexprProjected->Pop()->FScalar());

	CExpressionArray *pdrspqexprProjected = SPQOS_NEW(mp) CExpressionArray(mp);
	pdrspqexprProjected->Append(pexprProjected);

	CExpression *pexprProjection =
		PexprAddProjection(mp, pexpr, pdrspqexprProjected);
	pdrspqexprProjected->Release();

	return pexprProjection;
}

// generate a project expression with one or more additional project elements
CExpression *
CUtils::PexprAddProjection(CMemoryPool *mp, CExpression *pexpr,
						   CExpressionArray *pdrspqexprProjected,
						   BOOL fNewComputedCol)
{
	SPQOS_ASSERT(pexpr->Pop()->FLogical());
	SPQOS_ASSERT(NULL != pdrspqexprProjected);

	if (0 == pdrspqexprProjected->Size())
	{
		return pexpr;
	}

	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	CExpressionArray *pdrspqexprPrjElem = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG ulProjected = pdrspqexprProjected->Size();
	for (ULONG ul = 0; ul < ulProjected; ul++)
	{
		CExpression *pexprProjected = (*pdrspqexprProjected)[ul];
		SPQOS_ASSERT(pexprProjected->Pop()->FScalar());

		// generate a computed column with scalar expression type
		CScalar *popScalar = CScalar::PopConvert(pexprProjected->Pop());
		const IMDType *pmdtype =
			md_accessor->RetrieveType(popScalar->MdidType());
		CColRef *colref =
			col_factory->PcrCreate(pmdtype, popScalar->TypeModifier());

		pexprProjected->AddRef();
		pdrspqexprPrjElem->Append(
			PexprScalarProjectElement(mp, colref, pexprProjected));
	}

	CExpression *pexprPrjList = SPQOS_NEW(mp)
		CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp), pdrspqexprPrjElem);

	return PexprLogicalProject(mp, pexpr, pexprPrjList, fNewComputedCol);
}

// generate an aggregate expression
CExpression *
CUtils::PexprLogicalGbAgg(CMemoryPool *mp, CColRefArray *colref_array,
						  CExpression *pexprRelational, CExpression *pexprPrL,
						  COperator::EGbAggType egbaggtype)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(NULL != pexprRelational);
	SPQOS_ASSERT(NULL != pexprPrL);

	// create a new logical group by operator
	CLogicalGbAgg *pop =
		SPQOS_NEW(mp) CLogicalGbAgg(mp, colref_array, egbaggtype);

	return SPQOS_NEW(mp) CExpression(mp, pop, pexprRelational, pexprPrL);
}

// generate a global aggregate expression
CExpression *
CUtils::PexprLogicalGbAggGlobal(CMemoryPool *mp, CColRefArray *colref_array,
								CExpression *pexprRelational,
								CExpression *pexprProjList)
{
	return PexprLogicalGbAgg(mp, colref_array, pexprRelational, pexprProjList,
							 COperator::EgbaggtypeGlobal);
}

// check if given project list has a global aggregate function
BOOL
CUtils::FHasGlobalAggFunc(const CExpression *pexprAggProjList)
{
	SPQOS_ASSERT(COperator::EopScalarProjectList ==
				pexprAggProjList->Pop()->Eopid());
	BOOL fGlobal = false;

	const ULONG arity = pexprAggProjList->Arity();

	for (ULONG ul = 0; ul < arity && !fGlobal; ul++)
	{
		CExpression *pexprPrEl = (*pexprAggProjList)[ul];
		SPQOS_ASSERT(COperator::EopScalarProjectElement ==
					pexprPrEl->Pop()->Eopid());
		// get the scalar child of the project element
		CExpression *pexprAggFunc = (*pexprPrEl)[0];
		CScalarAggFunc *popScAggFunc =
			CScalarAggFunc::PopConvert(pexprAggFunc->Pop());
		fGlobal = popScAggFunc->FGlobal();
	}

	return fGlobal;
}

// return the comparison type for the given mdid
IMDType::ECmpType
CUtils::ParseCmpType(IMDId *mdid)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDScalarOp *md_scalar_op = md_accessor->RetrieveScOp(mdid);
	return md_scalar_op->ParseCmpType();
}

// return the comparison type for the given mdid
IMDType::ECmpType
CUtils::ParseCmpType(CMDAccessor *md_accessor, IMDId *mdid)
{
	const IMDScalarOp *md_scalar_op = md_accessor->RetrieveScOp(mdid);
	return md_scalar_op->ParseCmpType();
}

// check if the expression is a scalar boolean const
BOOL
CUtils::FScalarConstBool(CExpression *pexpr, BOOL value)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (COperator::EopScalarConst == pop->Eopid())
	{
		CScalarConst *popScalarConst = CScalarConst::PopConvert(pop);
		if (IMDType::EtiBool == popScalarConst->GetDatum()->GetDatumType())
		{
			IDatumBool *datum =
				dynamic_cast<IDatumBool *>(popScalarConst->GetDatum());
			return !datum->IsNull() && datum->GetValue() == value;
		}
	}

	return false;
}

BOOL
CUtils::FScalarConstBoolNull(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	if (COperator::EopScalarConst == pop->Eopid())
	{
		CScalarConst *popScalarConst = CScalarConst::PopConvert(pop);
		if (IMDType::EtiBool == popScalarConst->GetDatum()->GetDatumType())
		{
			return popScalarConst->GetDatum()->IsNull();
		}
	}

	return false;
}

// checks to see if the expression is a scalar const TRUE
BOOL
CUtils::FScalarConstTrue(CExpression *pexpr)
{
	return FScalarConstBool(pexpr, true /*value*/);
}

// checks to see if the expression is a scalar const FALSE
BOOL
CUtils::FScalarConstFalse(CExpression *pexpr)
{
	return FScalarConstBool(pexpr, false /*value*/);
}

//	create an array of expression's output columns including a key for grouping
CColRefArray *
CUtils::PdrspqcrGroupingKey(
	CMemoryPool *mp, CExpression *pexpr,
	CColRefArray **
		ppdrspqcrKey	 // output: array of key columns contained in the returned grouping columns
)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != ppdrspqcrKey);

	CKeyCollection *pkc = pexpr->DeriveKeyCollection();
	SPQOS_ASSERT(NULL != pkc);

	CColRefSet *pcrsOutput = pexpr->DeriveOutputColumns();
	CColRefSet *pcrsUsedOuter = SPQOS_NEW(mp) CColRefSet(mp);

	// remove any columns that are not referenced in the query from pcrsOuterOutput
	// filter out system columns since they may introduce columns with undefined sort/hash operators
	CColRefSetIter it(*pcrsOutput);
	while (it.Advance())
	{
		CColRef *pcr = it.Pcr();

		if (CColRef::EUsed == pcr->GetUsage() && !pcr->IsSystemCol())
		{
			pcrsUsedOuter->Include(pcr);
		}
	}

	// prefer extracting a hashable key since Agg operator may redistribute child on grouping columns
	CColRefArray *pdrspqcrKey = pkc->PdrspqcrHashableKey(mp);
	if (NULL == pdrspqcrKey)
	{
		// if no hashable key, extract any key
		pdrspqcrKey = pkc->PdrspqcrKey(mp);
	}
	SPQOS_ASSERT(NULL != pdrspqcrKey);

	CColRefSet *pcrsKey = SPQOS_NEW(mp) CColRefSet(mp, pdrspqcrKey);
	pcrsUsedOuter->Union(pcrsKey);

	pcrsKey->Release();
	CColRefArray *colref_array = pcrsUsedOuter->Pdrspqcr(mp);
	pcrsUsedOuter->Release();

	// set output key array
	*ppdrspqcrKey = pdrspqcrKey;

	return colref_array;
}

// add an equivalence class to the array. If the new equiv class contains
// columns from separate equiv classes, then these are merged. Returns a new
// array of equivalence classes
CColRefSetArray *
CUtils::AddEquivClassToArray(CMemoryPool *mp, const CColRefSet *pcrsNew,
							 const CColRefSetArray *pdrspqcrs)
{
	CColRefSetArray *pdrspqcrsNew = SPQOS_NEW(mp) CColRefSetArray(mp);
	CColRefSet *pcrsCopy = SPQOS_NEW(mp) CColRefSet(mp, *pcrsNew);

	const ULONG length = pdrspqcrs->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefSet *pcrs = (*pdrspqcrs)[ul];
		if (pcrsCopy->IsDisjoint(pcrs))
		{
			pcrs->AddRef();
			pdrspqcrsNew->Append(pcrs);
		}
		else
		{
			pcrsCopy->Include(pcrs);
		}
	}

	pdrspqcrsNew->Append(pcrsCopy);

	return pdrspqcrsNew;
}

// merge 2 arrays of equivalence classes
CColRefSetArray *
CUtils::PdrspqcrsMergeEquivClasses(CMemoryPool *mp, CColRefSetArray *pdrspqcrsFst,
								  CColRefSetArray *pdrspqcrsSnd)
{
	pdrspqcrsFst->AddRef();
	CColRefSetArray *pdrspqcrsMerged = pdrspqcrsFst;

	const ULONG length = pdrspqcrsSnd->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRefSet *pcrs = (*pdrspqcrsSnd)[ul];

		CColRefSetArray *pdrspqcrs =
			AddEquivClassToArray(mp, pcrs, pdrspqcrsMerged);
		pdrspqcrsMerged->Release();
		pdrspqcrsMerged = pdrspqcrs;
	}

	return pdrspqcrsMerged;
}

// intersect 2 arrays of equivalence classes. This is accomplished by using
// a hashmap to find intersects between two arrays of colomun referance sets
CColRefSetArray *
CUtils::PdrspqcrsIntersectEquivClasses(CMemoryPool *mp,
									  CColRefSetArray *pdrspqcrsFst,
									  CColRefSetArray *pdrspqcrsSnd)
{
	SPQOS_ASSERT(CUtils::FEquivalanceClassesDisjoint(mp, pdrspqcrsFst));
	SPQOS_ASSERT(CUtils::FEquivalanceClassesDisjoint(mp, pdrspqcrsSnd));

	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);

	const ULONG ulLenFst = pdrspqcrsFst->Size();
	const ULONG ulLenSnd = pdrspqcrsSnd->Size();

	// nothing to intersect, so return empty array
	if (ulLenSnd == 0 || ulLenFst == 0)
	{
		return pdrspqcrs;
	}

	ColRefToColRefSetMap *phmcscrs = SPQOS_NEW(mp) ColRefToColRefSetMap(mp);
	ColRefToColRefMap *phmcscrDone = SPQOS_NEW(mp) ColRefToColRefMap(mp);

	// populate a hashmap in this loop
	for (ULONG ulFst = 0; ulFst < ulLenFst; ulFst++)
	{
		CColRefSet *pcrsFst = (*pdrspqcrsFst)[ulFst];

		// because the equivalence classes are disjoint, a single column will only be a member
		// of one equivalence class. therefore, we create a hash map keyed on one column
		CColRefSetIter crsi(*pcrsFst);
		while (crsi.Advance())
		{
			CColRef *colref = crsi.Pcr();
			pcrsFst->AddRef();
			phmcscrs->Insert(colref, pcrsFst);
		}
	}

	// probe the hashmap with the equivalence classes
	for (ULONG ulSnd = 0; ulSnd < ulLenSnd; ulSnd++)
	{
		CColRefSet *pcrsSnd = (*pdrspqcrsSnd)[ulSnd];

		// iterate on column references in the equivalence class
		CColRefSetIter crsi(*pcrsSnd);
		while (crsi.Advance())
		{
			// lookup a column in the hashmap
			CColRef *colref = crsi.Pcr();
			CColRefSet *pcrs = phmcscrs->Find(colref);
			CColRef *pcrDone = phmcscrDone->Find(colref);

			// continue if we don't find this column or if that column
			// is already processed and outputed as an intersection of two
			// column referance sets
			if (NULL != pcrs && pcrDone == NULL)
			{
				CColRefSet *pcrsNew = SPQOS_NEW(mp) CColRefSet(mp);
				pcrsNew->Include(pcrsSnd);
				pcrsNew->Intersection(pcrs);
				pdrspqcrs->Append(pcrsNew);

				CColRefSetIter hmpcrs(*pcrsNew);
				// now that we have created an intersection, any additional matches to these
				// columns would create a duplicate intersect, so we add those columns to
				// the DONE hash map
				while (hmpcrs.Advance())
				{
					CColRef *pcrProcessed = hmpcrs.Pcr();
					phmcscrDone->Insert(pcrProcessed, pcrProcessed);
				}
			}
		}
	}
	phmcscrDone->Release();
	phmcscrs->Release();

	return pdrspqcrs;
}

// return a copy of equivalence classes from all children
CColRefSetArray *
CUtils::PdrspqcrsCopyChildEquivClasses(CMemoryPool *mp,
									  CExpressionHandle &exprhdl)
{
	CColRefSetArray *pdrspqcrs = SPQOS_NEW(mp) CColRefSetArray(mp);
	const ULONG arity = exprhdl.Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (!exprhdl.FScalarChild(ul))
		{
			CColRefSetArray *pdrspqcrsChild =
				exprhdl.DerivePropertyConstraint(ul)->PdrspqcrsEquivClasses();

			CColRefSetArray *pdrspqcrsChildCopy =
				SPQOS_NEW(mp) CColRefSetArray(mp);
			const ULONG size = pdrspqcrsChild->Size();
			for (ULONG ulInner = 0; ulInner < size; ulInner++)
			{
				CColRefSet *pcrs =
					SPQOS_NEW(mp) CColRefSet(mp, *(*pdrspqcrsChild)[ulInner]);
				pdrspqcrsChildCopy->Append(pcrs);
			}

			CColRefSetArray *pdrspqcrsMerged =
				PdrspqcrsMergeEquivClasses(mp, pdrspqcrs, pdrspqcrsChildCopy);
			pdrspqcrsChildCopy->Release();
			pdrspqcrs->Release();
			pdrspqcrs = pdrspqcrsMerged;
		}
	}

	return pdrspqcrs;
}

// return a copy of the given array of columns, excluding the columns in the given colrefset
CColRefArray *
CUtils::PdrspqcrExcludeColumns(CMemoryPool *mp, CColRefArray *pdrspqcrOriginal,
							  CColRefSet *pcrsExcluded)
{
	SPQOS_ASSERT(NULL != pdrspqcrOriginal);
	SPQOS_ASSERT(NULL != pcrsExcluded);

	CColRefArray *colref_array = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG num_cols = pdrspqcrOriginal->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *colref = (*pdrspqcrOriginal)[ul];
		if (!pcrsExcluded->FMember(colref))
		{
			colref_array->Append(colref);
		}
	}

	return colref_array;
}

// helper function to print a colref array
IOstream &
CUtils::OsPrintDrgPcr(IOstream &os, const CColRefArray *colref_array,
					  ULONG ulLenMax)
{
	ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < std::min(length, ulLenMax); ul++)
	{
		(*colref_array)[ul]->OsPrint(os);
		if (ul < length - 1)
		{
			os << ", ";
		}
	}

	if (ulLenMax < length)
	{
		os << "...";
	}

	return os;
}

// check if given expression is a scalar comparison
BOOL
CUtils::FScalarCmp(CExpression *pexpr)
{
	return (COperator::EopScalarCmp == pexpr->Pop()->Eopid());
}

// check if given expression is a scalar array comparison
BOOL
CUtils::FScalarArrayCmp(CExpression *pexpr)
{
	return (COperator::EopScalarArrayCmp == pexpr->Pop()->Eopid());
}

// check if given expression has any one stage agg nodes
BOOL
CUtils::FHasOneStagePhysicalAgg(const CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	if (FPhysicalAgg(pexpr->Pop()) &&
		!CPhysicalAgg::PopConvert(pexpr->Pop())->FMultiStage())
	{
		return true;
	}

	// recursively check children
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (FHasOneStagePhysicalAgg((*pexpr)[ul]))
		{
			return true;
		}
	}

	return false;
}

// check if given operator exists in the given list
BOOL
CUtils::FOpExists(const COperator *pop, const COperator::EOperatorId *peopid,
				  ULONG ulOps)
{
	SPQOS_ASSERT(NULL != pop);
	SPQOS_ASSERT(NULL != peopid);

	COperator::EOperatorId op_id = pop->Eopid();
	for (ULONG ul = 0; ul < ulOps; ul++)
	{
		if (op_id == peopid[ul])
		{
			return true;
		}
	}

	return false;
}

// check if given expression has any operator in the given list
BOOL
CUtils::FHasOp(const CExpression *pexpr, const COperator::EOperatorId *peopid,
			   ULONG ulOps)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != peopid);

	if (FOpExists(pexpr->Pop(), peopid, ulOps))
	{
		return true;
	}

	// recursively check children
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (FHasOp((*pexpr)[ul], peopid, ulOps))
		{
			return true;
		}
	}

	return false;
}

// return number of inlinable CTEs in the given expression
ULONG
CUtils::UlInlinableCTEs(CExpression *pexpr, ULONG ulDepth)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();

	if (COperator::EopLogicalCTEConsumer == pop->Eopid())
	{
		CLogicalCTEConsumer *popConsumer = CLogicalCTEConsumer::PopConvert(pop);
		CExpression *pexprProducer =
			COptCtxt::PoctxtFromTLS()->Pcteinfo()->PexprCTEProducer(
				popConsumer->UlCTEId());
		SPQOS_ASSERT(NULL != pexprProducer);
		return ulDepth + UlInlinableCTEs(pexprProducer, ulDepth + 1);
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	ULONG ulChildCTEs = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ulChildCTEs += UlInlinableCTEs((*pexpr)[ul], ulDepth);
	}

	return ulChildCTEs;
}

// return number of joins in the given expression
ULONG
CUtils::UlJoins(CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	ULONG ulJoins = 0;
	COperator *pop = pexpr->Pop();

	if (COperator::EopLogicalCTEConsumer == pop->Eopid())
	{
		CLogicalCTEConsumer *popConsumer = CLogicalCTEConsumer::PopConvert(pop);
		CExpression *pexprProducer =
			COptCtxt::PoctxtFromTLS()->Pcteinfo()->PexprCTEProducer(
				popConsumer->UlCTEId());
		return UlJoins(pexprProducer);
	}

	if (FLogicalJoin(pop) || FPhysicalJoin(pop))
	{
		ulJoins = 1;
		if (COperator::EopLogicalNAryJoin == pop->Eopid())
		{
			// N-Ary join is equivalent to a cascade of (Arity - 2) binary joins
			ulJoins = pexpr->Arity() - 2;
		}
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	ULONG ulChildJoins = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ulChildJoins += UlJoins((*pexpr)[ul]);
	}

	return ulJoins + ulChildJoins;
}

// return number of subqueries in the given expression
ULONG
CUtils::UlSubqueries(CExpression *pexpr)
{
	SPQOS_CHECK_STACK_SIZE;
	SPQOS_ASSERT(NULL != pexpr);

	ULONG ulSubqueries = 0;
	COperator *pop = pexpr->Pop();

	if (COperator::EopLogicalCTEConsumer == pop->Eopid())
	{
		CLogicalCTEConsumer *popConsumer = CLogicalCTEConsumer::PopConvert(pop);
		CExpression *pexprProducer =
			COptCtxt::PoctxtFromTLS()->Pcteinfo()->PexprCTEProducer(
				popConsumer->UlCTEId());
		return UlSubqueries(pexprProducer);
	}

	if (FSubquery(pop))
	{
		ulSubqueries = 1;
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	ULONG ulChildSubqueries = 0;
	for (ULONG ul = 0; ul < arity; ul++)
	{
		ulChildSubqueries += UlSubqueries((*pexpr)[ul]);
	}

	return ulSubqueries + ulChildSubqueries;
}

// check if given expression is a scalar boolean operator
BOOL
CUtils::FScalarBoolOp(CExpression *pexpr)
{
	return (COperator::EopScalarBoolOp == pexpr->Pop()->Eopid());
}

// is the given expression a scalar bool op of the passed type?
BOOL
CUtils::FScalarBoolOp(CExpression *pexpr, CScalarBoolOp::EBoolOperator eboolop)
{
	SPQOS_ASSERT(NULL != pexpr);

	COperator *pop = pexpr->Pop();
	return pop->FScalar() && COperator::EopScalarBoolOp == pop->Eopid() &&
		   eboolop == CScalarBoolOp::PopConvert(pop)->Eboolop();
}

// check if given expression is a scalar null test
BOOL
CUtils::FScalarNullTest(CExpression *pexpr)
{
	return (COperator::EopScalarNullTest == pexpr->Pop()->Eopid());
}

// check if given expression is a NOT NULL predicate
BOOL
CUtils::FScalarNotNull(CExpression *pexpr)
{
	return FScalarBoolOp(pexpr, CScalarBoolOp::EboolopNot) &&
		   FScalarNullTest((*pexpr)[0]);
}

// check if given expression is a scalar identifier
BOOL
CUtils::FScalarIdent(CExpression *pexpr)
{
	return (COperator::EopScalarIdent == pexpr->Pop()->Eopid());
}

BOOL
CUtils::FScalarIdentIgnoreCast(CExpression *pexpr)
{
	return (COperator::EopScalarIdent == pexpr->Pop()->Eopid() ||
			CScalarIdent::FCastedScId(pexpr));
}

// check if given expression is a scalar boolean identifier
BOOL
CUtils::FScalarIdentBoolType(CExpression *pexpr)
{
	return FScalarIdent(pexpr) &&
		   IMDType::EtiBool == CScalarIdent::PopConvert(pexpr->Pop())
								   ->Pcr()
								   ->RetrieveType()
								   ->GetDatumType();
}

// check if given expression is a scalar array
BOOL
CUtils::FScalarArray(CExpression *pexpr)
{
	return (COperator::EopScalarArray == pexpr->Pop()->Eopid());
}

// check if given expression is a scalar array coerce
BOOL
CUtils::FScalarArrayCoerce(CExpression *pexpr)
{
	return (COperator::EopScalarArrayCoerceExpr == pexpr->Pop()->Eopid());
}

// is the given expression a scalar identifier with the given column reference
BOOL
CUtils::FScalarIdent(CExpression *pexpr, CColRef *colref)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != colref);

	return COperator::EopScalarIdent == pexpr->Pop()->Eopid() &&
		   CScalarIdent::PopConvert(pexpr->Pop())->Pcr() == colref;
}

// check if the expression is a scalar const
BOOL
CUtils::FScalarConst(CExpression *pexpr)
{
	return (COperator::EopScalarConst == pexpr->Pop()->Eopid());
}

// check if the expression is variable-free
BOOL
CUtils::FVarFreeExpr(CExpression *pexpr)
{
	SPQOS_ASSERT(pexpr->Pop()->FScalar());

	if (FScalarConst(pexpr))
	{
		return true;
	}

	if (pexpr->DeriveHasSubquery())
	{
		return false;
	}

	SPQOS_ASSERT(0 == pexpr->DeriveDefinedColumns()->Size());
	CColRefSet *pcrsUsed = pexpr->DeriveUsedColumns();

	// no variables in expression
	return 0 == pcrsUsed->Size();
}

// check if the expression is a scalar predicate, i.e. bool op, comparison, or null test
BOOL
CUtils::FPredicate(CExpression *pexpr)
{
	COperator *pop = pexpr->Pop();
	return pop->FScalar() &&
		   (FScalarCmp(pexpr) || CPredicateUtils::FIDF(pexpr) ||
			FScalarArrayCmp(pexpr) || FScalarBoolOp(pexpr) ||
			FScalarNullTest(pexpr) ||
			CLogical::EopScalarNAryJoinPredList == pop->Eopid());
}

// checks that the given type has all the comparisons: Eq, NEq, L, LEq, G, GEq.
BOOL
CUtils::FHasAllDefaultComparisons(const IMDType *pmdtype)
{
	SPQOS_ASSERT(NULL != pmdtype);

	return IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptEq)) &&
		   IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptNEq)) &&
		   IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptL)) &&
		   IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptLEq)) &&
		   IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptG)) &&
		   IMDId::IsValid(pmdtype->GetMdidForCmpType(IMDType::EcmptGEq));
}

// determine whether a type is supported for use in contradiction detection.
// The assumption is that we only compare data of the same type.
BOOL
CUtils::FConstrainableType(IMDId *mdid_type)
{
	if (FIntType(mdid_type))
	{
		return true;
	}
	if (!SPQOS_FTRACE(EopttraceEnableConstantExpressionEvaluation))
	{
		return false;
	}
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDType *pmdtype = md_accessor->RetrieveType(mdid_type);

	return FHasAllDefaultComparisons(pmdtype);
}

// determine whether a type is an integer type
BOOL
CUtils::FIntType(IMDId *mdid_type)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDType::ETypeInfo type_info =
		md_accessor->RetrieveType(mdid_type)->GetDatumType();

	return (IMDType::EtiInt2 == type_info || IMDType::EtiInt4 == type_info ||
			IMDType::EtiInt8 == type_info);
}

// check if a binary operator uses only columns produced by its children
BOOL
CUtils::FUsesChildColsOnly(CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(3 == exprhdl.Arity());

	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CColRefSet *pcrsUsed = exprhdl.DeriveUsedColumns(2);
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(exprhdl.DeriveOutputColumns(0 /*child_index*/));
	pcrs->Include(exprhdl.DeriveOutputColumns(1 /*child_index*/));
	BOOL fUsesChildCols = pcrs->ContainsAll(pcrsUsed);
	pcrs->Release();

	return fUsesChildCols;
}

// check if inner child of a binary operator uses columns not produced by outer child
BOOL
CUtils::FInnerUsesExternalCols(CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(3 == exprhdl.Arity());

	CColRefSet *outer_refs = exprhdl.DeriveOuterReferences(1 /*child_index*/);
	if (0 == outer_refs->Size())
	{
		return false;
	}
	CColRefSet *pcrsOutput = exprhdl.DeriveOutputColumns(0 /*child_index*/);

	return !pcrsOutput->ContainsAll(outer_refs);
}

// check if inner child of a binary operator uses only columns not produced by outer child
BOOL
CUtils::FInnerUsesExternalColsOnly(CExpressionHandle &exprhdl)
{
	return FInnerUsesExternalCols(exprhdl) &&
		   exprhdl.DeriveOuterReferences(1)->IsDisjoint(
			   exprhdl.DeriveOutputColumns(0));
}

// check if given columns have available comparison operators
BOOL
CUtils::FComparisonPossible(CColRefArray *colref_array,
							IMDType::ECmpType cmp_type)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(0 < colref_array->Size());

	const ULONG size = colref_array->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		const IMDType *pmdtype = colref->RetrieveType();
		if (!IMDId::IsValid(pmdtype->GetMdidForCmpType(cmp_type)))
		{
			return false;
		}
	}

	return true;
}

// counts the number of times a certain operator appears
ULONG
CUtils::UlCountOperator(const CExpression *pexpr, COperator::EOperatorId op_id)
{
	ULONG ulOpCnt = 0;
	if (op_id == pexpr->Pop()->Eopid())
	{
		ulOpCnt += 1;
	}

	const ULONG arity = pexpr->Arity();
	for (ULONG ulChild = 0; ulChild < arity; ulChild++)
	{
		ulOpCnt += UlCountOperator((*pexpr)[ulChild], op_id);
	}
	return ulOpCnt;
}

// return the max subset of redistributable columns for the given columns
CColRefArray *
CUtils::PdrspqcrRedistributableSubset(CMemoryPool *mp,
									 CColRefArray *colref_array)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(0 < colref_array->Size());

	CColRefArray *pdrspqcrRedist = SPQOS_NEW(mp) CColRefArray(mp);
	const ULONG size = colref_array->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		const IMDType *pmdtype = colref->RetrieveType();
		if (pmdtype->IsRedistributable())
		{
			pdrspqcrRedist->Append(colref);
		}
	}

	return pdrspqcrRedist;
}

// check if hashing is possible for the given columns
BOOL
CUtils::IsHashable(CColRefArray *colref_array)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(0 < colref_array->Size());

	const ULONG size = colref_array->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		const IMDType *pmdtype = colref->RetrieveType();
		if (!pmdtype->IsHashable())
		{
			return false;
		}
	}

	return true;
}

// check if given operator is a logical DML operator
BOOL
CUtils::FLogicalDML(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	COperator::EOperatorId op_id = pop->Eopid();
	return COperator::EopLogicalDML == op_id ||
		   COperator::EopLogicalInsert == op_id ||
		   COperator::EopLogicalDelete == op_id ||
		   COperator::EopLogicalUpdate == op_id;
}

// return regular string from wide-character string
CHAR *
CUtils::CreateMultiByteCharStringFromWCString(CMemoryPool *mp, WCHAR *wsz)
{
	ULONG ulMaxLength = SPQOS_WSZ_LENGTH(wsz) * SPQOS_SIZEOF(WCHAR) + 1;
	CHAR *sz = SPQOS_NEW_ARRAY(mp, CHAR, ulMaxLength);
	clib::Wcstombs(sz, wsz, ulMaxLength);
	sz[ulMaxLength - 1] = '\0';

	return sz;
}

// construct an array of colids from the given array of column references
ULongPtrArray *
CUtils::Pdrspqul(CMemoryPool *mp, const CColRefArray *colref_array)
{
	ULongPtrArray *pdrspqul = SPQOS_NEW(mp) ULongPtrArray(mp);

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		ULONG *pul = SPQOS_NEW(mp) ULONG(colref->Id());
		pdrspqul->Append(pul);
	}

	return pdrspqul;
}

// generate a timestamp-based filename in the provided buffer.
void
CUtils::GenerateFileName(CHAR *buf, const CHAR *szPrefix, const CHAR *szExt,
						 ULONG length, ULONG ulSessionId, ULONG ulCmdId)
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();

	CWStringDynamic *filename_template = SPQOS_NEW(mp) CWStringDynamic(mp);
	COstreamString oss(filename_template);
	oss << szPrefix << "_%04d%02d%02d_%02d%02d%02d_%d_%d." << szExt;

	const WCHAR *wszFileNameTemplate = filename_template->GetBuffer();
	SPQOS_ASSERT(length >= SPQOS_FILE_NAME_BUF_SIZE);

	TIMEVAL tv;
	TIME tm;

	// get local time
	syslib::GetTimeOfDay(&tv, NULL /*timezone*/);
#ifdef SPQOS_DEBUG
	TIME *ptm =
#endif	// SPQOS_DEBUG
		clib::Localtime_r(&tv.tv_sec, &tm);

	SPQOS_ASSERT(NULL != ptm && "Failed to get local time");

	WCHAR wszBuf[SPQOS_FILE_NAME_BUF_SIZE];
	CWStringStatic str(wszBuf, SPQOS_ARRAY_SIZE(wszBuf));

	str.AppendFormat(wszFileNameTemplate, tm.tm_year + 1900, tm.tm_mon + 1,
					 tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ulSessionId,
					 ulCmdId);

	INT iResult = (INT) clib::Wcstombs(buf, wszBuf, length);

	SPQOS_RTL_ASSERT((INT) 0 < iResult && iResult <= (INT) str.Length());

	buf[length - 1] = '\0';

	SPQOS_DELETE(filename_template);
}

// return the mapping of the given colref based on the given hashmap
CColRef *
CUtils::PcrRemap(const CColRef *colref, UlongToColRefMap *colref_mapping,
				 BOOL
#ifdef SPQOS_DEBUG
					 must_exist
#endif	//SPQOS_DEBUG
)
{
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(NULL != colref_mapping);

	ULONG id = colref->Id();
	CColRef *pcrMapped = colref_mapping->Find(&id);

	if (NULL != pcrMapped)
	{
		SPQOS_ASSERT(colref != pcrMapped);
		return pcrMapped;
	}

	SPQOS_ASSERT(!must_exist && "Column does not exist in hashmap");
	return const_cast<CColRef *>(colref);
}

// create a new colrefset corresponding to the given colrefset and based on the given mapping
CColRefSet *
CUtils::PcrsRemap(CMemoryPool *mp, CColRefSet *pcrs,
				  UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	SPQOS_ASSERT(NULL != pcrs);
	SPQOS_ASSERT(NULL != colref_mapping);

	CColRefSet *pcrsMapped = SPQOS_NEW(mp) CColRefSet(mp);

	CColRefSetIter crsi(*pcrs);
	while (crsi.Advance())
	{
		CColRef *colref = crsi.Pcr();
		CColRef *pcrMapped = PcrRemap(colref, colref_mapping, must_exist);
		pcrsMapped->Include(pcrMapped);
	}

	return pcrsMapped;
}

// create an array of column references corresponding to the given array
// and based on the given mapping
CColRefArray *
CUtils::PdrspqcrRemap(CMemoryPool *mp, CColRefArray *colref_array,
					 UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(NULL != colref_mapping);

	CColRefArray *pdrspqcrNew = SPQOS_NEW(mp) CColRefArray(mp);

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		CColRef *pcrMapped = PcrRemap(colref, colref_mapping, must_exist);
		pdrspqcrNew->Append(pcrMapped);
	}

	return pdrspqcrNew;
}

// ceate an array of column references corresponding to the given array
// and based on the given mapping. Create new colrefs if necessary
CColRefArray *
CUtils::PdrspqcrRemapAndCreate(CMemoryPool *mp, CColRefArray *colref_array,
							  UlongToColRefMap *colref_mapping)
{
	SPQOS_ASSERT(NULL != colref_array);
	SPQOS_ASSERT(NULL != colref_mapping);

	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CColRefArray *pdrspqcrNew = SPQOS_NEW(mp) CColRefArray(mp);

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		ULONG id = colref->Id();
		CColRef *pcrMapped = colref_mapping->Find(&id);
		if (NULL == pcrMapped)
		{
			// not found in hashmap, so create a new colref and add to hashmap
			pcrMapped = col_factory->PcrCopy(colref);

#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(id), pcrMapped);
			SPQOS_ASSERT(result);
		}

		pdrspqcrNew->Append(pcrMapped);
	}

	return pdrspqcrNew;
}

// create an array of column arrays corresponding to the given array
// and based on the given mapping
CColRef2dArray *
CUtils::PdrspqdrspqcrRemap(CMemoryPool *mp, CColRef2dArray *pdrspqdrspqcr,
						 UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcr);
	SPQOS_ASSERT(NULL != colref_mapping);

	CColRef2dArray *pdrspqdrspqcrNew = SPQOS_NEW(mp) CColRef2dArray(mp);

	const ULONG arity = pdrspqdrspqcr->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColRefArray *colref_array =
			PdrspqcrRemap(mp, (*pdrspqdrspqcr)[ul], colref_mapping, must_exist);
		pdrspqdrspqcrNew->Append(colref_array);
	}

	return pdrspqdrspqcrNew;
}

// remap given array of expressions with provided column mappings
CExpressionArray *
CUtils::PdrspqexprRemap(CMemoryPool *mp, CExpressionArray *pdrspqexpr,
					   UlongToColRefMap *colref_mapping)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	SPQOS_ASSERT(NULL != colref_mapping);

	CExpressionArray *pdrspqexprNew = SPQOS_NEW(mp) CExpressionArray(mp);

	const ULONG arity = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		CExpression *pexpr = (*pdrspqexpr)[ul];
		pdrspqexprNew->Append(pexpr->PexprCopyWithRemappedColumns(
			mp, colref_mapping, true /*must_exist*/));
	}

	return pdrspqexprNew;
}

// create col ID->ColRef mapping using the given ColRef arrays
UlongToColRefMap *
CUtils::PhmulcrMapping(CMemoryPool *mp, CColRefArray *pdrspqcrFrom,
					   CColRefArray *pdrspqcrTo)
{
	SPQOS_ASSERT(NULL != pdrspqcrFrom);
	SPQOS_ASSERT(NULL != pdrspqcrTo);

	UlongToColRefMap *colref_mapping = SPQOS_NEW(mp) UlongToColRefMap(mp);
	AddColumnMapping(mp, colref_mapping, pdrspqcrFrom, pdrspqcrTo);

	return colref_mapping;
}

// add col ID->ColRef mappings to the given hashmap based on the given ColRef arrays
void
CUtils::AddColumnMapping(CMemoryPool *mp, UlongToColRefMap *colref_mapping,
						 CColRefArray *pdrspqcrFrom, CColRefArray *pdrspqcrTo)
{
	SPQOS_ASSERT(NULL != colref_mapping);
	SPQOS_ASSERT(NULL != pdrspqcrFrom);
	SPQOS_ASSERT(NULL != pdrspqcrTo);

	const ULONG ulColumns = pdrspqcrFrom->Size();
	SPQOS_ASSERT(ulColumns == pdrspqcrTo->Size());

	for (ULONG ul = 0; ul < ulColumns; ul++)
	{
		CColRef *pcrFrom = (*pdrspqcrFrom)[ul];
		ULONG ulFromId = pcrFrom->Id();
		CColRef *pcrTo = (*pdrspqcrTo)[ul];
		SPQOS_ASSERT(pcrFrom != pcrTo);

#ifdef SPQOS_DEBUG
		BOOL result = false;
#endif	// SPQOS_DEBUG
		CColRef *pcrExist = colref_mapping->Find(&ulFromId);
		if (NULL == pcrExist)
		{
#ifdef SPQOS_DEBUG
			result =
#endif	// SPQOS_DEBUG
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(ulFromId), pcrTo);
			SPQOS_ASSERT(result);
		}
		else
		{
#ifdef SPQOS_DEBUG
			result =
#endif	// SPQOS_DEBUG
				colref_mapping->Replace(&ulFromId, pcrTo);
		}
		SPQOS_ASSERT(result);
	}
}

// create a copy of the array of column references
CColRefArray *
CUtils::PdrspqcrExactCopy(CMemoryPool *mp, CColRefArray *colref_array)
{
	CColRefArray *pdrspqcrNew = SPQOS_NEW(mp) CColRefArray(mp);

	const ULONG num_cols = colref_array->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		pdrspqcrNew->Append(colref);
	}

	return pdrspqcrNew;
}

// Create an array of new column references with the same names and types
// as the given column references. If the passed map is not null, mappings
// from old to copied variables are added to it.
CColRefArray *
CUtils::PdrspqcrCopy(CMemoryPool *mp, CColRefArray *colref_array,
					BOOL fAllComputed, UlongToColRefMap *colref_mapping)
{
	// get column factory from optimizer context object
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

	CColRefArray *pdrspqcrNew = SPQOS_NEW(mp) CColRefArray(mp);

	const ULONG num_cols = colref_array->Size();
	for (ULONG ul = 0; ul < num_cols; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		CColRef *new_colref = NULL;
		if (fAllComputed)
		{
			new_colref = col_factory->PcrCreate(colref);
		}
		else
		{
			new_colref = col_factory->PcrCopy(colref);
		}

		pdrspqcrNew->Append(new_colref);
		if (NULL != colref_mapping)
		{
#ifdef SPQOS_DEBUG
			BOOL fInserted =
#endif
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(colref->Id()),
									   new_colref);
			SPQOS_ASSERT(fInserted);
		}
	}

	return pdrspqcrNew;
}

// equality check between two arrays of column refs. Inputs can be NULL
BOOL
CUtils::Equals(CColRefArray *pdrspqcrFst, CColRefArray *pdrspqcrSnd)
{
	if (NULL == pdrspqcrFst || NULL == pdrspqcrSnd)
	{
		return (NULL == pdrspqcrFst && NULL == pdrspqcrSnd);
	}

	return pdrspqcrFst->Equals(pdrspqcrSnd);
}

// compute hash value for an array of column references
ULONG
CUtils::UlHashColArray(const CColRefArray *colref_array, const ULONG ulMaxCols)
{
	ULONG ulHash = 0;

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length && ul < ulMaxCols; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		ulHash = spqos::CombineHashes(ulHash, spqos::HashPtr<CColRef>(colref));
	}

	return ulHash;
}

// Return the set of column reference from the CTE Producer corresponding to the
// subset of input columns from the CTE Consumer
CColRefSet *
CUtils::PcrsCTEProducerColumns(CMemoryPool *mp, CColRefSet *pcrsInput,
							   CLogicalCTEConsumer *popCTEConsumer)
{
	CExpression *pexprProducer =
		COptCtxt::PoctxtFromTLS()->Pcteinfo()->PexprCTEProducer(
			popCTEConsumer->UlCTEId());
	SPQOS_ASSERT(NULL != pexprProducer);
	CLogicalCTEProducer *popProducer =
		CLogicalCTEProducer::PopConvert(pexprProducer->Pop());

	CColRefArray *pdrspqcrInput = pcrsInput->Pdrspqcr(mp);
	UlongToColRefMap *colref_mapping =
		PhmulcrMapping(mp, popCTEConsumer->Pdrspqcr(), popProducer->Pdrspqcr());
	CColRefArray *pdrspqcrOutput =
		PdrspqcrRemap(mp, pdrspqcrInput, colref_mapping, true /*must_exist*/);

	CColRefSet *pcrsCTEProducer = SPQOS_NEW(mp) CColRefSet(mp);
	pcrsCTEProducer->Include(pdrspqcrOutput);

	colref_mapping->Release();
	pdrspqcrInput->Release();
	pdrspqcrOutput->Release();

	return pcrsCTEProducer;
}

// Construct the join condition (AND-tree) of INDF condition
// from the array of input column reference arrays
CExpression *
CUtils::PexprConjINDFCond(CMemoryPool *mp, CColRef2dArray *pdrspqdrspqcrInput)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcrInput);
	SPQOS_ASSERT(2 == pdrspqdrspqcrInput->Size());

	// assemble the new scalar condition
	CExpression *pexprScCond = NULL;
	const ULONG length = (*pdrspqdrspqcrInput)[0]->Size();
	SPQOS_ASSERT(0 != length);
	SPQOS_ASSERT(length == (*pdrspqdrspqcrInput)[1]->Size());

	CExpressionArray *pdrspqexprInput =
		SPQOS_NEW(mp) CExpressionArray(mp, length);
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *pcrLeft = (*(*pdrspqdrspqcrInput)[0])[ul];
		CColRef *pcrRight = (*(*pdrspqdrspqcrInput)[1])[ul];
		SPQOS_ASSERT(pcrLeft != pcrRight);

		pdrspqexprInput->Append(CUtils::PexprNegate(
			mp, CUtils::PexprIDF(mp, CUtils::PexprScalarIdent(mp, pcrLeft),
								 CUtils::PexprScalarIdent(mp, pcrRight))));
	}

	pexprScCond = CPredicateUtils::PexprConjunction(mp, pdrspqexprInput);

	return pexprScCond;
}

// return index of the set containing given column; if column is not found, return spqos::ulong_max
ULONG
CUtils::UlPcrIndexContainingSet(CColRefSetArray *pdrspqcrs,
								const CColRef *colref)
{
	SPQOS_ASSERT(NULL != pdrspqcrs);

	const ULONG size = pdrspqcrs->Size();
	for (ULONG ul = 0; ul < size; ul++)
	{
		CColRefSet *pcrs = (*pdrspqcrs)[ul];
		if (pcrs->FMember(colref))
		{
			return ul;
		}
	}

	return spqos::ulong_max;
}

// cast the input expression to the destination mdid
CExpression *
CUtils::PexprCast(CMemoryPool *mp, CMDAccessor *md_accessor, CExpression *pexpr,
				  IMDId *mdid_dest)
{
	SPQOS_ASSERT(NULL != mdid_dest);
	IMDId *mdid_src = CScalar::PopConvert(pexpr->Pop())->MdidType();
	SPQOS_ASSERT(
		CMDAccessorUtils::FCastExists(md_accessor, mdid_src, mdid_dest));

	const IMDCast *pmdcast = md_accessor->Pmdcast(mdid_src, mdid_dest);

	mdid_dest->AddRef();
	pmdcast->GetCastFuncMdId()->AddRef();
	CExpression *pexprCast;

	if (pmdcast->GetMDPathType() == IMDCast::EmdtArrayCoerce)
	{
		CMDArrayCoerceCastSPQDB *parrayCoerceCast =
			(CMDArrayCoerceCastSPQDB *) pmdcast;
		pexprCast = SPQOS_NEW(mp) CExpression(
			mp,
			SPQOS_NEW(mp) CScalarArrayCoerceExpr(
				mp, parrayCoerceCast->GetCastFuncMdId(), mdid_dest,
				parrayCoerceCast->TypeModifier(),
				parrayCoerceCast->IsExplicit(),
				(COperator::ECoercionForm) parrayCoerceCast->GetCoercionForm(),
				parrayCoerceCast->Location()),
			pexpr);
	}
	else if (pmdcast->GetMDPathType() == IMDCast::EmdtCoerceViaIO)
	{
		CScalarCoerceViaIO *op = SPQOS_NEW(mp)
			CScalarCoerceViaIO(mp, mdid_dest, default_type_modifier,
							   COperator::EcfImplicitCast, -1 /* location */);
		pexprCast = SPQOS_NEW(mp) CExpression(mp, op, pexpr);
	}
	else
	{
		CScalarCast *popCast =
			SPQOS_NEW(mp) CScalarCast(mp, mdid_dest, pmdcast->GetCastFuncMdId(),
									 pmdcast->IsBinaryCoercible());
		pexprCast = SPQOS_NEW(mp) CExpression(mp, popCast, pexpr);
	}

	return pexprCast;
}

// check whether a colref array contains repeated items
BOOL
CUtils::FHasDuplicates(const CColRefArray *colref_array)
{
	CAutoMemoryPool amp;
	CMemoryPool *mp = amp.Pmp();
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		if (pcrs->FMember(colref))
		{
			pcrs->Release();
			return true;
		}

		pcrs->Include(colref);
	}

	pcrs->Release();
	return false;
}

// construct a logical join expression operator of the given type, with the given children
CExpression *
CUtils::PexprLogicalJoin(CMemoryPool *mp, EdxlJoinType edxljointype,
						 CExpressionArray *pdrspqexpr)
{
	COperator *pop = NULL;
	switch (edxljointype)
	{
		case EdxljtInner:
			pop = SPQOS_NEW(mp) CLogicalNAryJoin(mp);
			break;

		case EdxljtLeft:
			pop = SPQOS_NEW(mp) CLogicalLeftOuterJoin(mp);
			break;

		case EdxljtLeftAntiSemijoin:
			pop = SPQOS_NEW(mp) CLogicalLeftAntiSemiJoin(mp);
			break;

		case EdxljtLeftAntiSemijoinNotIn:
			pop = SPQOS_NEW(mp) CLogicalLeftAntiSemiJoinNotIn(mp);
			break;

		case EdxljtFull:
			pop = SPQOS_NEW(mp) CLogicalFullOuterJoin(mp);
			break;

		default:
			SPQOS_ASSERT(!"Unsupported join type");
	}

	return SPQOS_NEW(mp) CExpression(mp, pop, pdrspqexpr);
}

// construct an array of scalar ident expressions from the given array of column references
CExpressionArray *
CUtils::PdrspqexprScalarIdents(CMemoryPool *mp, CColRefArray *colref_array)
{
	SPQOS_ASSERT(NULL != colref_array);

	CExpressionArray *pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
	const ULONG length = colref_array->Size();

	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		CExpression *pexpr = PexprScalarIdent(mp, colref);
		pdrspqexpr->Append(pexpr);
	}

	return pdrspqexpr;
}

// return used columns by expressions in the given array
CColRefSet *
CUtils::PcrsExtractColumns(CMemoryPool *mp, const CExpressionArray *pdrspqexpr)
{
	SPQOS_ASSERT(NULL != pdrspqexpr);
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	const ULONG length = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CExpression *pexpr = (*pdrspqexpr)[ul];
		pcrs->Include(pexpr->DeriveUsedColumns());
	}

	return pcrs;
}

// Create a hashmap of constraints corresponding to a bool const on the given partkeys
// true - unbounded intervals with nulls
// false - empty intervals with no nulls
UlongToConstraintMap *
CUtils::PhmulcnstrBoolConstOnPartKeys(CMemoryPool *mp,
									  CColRef2dArray *pdrspqdrspqcrPartKey,
									  BOOL value)
{
	SPQOS_ASSERT(NULL != pdrspqdrspqcrPartKey);
	UlongToConstraintMap *phmulcnstr = SPQOS_NEW(mp) UlongToConstraintMap(mp);

	const ULONG ulLevels = pdrspqdrspqcrPartKey->Size();
	for (ULONG ul = 0; ul < ulLevels; ul++)
	{
		CColRef *pcrPartKey = PcrExtractPartKey(pdrspqdrspqcrPartKey, ul);
		CConstraint *pcnstr = NULL;
		if (value)
		{
			// unbounded constraint
			pcnstr = CConstraintInterval::PciUnbounded(mp, pcrPartKey,
													   true /*is_null*/);
		}
		else
		{
			// empty constraint (contradiction)
			pcnstr = SPQOS_NEW(mp) CConstraintInterval(
				mp, pcrPartKey, SPQOS_NEW(mp) CRangeArray(mp),
				false /*is_null*/);
		}

		if (NULL != pcnstr)
		{
#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				phmulcnstr->Insert(SPQOS_NEW(mp) ULONG(ul), pcnstr);
			SPQOS_ASSERT(result);
		}
	}

	return phmulcnstr;
}

// returns a new bitset of the given length, where all the bits are set
CBitSet *
CUtils::PbsAllSet(CMemoryPool *mp, ULONG size)
{
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp, size);
	for (ULONG ul = 0; ul < size; ul++)
	{
		pbs->ExchangeSet(ul);
	}

	return pbs;
}

// returns a new bitset, setting the bits in the given array
CBitSet *
CUtils::Pbs(CMemoryPool *mp, ULongPtrArray *pdrspqul)
{
	SPQOS_ASSERT(NULL != pdrspqul);
	CBitSet *pbs = SPQOS_NEW(mp) CBitSet(mp);

	const ULONG length = pdrspqul->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		ULONG *pul = (*pdrspqul)[ul];
		pbs->ExchangeSet(*pul);
	}

	return pbs;
}

// extract part constraint from metadata
CPartConstraint *
CUtils::PpartcnstrFromMDPartCnstr(CMemoryPool *mp, CMDAccessor *md_accessor,
								  CColRef2dArray *pdrspqdrspqcrPartKey,
								  const IMDPartConstraint *mdpart_constraint,
								  CColRefArray *pdrspqcrOutput,
								  BOOL fDummyConstraint)
{
	if (fDummyConstraint)
	{
		return SPQOS_NEW(mp) CPartConstraint(true /*fUninterpreted*/);
	}

	SPQOS_ASSERT(NULL != pdrspqdrspqcrPartKey);

	const ULONG ulLevels = pdrspqdrspqcrPartKey->Size();

	CExpression *pexprPartCnstr = mdpart_constraint->GetPartConstraintExpr(
		mp, md_accessor, pdrspqcrOutput);

	UlongToConstraintMap *phmulcnstr = NULL;
	if (CUtils::FScalarConstTrue(pexprPartCnstr))
	{
		phmulcnstr = PhmulcnstrBoolConstOnPartKeys(mp, pdrspqdrspqcrPartKey,
												   true /*value*/);
	}
	else if (CUtils::FScalarConstFalse(pexprPartCnstr))
	{
		// contradiction
		phmulcnstr = PhmulcnstrBoolConstOnPartKeys(mp, pdrspqdrspqcrPartKey,
												   false /*value*/);
	}
	else
	{
		CColRefSetArray *pdrspqcrs = NULL;
		CConstraint *pcnstr =
			CConstraint::PcnstrFromScalarExpr(mp, pexprPartCnstr, &pdrspqcrs);
		CRefCount::SafeRelease(pdrspqcrs);

		phmulcnstr = SPQOS_NEW(mp) UlongToConstraintMap(mp);
		for (ULONG ul = 0; ul < ulLevels && NULL != pcnstr; ul++)
		{
			CColRef *pcrPartKey = PcrExtractPartKey(pdrspqdrspqcrPartKey, ul);
			CConstraint *pcnstrLevel = pcnstr->Pcnstr(mp, pcrPartKey);
			if (NULL == pcnstrLevel)
			{
				pcnstrLevel = CConstraintInterval::PciUnbounded(
					mp, pcrPartKey, true /*is_null*/);
			}

			if (NULL != pcnstrLevel)
			{
#ifdef SPQOS_DEBUG
				BOOL result =
#endif	// SPQOS_DEBUG
					phmulcnstr->Insert(SPQOS_NEW(mp) ULONG(ul), pcnstrLevel);
				SPQOS_ASSERT(result);
			}
		}
		CRefCount::SafeRelease(pcnstr);
	}

	pexprPartCnstr->Release();

	CBitSet *pbsDefaultParts =
		Pbs(mp, mdpart_constraint->GetDefaultPartsArray());
	pdrspqdrspqcrPartKey->AddRef();

	return SPQOS_NEW(mp) CPartConstraint(
		mp, phmulcnstr, pbsDefaultParts,
		mdpart_constraint->IsConstraintUnbounded(), pdrspqdrspqcrPartKey);
}

// Helper to create a dummy constant table expression;
// the table has one boolean column with value True and one row
CExpression *
CUtils::PexprLogicalCTGDummy(CMemoryPool *mp)
{
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();

	// generate a bool column
	const IMDTypeBool *pmdtypebool = md_accessor->PtMDType<IMDTypeBool>();
	CColRef *colref =
		col_factory->PcrCreate(pmdtypebool, default_type_modifier);
	CColRefArray *pdrspqcrCTG = SPQOS_NEW(mp) CColRefArray(mp);
	pdrspqcrCTG->Append(colref);

	// generate a bool datum
	IDatumArray *pdrspqdatum = SPQOS_NEW(mp) IDatumArray(mp);
	IDatumBool *datum =
		pmdtypebool->CreateBoolDatum(mp, false /*value*/, false /*is_null*/);
	pdrspqdatum->Append(datum);
	IDatum2dArray *pdrspqdrspqdatum = SPQOS_NEW(mp) IDatum2dArray(mp);
	pdrspqdrspqdatum->Append(pdrspqdatum);

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalConstTableGet(mp, pdrspqcrCTG, pdrspqdrspqdatum));
}

// map a column from source array to destination array based on position
CColRef *
CUtils::PcrMap(CColRef *pcrSource, CColRefArray *pdrspqcrSource,
			   CColRefArray *pdrspqcrTarget)
{
	const ULONG num_cols = pdrspqcrSource->Size();
	SPQOS_ASSERT(num_cols == pdrspqcrTarget->Size());

	CColRef *pcrTarget = NULL;
	for (ULONG ul = 0; NULL == pcrTarget && ul < num_cols; ul++)
	{
		if ((*pdrspqcrSource)[ul] == pcrSource)
		{
			pcrTarget = (*pdrspqcrTarget)[ul];
		}
	}

	return pcrTarget;
}

// check if the given operator is a motion and the derived relational
// properties contain a consumer which is not in the required part consumers
BOOL
CUtils::FMotionOverUnresolvedPartConsumers(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CPartIndexMap *ppimReqd)
{
	SPQOS_ASSERT(NULL != ppimReqd);

	if (!FPhysicalMotion(exprhdl.Pop()))
	{
		return false;
	}

	CPartIndexMap *ppimDrvd = exprhdl.Pdpplan(0 /*child_index*/)->Ppim();
	ULongPtrArray *pdrspqulScanIds =
		ppimDrvd->PdrspqulScanIds(mp, true /*fConsumersOnly*/);
	BOOL fHasUnresolvedConsumers = false;

	const ULONG ulConsumers = pdrspqulScanIds->Size();
	if (0 < ulConsumers && !ppimReqd->FContainsUnresolved())
	{
		fHasUnresolvedConsumers = true;
	}

	for (ULONG ul = 0; !fHasUnresolvedConsumers && ul < ulConsumers; ul++)
	{
		ULONG *pulScanId = (*pdrspqulScanIds)[ul];
		if (!ppimReqd->Contains(*pulScanId))
		{
			// there is an unresolved consumer which is not included in the
			// requirements and will therefore be resolved elsewhere
			fHasUnresolvedConsumers = true;
		}
	}

	pdrspqulScanIds->Release();

	return fHasUnresolvedConsumers;
}

BOOL
CUtils::FDuplicateHazardDistributionSpec(CDistributionSpec *pds)
{
	CDistributionSpec::EDistributionType edt = pds->Edt();

	return CDistributionSpec::EdtStrictReplicated == edt ||
		   CDistributionSpec::EdtUniversal == edt;
}

// Check if duplicate values can be generated when executing the given Motion expression,
// duplicates occur if Motion's input has strict-replicated/universal distribution,
// which means that we have exactly the same copy of input on each host. Note that
// tainted-replicated does not satisfy the assertion of identical input copies.
BOOL
CUtils::FDuplicateHazardMotion(CExpression *pexprMotion)
{
	SPQOS_ASSERT(NULL != pexprMotion);
	SPQOS_ASSERT(CUtils::FPhysicalMotion(pexprMotion->Pop()));

	CExpression *pexprChild = (*pexprMotion)[0];
	CDrvdPropPlan *pdpplanChild =
		CDrvdPropPlan::Pdpplan(pexprChild->PdpDerive());
	CDistributionSpec *pdsChild = pdpplanChild->Pds();

	return CUtils::FDuplicateHazardDistributionSpec(pdsChild);
}

// Collapse the top two project nodes like this, if unable return NULL;
//
// clang-format off
//	+--CLogicalProject                                            <-- pexpr
//		|--CLogicalProject                                        <-- pexprRel
//		|  |--CLogicalGet "t" ("t"), Columns: ["a" (0), "b" (1)]  <-- pexprChildRel
//		|  +--CScalarProjectList                                  <-- pexprChildScalar
//		|     +--CScalarProjectElement "c" (9)
//		|        +--CScalarFunc ()
//		|           |--CScalarIdent "a" (0)
//		|           +--CScalarConst ()
//		+--CScalarProjectList                                     <-- pexprScalar
//		   +--CScalarProjectElement "d" (10)                      <-- pexprPrE
//			  +--CScalarFunc ()
//				 |--CScalarIdent "b" (1)
//				 +--CScalarConst ()
// clang-format on
CExpression *
CUtils::PexprCollapseProjects(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (pexpr->Pop()->Eopid() != COperator::EopLogicalProject)
	{
		// not a project node
		return NULL;
	}

	CExpression *pexprRel = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	if (pexprRel->Pop()->Eopid() != COperator::EopLogicalProject)
	{
		// not a project node
		return NULL;
	}

	CExpression *pexprChildRel = (*pexprRel)[0];
	CExpression *pexprChildScalar = (*pexprRel)[1];

	CColRefSet *pcrsDefinedChild =
		SPQOS_NEW(mp) CColRefSet(mp, *pexprChildScalar->DeriveDefinedColumns());

	// array of project elements for the new child project node
	CExpressionArray *pdrspqexprPrElChild = SPQOS_NEW(mp) CExpressionArray(mp);

	// array of project elements that have set returning scalar functions that can be collapsed
	CExpressionArray *pdrspqexprSetReturnFunc =
		SPQOS_NEW(mp) CExpressionArray(mp);
	ULONG ulCollapsableSetReturnFunc = 0;

	BOOL fChildProjElHasSetReturn =
		pexprChildScalar->DeriveHasNonScalarFunction();

	// iterate over the parent project elements and see if we can add it to the child's project node
	CExpressionArray *pdrspqexprPrEl = SPQOS_NEW(mp) CExpressionArray(mp);
	ULONG ulLenPr = pexprScalar->Arity();
	for (ULONG ul1 = 0; ul1 < ulLenPr; ul1++)
	{
		CExpression *pexprPrE = (*pexprScalar)[ul1];

		CColRefSet *pcrsUsed =
			SPQOS_NEW(mp) CColRefSet(mp, *pexprPrE->DeriveUsedColumns());

		pexprPrE->AddRef();

		BOOL fHasSetReturn = pexprPrE->DeriveHasNonScalarFunction();

		pcrsUsed->Intersection(pcrsDefinedChild);
		ULONG ulIntersect = pcrsUsed->Size();

		if (fHasSetReturn)
		{
			pdrspqexprSetReturnFunc->Append(pexprPrE);

			if (0 == ulIntersect)
			{
				// there are no columns from the relational child that this project element uses
				ulCollapsableSetReturnFunc++;
			}
		}
		else if (0 == ulIntersect)
		{
			pdrspqexprPrElChild->Append(pexprPrE);
		}
		else
		{
			pdrspqexprPrEl->Append(pexprPrE);
		}

		pcrsUsed->Release();
	}

	const ULONG ulTotalSetRetFunc = pdrspqexprSetReturnFunc->Size();

	if (!fChildProjElHasSetReturn &&
		ulCollapsableSetReturnFunc == ulTotalSetRetFunc)
	{
		// there are set returning functions and
		// all of the set returning functions are collapsabile
		AppendArrayExpr(pdrspqexprSetReturnFunc, pdrspqexprPrElChild);
	}
	else
	{
		// We come here when either
		// 1. None of the parent's project element use a set retuning function
		// 2. Both parent's and relation child's project list has atleast one project element using set
		// returning functions. In this case we should not collapsed into one project to ensure for correctness.
		// 3. In the parent's project list there exists a project element with set returning functions that
		// cannot be collapsed. If the parent's project list has more than one project element with
		// set returning functions we should either collapse all of them or none of them for correctness.

		AppendArrayExpr(pdrspqexprSetReturnFunc, pdrspqexprPrEl);
	}

	// clean up
	pcrsDefinedChild->Release();
	pdrspqexprSetReturnFunc->Release();

	// add all project elements of the origin child project node
	ULONG ulLenChild = pexprChildScalar->Arity();
	for (ULONG ul = 0; ul < ulLenChild; ul++)
	{
		CExpression *pexprPrE = (*pexprChildScalar)[ul];
		pexprPrE->AddRef();
		pdrspqexprPrElChild->Append(pexprPrE);
	}

	if (ulLenPr == pdrspqexprPrEl->Size())
	{
		// no candidate project element found for collapsing
		pdrspqexprPrElChild->Release();
		pdrspqexprPrEl->Release();

		return NULL;
	}

	pexprChildRel->AddRef();
	CExpression *pexprProject = SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalProject(mp), pexprChildRel,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pdrspqexprPrElChild));

	if (0 == pdrspqexprPrEl->Size())
	{
		// no residual project elements
		pdrspqexprPrEl->Release();

		return pexprProject;
	}

	return SPQOS_NEW(mp) CExpression(
		mp, SPQOS_NEW(mp) CLogicalProject(mp), pexprProject,
		SPQOS_NEW(mp) CExpression(mp, SPQOS_NEW(mp) CScalarProjectList(mp),
								 pdrspqexprPrEl));
}

// append expressions in the source array to destination array
void
CUtils::AppendArrayExpr(CExpressionArray *pdrspqexprSrc,
						CExpressionArray *pdrspqexprDest)
{
	SPQOS_ASSERT(NULL != pdrspqexprSrc);
	SPQOS_ASSERT(NULL != pdrspqexprDest);

	ULONG length = pdrspqexprSrc->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CExpression *pexprPrE = (*pdrspqexprSrc)[ul];
		pexprPrE->AddRef();
		pdrspqexprDest->Append(pexprPrE);
	}
}

// compares two datums. Takes pointer pointer to a datums.
INT
CUtils::IDatumCmp(const void *val1, const void *val2)
{
	const IDatum *dat1 = *(IDatum **) (val1);
	const IDatum *dat2 = *(IDatum **) (val2);

	const IComparator *pcomp = COptCtxt::PoctxtFromTLS()->Pcomp();

	if (pcomp->Equals(dat1, dat2))
	{
		return 0;
	}
	else if (pcomp->IsLessThan(dat1, dat2))
	{
		return -1;
	}

	return 1;
}

// compares two points. Takes pointer pointer to a CPoint.
INT
CUtils::CPointCmp(const void *val1, const void *val2)
{
	const CPoint *p1 = *(CPoint **) (val1);
	const CPoint *p2 = *(CPoint **) (val2);

	if (p1->Equals(p2))
	{
		return 0;
	}
	else if (p1->IsLessThan(p2))
	{
		return -1;
	}

	SPQOS_ASSERT(p1->IsGreaterThan(p2));
	return 1;
}

// check if the equivalance classes are disjoint
BOOL
CUtils::FEquivalanceClassesDisjoint(CMemoryPool *mp,
									const CColRefSetArray *pdrspqcrs)
{
	const ULONG length = pdrspqcrs->Size();

	// nothing to check
	if (length == 0)
	{
		return true;
	}

	ColRefToColRefSetMap *phmcscrs = SPQOS_NEW(mp) ColRefToColRefSetMap(mp);

	// populate a hashmap in this loop
	for (ULONG ulFst = 0; ulFst < length; ulFst++)
	{
		CColRefSet *pcrsFst = (*pdrspqcrs)[ulFst];

		CColRefSetIter crsi(*pcrsFst);
		while (crsi.Advance())
		{
			CColRef *colref = crsi.Pcr();
			pcrsFst->AddRef();

			// if there is already an entry for the column referance it means the column is
			// part of another set which shows the equivalance classes are not disjoint
			if (!phmcscrs->Insert(colref, pcrsFst))
			{
				phmcscrs->Release();
				pcrsFst->Release();
				return false;
			}
		}
	}

	phmcscrs->Release();
	return true;
}

// check if the equivalance classes are same
BOOL
CUtils::FEquivalanceClassesEqual(CMemoryPool *mp, CColRefSetArray *pdrspqcrsFst,
								 CColRefSetArray *pdrspqcrsSnd)
{
	const ULONG ulLenFrst = pdrspqcrsFst->Size();
	const ULONG ulLenSecond = pdrspqcrsSnd->Size();

	if (ulLenFrst != ulLenSecond)
		return false;

	ColRefToColRefSetMap *phmcscrs = SPQOS_NEW(mp) ColRefToColRefSetMap(mp);
	for (ULONG ulFst = 0; ulFst < ulLenFrst; ulFst++)
	{
		CColRefSet *pcrsFst = (*pdrspqcrsFst)[ulFst];
		CColRefSetIter crsi(*pcrsFst);
		while (crsi.Advance())
		{
			CColRef *colref = crsi.Pcr();
			pcrsFst->AddRef();
			phmcscrs->Insert(colref, pcrsFst);
		}
	}

	for (ULONG ulSnd = 0; ulSnd < ulLenSecond; ulSnd++)
	{
		CColRefSet *pcrsSnd = (*pdrspqcrsSnd)[ulSnd];
		CColRef *colref = pcrsSnd->PcrAny();
		CColRefSet *pcrs = phmcscrs->Find(colref);
		if (!pcrsSnd->Equals(pcrs))
		{
			phmcscrs->Release();
			return false;
		}
	}
	phmcscrs->Release();
	return true;
}

// This function provides a check for a plan with CTE, if both
// CTEProducer and CTEConsumer are executed on the same locality.
// If it is not the case, the plan is bogus and cannot be executed
// by the executor, therefore it throws an exception causing fallback
// to planner.
//
// The overall algorithm for detecting CTE producer and consumer
// inconsistency employs a HashMap while preorder traversing the tree.
// Preorder traversal will guarantee that we visit the producer before
// we visit the consumer. In this regard, when we see a CTE producer,
// we add its CTE id as a key and its execution locality as a value to
// the HashMap.
// And when we encounter the matching CTE consumer while we traverse the
// tree, we check if the locality matches by looking up the CTE id from
// the HashMap. If we see a non-matching locality, we report the
// anamoly.
//
// We change the locality and push it down the tree whenever we detect
// a motion and the motion type enforces a locality change. We pass the
// locality type by value instead of referance to avoid locality changes
// affect parent and sibling localities.
void
CUtils::ValidateCTEProducerConsumerLocality(
	CMemoryPool *mp, CExpression *pexpr, EExecLocalityType eelt,
	UlongToUlongMap *
		phmulul	 // Hash Map containing the CTE Producer id and its execution locality
)
{
	COperator *pop = pexpr->Pop();
	if (COperator::EopPhysicalCTEProducer == pop->Eopid())
	{
		// record the location (either master or segment or singleton)
		// where the CTE producer is being executed
		ULONG ulCTEID = CPhysicalCTEProducer::PopConvert(pop)->UlCTEId();
		phmulul->Insert(SPQOS_NEW(mp) ULONG(ulCTEID), SPQOS_NEW(mp) ULONG(eelt));
	}
	else if (COperator::EopPhysicalCTEConsumer == pop->Eopid())
	{
		ULONG ulCTEID = CPhysicalCTEConsumer::PopConvert(pop)->UlCTEId();
		ULONG *pulLocProducer = phmulul->Find(&ulCTEID);

		// check if the CTEConsumer is being executed in the same location
		// as the CTE Producer
		if (NULL == pulLocProducer || *pulLocProducer != (ULONG) eelt)
		{
			phmulul->Release();
			SPQOS_RAISE(spqopt::ExmaSPQOPT,
					   spqopt::ExmiCTEProducerConsumerMisAligned, ulCTEID);
		}
	}
	// In case of a Gather motion, the execution locality is set to segments
	// since the child of Gather motion executes on segments
	else if (COperator::EopPhysicalMotionGather == pop->Eopid())
	{
		eelt = EeltSegments;
	}
	else if (COperator::EopPhysicalMotionHashDistribute == pop->Eopid() ||
			 COperator::EopPhysicalMotionRandom == pop->Eopid() ||
			 COperator::EopPhysicalMotionBroadcast == pop->Eopid())
	{
		// For any of these physical motions, the outer child's execution needs to be
		// tracked for depending upon the distribution spec
		CDrvdPropPlan *pdpplanChild =
			CDrvdPropPlan::Pdpplan((*pexpr)[0]->PdpDerive());
		CDistributionSpec *pdsChild = pdpplanChild->Pds();

		eelt = CUtils::ExecLocalityType(pdsChild);
	}

	const ULONG length = pexpr->Arity();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CExpression *pexprChild = (*pexpr)[ul];

		if (!pexprChild->Pop()->FScalar())
		{
			ValidateCTEProducerConsumerLocality(mp, pexprChild, eelt, phmulul);
		}
	}
}

// get execution locality type
CUtils::EExecLocalityType
CUtils::ExecLocalityType(CDistributionSpec *pds)
{
	EExecLocalityType eelt;
	if (CDistributionSpec::EdtSingleton == pds->Edt() ||
		CDistributionSpec::EdtStrictSingleton == pds->Edt())
	{
		CDistributionSpecSingleton *pdss =
			CDistributionSpecSingleton::PdssConvert(pds);
		if (pdss->FOnMaster())
		{
			eelt = EeltMaster;
		}
		else
		{
			eelt = EeltSingleton;
		}
	}
	else
	{
		eelt = EeltSegments;
	}
	return eelt;
}

// generate a limit expression on top of the given relational child with the given offset and limit count
CExpression *
CUtils::PexprLimit(CMemoryPool *mp, CExpression *pexpr, ULONG ulOffSet,
				   ULONG count)
{
	SPQOS_ASSERT(pexpr);

	COrderSpec *pos = SPQOS_NEW(mp) COrderSpec(mp);
	CLogicalLimit *popLimit = SPQOS_NEW(mp)
		CLogicalLimit(mp, pos, true /* fGlobal */, true /* fHasCount */,
					  false /*fTopLimitUnderDML*/);
	CExpression *pexprLimitOffset = CUtils::PexprScalarConstInt8(mp, ulOffSet);
	CExpression *pexprLimitCount = CUtils::PexprScalarConstInt8(mp, count);

	return SPQOS_NEW(mp)
		CExpression(mp, popLimit, pexpr, pexprLimitOffset, pexprLimitCount);
}

// check if a given operator is a ANY subquery
BOOL
CUtils::FAnySubquery(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	return COperator::EopScalarSubqueryAny == pop->Eopid();
}

// check if a given operator is an EXISTS subquery
BOOL
CUtils::FExistsSubquery(COperator *pop)
{
	SPQOS_ASSERT(NULL != pop);

	return COperator::EopScalarSubqueryExists == pop->Eopid();
}

// check if the expression is a correlated EXISTS/ANY subquery
BOOL
CUtils::FCorrelatedExistsAnySubquery(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	return (CUtils::FAnySubquery(pexpr->Pop()) ||
			CUtils::FExistsSubquery(pexpr->Pop())) &&
		   (*pexpr)[0]->HasOuterRefs();
}

CScalarProjectElement *
CUtils::PNthProjectElement(CExpression *pexpr, ULONG ul)
{
	SPQOS_ASSERT(pexpr->Pop()->Eopid() == COperator::EopLogicalProject);

	// Logical Project's first child is relational child and the second
	// child is the project list. We initially get the project list and then
	// the nth element in the project list
	return CScalarProjectElement::PopConvert((*(*pexpr)[1])[ul]->Pop());
}

// returns the expression under the Nth project element of a CLogicalProject
CExpression *
CUtils::PNthProjectElementExpr(CExpression *pexpr, ULONG ul)
{
	SPQOS_ASSERT(pexpr->Pop()->Eopid() == COperator::EopLogicalProject);

	// Logical Project's first child is relational child and the second
	// child is the project list. We initially get the project list and then
	// the nth element in the project list and finally expression under that
	// element.
	return (*(*(*pexpr)[1])[ul])[0];
}

// check if the Project list has an inner reference assuming project list has one projecet element
BOOL
CUtils::FInnerRefInProjectList(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(COperator::EopLogicalProject == pexpr->Pop()->Eopid());

	// extract output columns of the relational child
	CColRefSet *pcrsOuterOutput = (*pexpr)[0]->DeriveOutputColumns();

	// Project List with one project element
	CExpression *pexprInner = (*pexpr)[1];
	SPQOS_ASSERT(1 == pexprInner->Arity());
	BOOL fExprHasAnyCrFromCrs =
		CUtils::FExprHasAnyCrFromCrs(pexprInner, pcrsOuterOutput);

	return fExprHasAnyCrFromCrs;
}

// Check if expression tree has a col being referenced in the CColRefSet passed as input
BOOL
CUtils::FExprHasAnyCrFromCrs(CExpression *pexpr, CColRefSet *pcrs)
{
	SPQOS_ASSERT(NULL != pexpr);
	SPQOS_ASSERT(NULL != pcrs);
	CColRef *colref = NULL;

	COperator::EOperatorId op_id = pexpr->Pop()->Eopid();
	switch (op_id)
	{
		case COperator::EopScalarProjectElement:
		{
			// check project elements
			CScalarProjectElement *popScalarProjectElement =
				CScalarProjectElement::PopConvert(pexpr->Pop());
			colref = (CColRef *) popScalarProjectElement->Pcr();
			if (pcrs->FMember(colref))
				return true;
			break;
		}
		case COperator::EopScalarIdent:
		{
			// Check scalarIdents
			CScalarIdent *popScalarOp = CScalarIdent::PopConvert(pexpr->Pop());
			colref = (CColRef *) popScalarOp->Pcr();
			if (pcrs->FMember(colref))
				return true;
			break;
		}
		default:
			break;
	}

	// recursively process children
	const ULONG arity = pexpr->Arity();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (FExprHasAnyCrFromCrs((*pexpr)[ul], pcrs))
			return true;
	}

	return false;
}

// returns true if expression contains aggregate window function
BOOL
CUtils::FHasAggWindowFunc(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	if (COperator::EopScalarWindowFunc == pexpr->Pop()->Eopid())
	{
		CScalarWindowFunc *popScWindowFunc =
			CScalarWindowFunc::PopConvert(pexpr->Pop());
		return popScWindowFunc->FAgg();
	}

	// process children
	BOOL fHasAggWindowFunc = false;
	for (ULONG ul = 0; !fHasAggWindowFunc && ul < pexpr->Arity(); ul++)
	{
		fHasAggWindowFunc = FHasAggWindowFunc((*pexpr)[ul]);
	}

	return fHasAggWindowFunc;
}


// returns true if mdid is a supported ordered agg.
// Currently we only support inbuilt ordered aggs
// percentile_disc and percentile_cont for splitting
BOOL
CUtils::FIsInbuiltOrderedAgg(IMDId *mdid)
{
	SPQOS_ASSERT(mdid->IsValid());

	OID agg_oid = CMDIdSPQDB::CastMdid(mdid)->Oid();
	return (agg_oid == SPQDB_PERCENTILE_DISC || agg_oid == SPQDB_MEDIAN_FLOAT8 ||
			agg_oid == SPQDB_MEDIAN_INTERVAL ||
			agg_oid == SPQDB_MEDIAN_TIMESTAMP ||
			agg_oid == SPQDB_MEDIAN_TIMESTAMPTZ ||
			agg_oid == SPQDB_PERCENTILE_CONT_FLOAT8 ||
			agg_oid == SPQDB_PERCENTILE_CONT_INTERVAL ||
			agg_oid == SPQDB_PERCENTILE_CONT_TIMESTAMP ||
			agg_oid == SPQDB_PERCENTILE_CONT_TIMESTAMPTZ);
}


// returns true if expression contains ordered aggregate function
BOOL
CUtils::FHasOrderedAggToSplit(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	CScalarAggFunc *popScAggFunc = CScalarAggFunc::PopConvert(pexpr->Pop());
	return popScAggFunc->AggKind() == EaggfunckindOrderedSet &&
		   FIsInbuiltOrderedAgg(popScAggFunc->MDId()) &&
		   (NULL != popScAggFunc->GetGpAggMDId()) &&
		   (!FScalarConst((*(*pexpr)[1])[0]) ||
			!FIsConstArray((*(*pexpr)[1])[0])) &&
		   (FScalarIdent((*(*pexpr)[0])[0]) ||
			CScalarIdent::FCastedScId((*(*pexpr)[0])[0]));
}

BOOL
CUtils::FCrossJoin(CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	BOOL fCrossJoin = false;
	if (pexpr->Pop()->Eopid() == COperator::EopLogicalInnerJoin)
	{
		SPQOS_ASSERT(3 == pexpr->Arity());
		CExpression *pexprPred = (*pexpr)[2];
		if (CUtils::FScalarConstTrue(pexprPred))
			fCrossJoin = true;
	}
	return fCrossJoin;
}

BOOL
CUtils::IsHashJoinPossible(CMemoryPool *mp, CExpression *pexpr)
{
	SPQOS_ASSERT(NULL != pexpr);

	CExpressionArray *expr_preds =
		CCastUtils::PdrspqexprCastEquality(mp, (*pexpr)[2]);

	BOOL has_hashable_pred = false;
	ULONG ulPreds = expr_preds->Size();
	for (ULONG ul = 0; ul < ulPreds && !has_hashable_pred; ul++)
	{
		CExpression *pred = (*expr_preds)[ul];
		if (CPhysicalJoin::FHashJoinCompatible(pred, (*pexpr)[0], (*pexpr)[1]))
		{
			has_hashable_pred = true;
		}
	}
	expr_preds->Release();

	return has_hashable_pred;
}

// Determine whether a scalar expression consists only of a scalar id and NDV-preserving
// functions plus casts. If so, return the corresponding CColRef.
BOOL
CUtils::IsExprNDVPreserving(CExpression *pexpr,
							const CColRef **underlying_colref)
{
	CExpression *curr_expr = pexpr;

	*underlying_colref = NULL;

	// go down the expression tree, visiting the child containing a scalar ident until
	// we found the ident or until we found a non-NDV-preserving function (at which point there
	// is no more need to check)
	while (1)
	{
		COperator *pop = curr_expr->Pop();
		ULONG child_with_scalar_ident = 0;

		switch (pop->Eopid())
		{
			case COperator::EopScalarIdent:
			{
				// we reached the bottom of the expression, return the ColRef
				CScalarIdent *cr = CScalarIdent::PopConvert(pop);

				*underlying_colref = cr->Pcr();
				SPQOS_ASSERT(1 == pexpr->DeriveUsedColumns()->Size());
				return true;
			}

			case COperator::EopScalarCast:
				// skip over casts
				// Note: We might in the future investigate whether there are some casts
				// that reduce NDVs by too much. Most, if not all, casts that have that potential are
				// converted to functions, though. Examples: timestamp -> date, double precision -> int.
				break;

			case COperator::EopScalarCoalesce:
			{
				// coalesce(col, const1, ... constn) is treated as an NDV-preserving function
				for (ULONG c = 1; c < curr_expr->Arity(); c++)
				{
					if (0 < (*curr_expr)[c]->DeriveUsedColumns()->Size())
					{
						// this coalesce has a ColRef in the second or later arguments, assume for
						// now that this doesn't preserve NDVs (we could add logic to support this case later)
						return false;
					}
				}
				break;
			}
			case COperator::EopScalarFunc:
			{
				// check whether the function is NDV-preserving
				CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
				CScalarFunc *sf = CScalarFunc::PopConvert(pop);
				const IMDFunction *pmdfunc =
					md_accessor->RetrieveFunc(sf->FuncMdId());

				if (!pmdfunc->IsNDVPreserving() || 1 != curr_expr->Arity())
				{
					return false;
				}
				break;
			}

			case COperator::EopScalarOp:
			{
				CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
				CScalarOp *so = CScalarOp::PopConvert(pop);
				const IMDScalarOp *pmdscop =
					md_accessor->RetrieveScOp(so->MdIdOp());

				if (!pmdscop->IsNDVPreserving() || 2 != curr_expr->Arity())
				{
					return false;
				}

				// col <op> const is NDV-preserving, and so is const <op> col
				if (0 == (*curr_expr)[1]->DeriveUsedColumns()->Size())
				{
					// col <op> const
					child_with_scalar_ident = 0;
				}
				else if (0 == (*curr_expr)[0]->DeriveUsedColumns()->Size())
				{
					// const <op> col
					child_with_scalar_ident = 1;
				}
				else
				{
					// give up for now, both children reference a column,
					// e.g. col1 <op> col2
					return false;
				}
				break;
			}

			default:
				// anything else we see is considered non-NDV-preserving
				return false;
		}

		curr_expr = (*curr_expr)[child_with_scalar_ident];
	}
}


// search the given array of predicates for predicates with equality or IS NOT
// DISTINCT FROM operators that has one side equal to the given expression, if
// found, return the other side of equality, otherwise return NULL
CExpression *
CUtils::PexprMatchEqualityOrINDF(
	CExpression *pexprToMatch,
	CExpressionArray *pdrspqexpr	 // array of predicates to inspect
)
{
	SPQOS_ASSERT(NULL != pexprToMatch);
	SPQOS_ASSERT(NULL != pdrspqexpr);

	CExpression *pexprMatching = NULL;
	const ULONG ulSize = pdrspqexpr->Size();
	for (ULONG ul = 0; ul < ulSize; ul++)
	{
		CExpression *pexprPred = (*pdrspqexpr)[ul];
		CExpression *pexprPredOuter, *pexprPredInner;


		if (CPredicateUtils::IsEqualityOp(pexprPred))
		{
			pexprPredOuter = (*pexprPred)[0];
			pexprPredInner = (*pexprPred)[1];
		}
		else if (CPredicateUtils::FINDF(pexprPred))
		{
			pexprPredOuter = (*(*pexprPred)[0])[0];
			pexprPredInner = (*(*pexprPred)[0])[1];
		}
		else
		{
			continue;
		}

		IMDId *pmdidTypeOuter =
			CScalar::PopConvert(pexprPredOuter->Pop())->MdidType();
		IMDId *pmdidTypeInner =
			CScalar::PopConvert(pexprPredInner->Pop())->MdidType();
		if (!pmdidTypeOuter->Equals(pmdidTypeInner))
		{
			// only consider equality of identical types
			continue;
		}

		pexprToMatch =
			CCastUtils::PexprWithoutBinaryCoercibleCasts(pexprToMatch);
		if (CUtils::Equals(
				CCastUtils::PexprWithoutBinaryCoercibleCasts(pexprPredOuter),
				pexprToMatch))
		{
			pexprMatching = pexprPredInner;
			break;
		}

		if (CUtils::Equals(
				CCastUtils::PexprWithoutBinaryCoercibleCasts(pexprPredInner),
				pexprToMatch))
		{
			pexprMatching = pexprPredOuter;
			break;
		}
	}

	if (NULL != pexprMatching)
		return CCastUtils::PexprWithoutBinaryCoercibleCasts(pexprMatching);
	return pexprMatching;
}

// from the input join expression, remove the inferred predicates
// and return the new join expression without inferred predicate
CExpression *
CUtils::MakeJoinWithoutInferredPreds(CMemoryPool *mp, CExpression *join_expr)
{
	SPQOS_ASSERT(COperator::EopLogicalInnerJoin == join_expr->Pop()->Eopid());

	CExpressionHandle expression_handle(mp);
	expression_handle.Attach(join_expr);
	CExpression *scalar_expr = expression_handle.PexprScalarExactChild(
		join_expr->Arity() - 1, true /*error_on_null_return*/);
	CExpression *scalar_expr_without_inferred_pred =
		CPredicateUtils::PexprRemoveImpliedConjuncts(mp, scalar_expr,
													 expression_handle);

	// create a new join expression using the scalar expr without inferred predicate
	CExpression *left_child_expr = (*join_expr)[0];
	CExpression *right_child_expr = (*join_expr)[1];
	left_child_expr->AddRef();
	right_child_expr->AddRef();
	COperator *join_op = join_expr->Pop();
	join_op->AddRef();
	return SPQOS_NEW(mp)
		CExpression(mp, join_op, left_child_expr, right_child_expr,
					scalar_expr_without_inferred_pred);
}

// check if the input expr array contains the expr
BOOL
CUtils::Contains(const CExpressionArray *exprs, CExpression *expr_to_match)
{
	if (NULL == exprs)
	{
		return false;
	}

	BOOL contains = false;
	for (ULONG ul = 0; ul < exprs->Size() && !contains; ul++)
	{
		CExpression *expr = (*exprs)[ul];
		contains = CUtils::Equals(expr, expr_to_match);
	}
	return contains;
}

BOOL
CUtils::Equals(const CExpressionArrays *exprs_arr,
			   const CExpressionArrays *other_exprs_arr)
{
	SPQOS_CHECK_STACK_SIZE;

	// NULL arrays are equal
	if (NULL == exprs_arr || NULL == other_exprs_arr)
	{
		return NULL == exprs_arr && NULL == other_exprs_arr;
	}

	// do pointer comparision
	if (exprs_arr == other_exprs_arr)
	{
		return true;
	}

	// if the size is not equal, the two arrays are not equal
	if (exprs_arr->Size() != other_exprs_arr->Size())
	{
		return false;
	}

	// if all the elements are equal, then both the arrays are equal
	BOOL equal = true;
	for (ULONG id = 0; id < exprs_arr->Size() && equal; id++)
	{
		equal = CUtils::Equals((*exprs_arr)[id], (*other_exprs_arr)[id]);
	}
	return equal;
}

BOOL
CUtils::Equals(const IMdIdArray *mdids, const IMdIdArray *other_mdids)
{
	SPQOS_CHECK_STACK_SIZE;

	// NULL arrays are equal
	if (NULL == mdids || NULL == other_mdids)
	{
		return NULL == mdids && NULL == other_mdids;
	}

	// do pointer comparision
	if (mdids == other_mdids)
	{
		return true;
	}

	// if the size is not equal, the two arrays are not equal
	if (mdids->Size() != other_mdids->Size())
	{
		return false;
	}

	// if all the elements are equal, then both the arrays are equal
	for (ULONG id = 0; id < mdids->Size(); id++)
	{
		if (!CUtils::Equals((*mdids)[id], (*other_mdids)[id]))
		{
			return false;
		}
	}
	return true;
}

BOOL
CUtils::Equals(const IMDId *mdid, const IMDId *other_mdid)
{
	if ((mdid == NULL) ^ (other_mdid == NULL))
	{
		return false;
	}

	return (mdid == other_mdid) || mdid->Equals(other_mdid);
}

// operators from which the inferred predicates can be removed
// NB: currently, only inner join is included, but we can add more later.
BOOL
CUtils::CanRemoveInferredPredicates(COperator::EOperatorId op_id)
{
	return op_id == COperator::EopLogicalInnerJoin;
}

CExpressionArrays *
CUtils::GetCombinedExpressionArrays(CMemoryPool *mp,
									CExpressionArrays *exprs_array,
									CExpressionArrays *exprs_array_other)
{
	CExpressionArrays *result_exprs = SPQOS_NEW(mp) CExpressionArrays(mp);
	AddExprs(result_exprs, exprs_array);
	AddExprs(result_exprs, exprs_array_other);

	return result_exprs;
}

void
CUtils::AddExprs(CExpressionArrays *results_exprs,
				 CExpressionArrays *input_exprs)
{
	SPQOS_ASSERT(NULL != results_exprs);
	SPQOS_ASSERT(NULL != input_exprs);
	for (ULONG ul = 0; ul < input_exprs->Size(); ul++)
	{
		CExpressionArray *exprs = (*input_exprs)[ul];
		exprs->AddRef();
		results_exprs->Append(exprs);
	}
	SPQOS_ASSERT(results_exprs->Size() >= input_exprs->Size());
}
// EOF
