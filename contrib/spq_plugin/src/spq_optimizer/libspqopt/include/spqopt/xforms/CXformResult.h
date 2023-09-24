//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformResult.h
//
//	@doc:
//		Result container for all transformations
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformResult_H
#define SPQOPT_CXformResult_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformResult
//
//	@doc:
//		result container
//
//---------------------------------------------------------------------------
class CXformResult : public CRefCount, public DbgPrintMixin<CXformResult>
{
private:
	// set of alternatives
	CExpressionArray *m_pdrspqexpr;

	// cursor for retrieval
	ULONG m_ulExpr;

	// private copy ctor
	CXformResult(const CXformResult &);

public:
	// ctor
	explicit CXformResult(CMemoryPool *);

	// dtor
	~CXformResult();

	// accessor
	inline CExpressionArray *
	Pdrspqexpr() const
	{
		return m_pdrspqexpr;
	}

	// add alternative
	void Add(CExpression *pexpr);

	// retrieve next alternative
	CExpression *PexprNext();

	// print function
	virtual IOstream &OsPrint(IOstream &os) const;

	ULONG
	Size() const
	{
		return m_pdrspqexpr->Size();
	}

};	// class CXformResult

// shorthand for printing
inline IOstream &
operator<<(IOstream &os, CXformResult &xfres)
{
	return xfres.OsPrint(os);
}

}  // namespace spqopt


#endif	// !SPQOPT_CXformResult_H

// EOF
