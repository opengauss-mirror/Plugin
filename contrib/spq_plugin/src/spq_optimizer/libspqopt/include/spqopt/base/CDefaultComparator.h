//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CDefaultComparator.h
//
//	@doc:
//		Default comparator for IDatum instances
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_CDefaultComparator_H
#define SPQOPT_CDefaultComparator_H

#include "spqos/base.h"

#include "spqopt/base/IComparator.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/traceflags/traceflags.h"

namespace spqmd
{
// fwd declarations
class IMDId;
}  // namespace spqmd

namespace spqnaucrates
{
// fwd declarations
class IDatum;
}  // namespace spqnaucrates

namespace spqopt
{
using namespace spqmd;
using namespace spqnaucrates;
using namespace spqos;

// fwd declarations
class IConstExprEvaluator;

//---------------------------------------------------------------------------
//	@class:
//		CDefaultComparator
//
//	@doc:
//		Default comparator for IDatum instances. It is a singleton accessed
//		via CompGetInstance.
//
//---------------------------------------------------------------------------
class CDefaultComparator : public IComparator
{
private:
	// constant expression evaluator
	IConstExprEvaluator *m_pceeval;

	// disabled copy constructor
	CDefaultComparator(const CDefaultComparator &);

	// construct a comparison expression from the given components and evaluate it
	BOOL FEvalComparison(CMemoryPool *mp, const IDatum *datum1,
						 const IDatum *datum2,
						 IMDType::ECmpType cmp_type) const;

	// return true iff we use built-in evaluation for integers
	static BOOL
	FUseBuiltinIntEvaluators()
	{
		return !SPQOS_FTRACE(EopttraceEnableConstantExpressionEvaluation) ||
			   !SPQOS_FTRACE(
				   EopttraceUseExternalConstantExpressionEvaluationForInts);
	}

public:
	// ctor
	CDefaultComparator(IConstExprEvaluator *pceeval);

	// dtor
	virtual ~CDefaultComparator()
	{
	}

	// tests if the two arguments are equal
	virtual BOOL Equals(const IDatum *datum1, const IDatum *datum2) const;

	// tests if the first argument is less than the second
	virtual BOOL IsLessThan(const IDatum *datum1, const IDatum *datum2) const;

	// tests if the first argument is less or equal to the second
	virtual BOOL IsLessThanOrEqual(const IDatum *datum1,
								   const IDatum *datum2) const;

	// tests if the first argument is greater than the second
	virtual BOOL IsGreaterThan(const IDatum *datum1,
							   const IDatum *datum2) const;

	// tests if the first argument is greater or equal to the second
	virtual BOOL IsGreaterThanOrEqual(const IDatum *datum1,
									  const IDatum *datum2) const;

};	// CDefaultComparator
}  // namespace spqopt

#endif	// !CDefaultComparator_H

// EOF
