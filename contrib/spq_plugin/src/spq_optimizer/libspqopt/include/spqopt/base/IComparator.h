//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		IComparator.h
//
//	@doc:
//		Interface for comparing IDatum instances.
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef SPQOPT_IComparator_H
#define SPQOPT_IComparator_H

#include "spqos/base.h"

namespace spqnaucrates
{
// fwd declarations
class IDatum;
}  // namespace spqnaucrates

namespace spqopt
{
using spqnaucrates::IDatum;

//---------------------------------------------------------------------------
//	@class:
//		IComparator
//
//	@doc:
//		Interface for comparing IDatum instances.
//
//---------------------------------------------------------------------------
class IComparator
{
public:
	virtual ~IComparator()
	{
	}

	// tests if the two arguments are equal
	virtual spqos::BOOL Equals(const IDatum *datum1,
							  const IDatum *datum2) const = 0;

	// tests if the first argument is less than the second
	virtual spqos::BOOL IsLessThan(const IDatum *datum1,
								  const IDatum *datum2) const = 0;

	// tests if the first argument is less or equal to the second
	virtual spqos::BOOL IsLessThanOrEqual(const IDatum *datum1,
										 const IDatum *datum2) const = 0;

	// tests if the first argument is greater than the second
	virtual spqos::BOOL IsGreaterThan(const IDatum *datum1,
									 const IDatum *datum2) const = 0;

	// tests if the first argument is greater or equal to the second
	virtual spqos::BOOL IsGreaterThanOrEqual(const IDatum *datum1,
											const IDatum *datum2) const = 0;
};
}  // namespace spqopt

#endif	// !SPQOPT_IComparator_H

// EOF
