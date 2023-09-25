//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDecorrelator.h
//
//	@doc:
//		Decorrelation processor
//---------------------------------------------------------------------------
#ifndef SPQOPT_CDecorrelator_H
#define SPQOPT_CDecorrelator_H

#include "spqos/base.h"

#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CDecorrelator
//
//	@doc:
//		Helper class for extracting correlated expressions
//
//---------------------------------------------------------------------------
class CDecorrelator
{
private:
	// definition of operator processor
	typedef BOOL(FnProcessor)(CMemoryPool *, CExpression *, BOOL,
							  CExpression **, CExpressionArray *, CColRefSet *);

	//---------------------------------------------------------------------------
	//	@struct:
	//		SOperatorProcessor
	//
	//	@doc:
	//		Mapping of operator to a processor function
	//
	//---------------------------------------------------------------------------
	struct SOperatorProcessor
	{
		// scalar operator id
		COperator::EOperatorId m_eopid;

		// pointer to handler function
		FnProcessor *m_pfnp;

	};	// struct SOperatorHandler

	// private ctor
	CDecorrelator();

	// private dtor
	virtual ~CDecorrelator();

	// private copy ctor
	CDecorrelator(const CDecorrelator &);

	// helper to check if correlations below join are valid to be pulled-up
	static BOOL FPullableCorrelations(CMemoryPool *mp, CExpression *pexpr,
									  CExpressionArray *pdrspqexpr,
									  CExpressionArray *pdrspqexprCorrelations);

	// check if scalar operator can be delayed
	static BOOL FDelayableScalarOp(CExpression *pexprScalar);

	// check if scalar expression can be lifted
	static BOOL FDelayable(CExpression *pexprLogical, CExpression *pexprScalar,
						   BOOL fEqualityOnly);

	// switch function for all operators
	static BOOL FProcessOperator(CMemoryPool *mp, CExpression *pexpr,
								 BOOL fEqualityOnly,
								 CExpression **ppexprDecorrelated,
								 CExpressionArray *pdrspqexprCorrelations,
								 CColRefSet *outerRefsToRemove);

	// processor for predicates
	static BOOL FProcessPredicate(CMemoryPool *mp, CExpression *pexprLogical,
								  CExpression *pexprScalar, BOOL fEqualityOnly,
								  CExpression **ppexprDecorrelated,
								  CExpressionArray *pdrspqexprCorrelations,
								  CColRefSet *outerRefsToRemove);

	// processor for select operators
	static BOOL FProcessSelect(CMemoryPool *mp, CExpression *pexpr,
							   BOOL fEqualityOnly,
							   CExpression **ppexprDecorrelated,
							   CExpressionArray *pdrspqexprCorrelations,
							   CColRefSet *outerRefsToRemove);


	// processor for aggregates
	static BOOL FProcessGbAgg(CMemoryPool *mp, CExpression *pexpr,
							  BOOL fEqualityOnly,
							  CExpression **ppexprDecorrelated,
							  CExpressionArray *pdrspqexprCorrelations,
							  CColRefSet *outerRefsToRemove);

	// processor for joins (inner/n-ary)
	static BOOL FProcessJoin(CMemoryPool *mp, CExpression *pexpr,
							 BOOL fEqualityOnly,
							 CExpression **ppexprDecorrelated,
							 CExpressionArray *pdrspqexprCorrelations,
							 CColRefSet *outerRefsToRemove);


	// processor for projects
	static BOOL FProcessProject(CMemoryPool *mp, CExpression *pexpr,
								BOOL fEqualityOnly,
								CExpression **ppexprDecorrelated,
								CExpressionArray *pdrspqexprCorrelations,
								CColRefSet *outerRefsToRemove);

	// processor for assert
	static BOOL FProcessAssert(CMemoryPool *mp, CExpression *pexpr,
							   BOOL fEqualityOnly,
							   CExpression **ppexprDecorrelated,
							   CExpressionArray *pdrspqexprCorrelations,
							   CColRefSet *outerRefsToRemove);

	// processor for MaxOneRow
	static BOOL FProcessMaxOneRow(CMemoryPool *mp, CExpression *pexpr,
								  BOOL fEqualityOnly,
								  CExpression **ppexprDecorrelated,
								  CExpressionArray *pdrspqexprCorrelations,
								  CColRefSet *outerRefsToRemove);

	// processor for limits
	static BOOL FProcessLimit(CMemoryPool *mp, CExpression *pexpr,
							  BOOL fEqualityOnly,
							  CExpression **ppexprDecorrelated,
							  CExpressionArray *pdrspqexprCorrelations,
							  CColRefSet *outerRefsToRemove);

public:
	// main handler
	static BOOL FProcess(CMemoryPool *mp, CExpression *pexprOrig,
						 BOOL fEqualityOnly, CExpression **ppexprDecorrelated,
						 CExpressionArray *pdrspqexprCorrelations,
						 CColRefSet *outerRefsToRemove);

};	// class CDecorrelator

}  // namespace spqopt

#endif	// !SPQOPT_CDecorrelator_H

// EOF
