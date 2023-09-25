//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CRandom.h
//
//	@doc:
//		Random number generator.
//
//	@owner:
//		Siva
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQOS_CRandom_H
#define SPQOS_CRandom_H

#include "spqos/types.h"

namespace spqos
{
class CRandom
{
private:
	// no copy c'tor
	CRandom(const CRandom &);

	// seed
	ULONG m_seed;

public:
	// no seed
	CRandom();

	// c'tor with seed
	CRandom(ULONG seed);

	// next random number
	ULONG Next();

	// d'tor
	~CRandom();
};	//class CRandom
}  // namespace spqos
#endif /* CRANDOM_H_ */

// EOF
