//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformResult.cpp
//
//	@doc:
//		Implementation of result container
//---------------------------------------------------------------------------

#include "spqopt/xforms/CXformResult.h"

#include "spqos/base.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CXformResult);

//---------------------------------------------------------------------------
//	@function:
//		CXformResult::CXformResult
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformResult::CXformResult(CMemoryPool *mp) : m_ulExpr(0)
{
	SPQOS_ASSERT(NULL != mp);
	m_pdrspqexpr = SPQOS_NEW(mp) CExpressionArray(mp);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformResult::~CXformResult
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CXformResult::~CXformResult()
{
	// release array (releases all elements)
	m_pdrspqexpr->Release();
}


//---------------------------------------------------------------------------
//	@function:
//		CXformResult::Add
//
//	@doc:
//		add alternative
//
//---------------------------------------------------------------------------
void
CXformResult::Add(CExpression *pexpr)
{
	SPQOS_ASSERT(0 == m_ulExpr &&
				"Incorrect workflow: cannot add further alternatives");

	SPQOS_ASSERT(NULL != pexpr);
	m_pdrspqexpr->Append(pexpr);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformResult::PexprNext
//
//	@doc:
//		retrieve next alternative
//
//---------------------------------------------------------------------------
CExpression *
CXformResult::PexprNext()
{
	CExpression *pexpr = NULL;
	if (m_ulExpr < m_pdrspqexpr->Size())
	{
		pexpr = (*m_pdrspqexpr)[m_ulExpr];
	}

	SPQOS_ASSERT(m_ulExpr <= m_pdrspqexpr->Size());
	m_ulExpr++;

	return pexpr;
}


//---------------------------------------------------------------------------
//	@function:
//		CXformResult::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CXformResult::OsPrint(IOstream &os) const
{
	os << "Alternatives:" << std::endl;

	for (ULONG i = 0; i < m_pdrspqexpr->Size(); i++)
	{
		os << i << ": " << std::endl;
		(*m_pdrspqexpr)[i]->OsPrint(os);
	}

	return os;
}

// EOF
