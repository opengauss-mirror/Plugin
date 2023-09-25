//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IBucket.h
//
//	@doc:
//		Simple bucket interface
//---------------------------------------------------------------------------

#ifndef SPQNAUCRATES_IBucket_H
#define SPQNAUCRATES_IBucket_H

#include "spqos/base.h"

#include "naucrates/statistics/CPoint.h"

namespace spqnaucrates
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		IBucket
//
//	@doc:
//		Simple bucket interface. Has a lower point and upper point
//
//---------------------------------------------------------------------------

class IBucket
{
private:
	// private copy constructor
	IBucket(const IBucket &);

	// private assignment operator
	IBucket &operator=(const IBucket &);

public:
	// c'tor
	IBucket()
	{
	}

	// lower point
	virtual CPoint *GetLowerBound() const = 0;

	// upper point
	virtual CPoint *GetUpperBound() const = 0;

	// is bucket singleton?
	BOOL
	IsSingleton() const
	{
		return GetLowerBound()->Equals(GetUpperBound());
	}

	// d'tor
	virtual ~IBucket()
	{
	}
};
}  // namespace spqnaucrates

#endif	// !SPQNAUCRATES_IBucket_H

// EOF
