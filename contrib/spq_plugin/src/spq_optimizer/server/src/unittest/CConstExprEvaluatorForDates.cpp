//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CConstExprEvaluatorForDates.cpp
//
//	@doc:
//		Implementation of a constant expression evaluator for dates data
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "unittest/spqopt/CConstExprEvaluatorForDates.h"

#include "spqopt/base/CUtils.h"
#include "spqopt/operators/ops.h"
#include "naucrates/base/IDatum.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"

using namespace spqnaucrates;
using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CConstExprEvaluatorForDates::PexprEval
//
//	@doc:
//		It expects that the given expression is a scalar comparison between
//		two date constants. It compares the two constants using their lint
//		stats mapping, which in the case of the date type gives a correct result.
//		If it gets an illegal expression, an assertion failure is raised in
//		debug mode.
//
//---------------------------------------------------------------------------
CExpression *
CConstExprEvaluatorForDates::PexprEval(CExpression *pexpr)
{
	SPQOS_ASSERT(COperator::EopScalarCmp == pexpr->Pop()->Eopid());
	SPQOS_ASSERT(COperator::EopScalarConst == (*pexpr)[0]->Pop()->Eopid());
	SPQOS_ASSERT(COperator::EopScalarConst == (*pexpr)[1]->Pop()->Eopid());

	CScalarConst *popScalarLeft =
		dynamic_cast<CScalarConst *>((*pexpr)[0]->Pop());

	SPQOS_ASSERT(
		CMDIdSPQDB::m_mdid_date.Equals(popScalarLeft->GetDatum()->MDId()));
	CScalarConst *popScalarRight =
		dynamic_cast<CScalarConst *>((*pexpr)[1]->Pop());

	SPQOS_ASSERT(
		CMDIdSPQDB::m_mdid_date.Equals(popScalarRight->GetDatum()->MDId()));

	CScalarCmp *popScCmp = dynamic_cast<CScalarCmp *>(pexpr->Pop());
	LINT dLeft = popScalarLeft->GetDatum()->GetLINTMapping();
	LINT dRight = popScalarRight->GetDatum()->GetLINTMapping();
	BOOL result = false;
	switch (popScCmp->ParseCmpType())
	{
		case IMDType::EcmptEq:
			result = dLeft == dRight;
			break;
		case IMDType::EcmptNEq:
			result = dLeft != dRight;
			break;
		case IMDType::EcmptL:
			result = dLeft < dRight;
			break;
		case IMDType::EcmptLEq:
			result = dLeft <= dRight;
			break;
		case IMDType::EcmptG:
			result = dLeft > dRight;
			break;
		case IMDType::EcmptGEq:
			result = dLeft >= dRight;
			break;
		default:
			SPQOS_ASSERT(false && "Unsupported comparison");
			return NULL;
	}
	CExpression *pexprResult = CUtils::PexprScalarConstBool(m_mp, result);

	return pexprResult;
}

// EOF
