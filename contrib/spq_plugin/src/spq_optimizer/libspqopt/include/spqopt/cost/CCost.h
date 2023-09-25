//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CCost.h
//
//	@doc:
//		Cost type
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_CCost_H
#define SPQNAUCRATES_CCost_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"
#include "spqos/common/CDynamicPtrArray.h"

namespace spqopt
{
using namespace spqos;

class CCost;
typedef CDynamicPtrArray<CCost, CleanupDelete> CCostArray;

//---------------------------------------------------------------------------
//	@class:
//		CCost
//
//	@doc:
//		Cost for comparing two plans
//
//---------------------------------------------------------------------------
class CCost : public CDouble
{
public:
	// ctor
	explicit CCost(DOUBLE d) : CDouble(d)
	{
	}

	// ctor
	explicit CCost(CDouble d) : CDouble(d.Get())
	{
	}

	// ctor
	CCost(const CCost &cost) : CDouble(cost.Get())
	{
	}

	// assignment
	CCost &
	operator=(const CCost &cost)
	{
		*(CDouble *) (this) = (CDouble) cost;
		return (*this);
	}

	// addition operator
	CCost
	operator+(const CCost &cost) const
	{
		CDouble d = (CDouble)(*this) + (CDouble) cost;
		return CCost(d);
	}

	// multiplication operator
	CCost
	operator*(const CCost &cost) const
	{
		return CCost((CDouble)(*this) * (CDouble) cost);
	}

	// comparison operator
	BOOL
	operator<(const CCost &cost) const
	{
		return (CDouble)(*this) < (CDouble) cost;
	}

	// comparison operator
	BOOL
	operator>(const CCost &cost) const
	{
		return (CDouble) cost < (CDouble)(*this);
	}

	// d'tor
	virtual ~CCost()
	{
	}

};	// class CCost

}  // namespace spqopt

#endif	// !SPQNAUCRATES_CCost_H

// EOF
