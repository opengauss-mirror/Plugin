//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CXformImplementation.h
//
//	@doc:
//		Base class for implementation transforms
//---------------------------------------------------------------------------
#ifndef SPQOPT_CXformImplementation_H
#define SPQOPT_CXformImplementation_H

#include "spqos/base.h"

#include "spqopt/xforms/CXform.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementation
//
//	@doc:
//		base class for all implementations
//
//---------------------------------------------------------------------------
class CXformImplementation : public CXform
{
private:
	// private copy ctor
	CXformImplementation(const CXformImplementation &);

public:
	// ctor
	explicit CXformImplementation(CExpression *);

	// dtor
	virtual ~CXformImplementation();

	// type of operator
	virtual BOOL
	FImplementation() const
	{
		SPQOS_ASSERT(!FSubstitution() && !FExploration());
		return true;
	}

};	// class CXformImplementation

}  // namespace spqopt


#endif	// !SPQOPT_CXformImplementation_H

// EOF
