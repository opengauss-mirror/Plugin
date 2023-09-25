//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformExploration.h
//
//	@doc:
//		Base class for exploration transforms
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformExploration_H
#define SPQOPT_CXformExploration_H

#include "spqos/base.h"

#include "spqopt/xforms/CXform.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformExploration
//
//	@doc:
//		Base class for all explorations
//
//---------------------------------------------------------------------------
class CXformExploration : public CXform
{
private:
	// private copy ctor
	CXformExploration(const CXformExploration &);

public:
	// ctor
	explicit CXformExploration(CExpression *pexpr);

	// dtor
	virtual ~CXformExploration();

	// type of operator
	virtual BOOL
	FExploration() const
	{
		SPQOS_ASSERT(!FSubstitution() && !FImplementation());
		return true;
	}

	// is transformation a subquery unnesting (Subquery To Apply) xform?
	virtual BOOL
	FSubqueryUnnesting() const
	{
		return false;
	}

	// is transformation an Apply decorrelation (Apply To Join) xform?
	virtual BOOL
	FApplyDecorrelating() const
	{
		return false;
	}

	// do stats need to be computed before applying xform?
	virtual BOOL
	FNeedsStats() const
	{
		return false;
	}

	// conversion function
	static CXformExploration *
	Pxformexp(CXform *pxform)
	{
		SPQOS_ASSERT(NULL != pxform);
		SPQOS_ASSERT(pxform->FExploration());

		return dynamic_cast<CXformExploration *>(pxform);
	}

};	// class CXformExploration

}  // namespace spqopt


#endif	// !SPQOPT_CXformExploration_H

// EOF
