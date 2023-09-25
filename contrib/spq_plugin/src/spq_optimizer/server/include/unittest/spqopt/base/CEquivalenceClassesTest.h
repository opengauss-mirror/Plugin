//---------------------------------------------------------------------------
//	Pivotal Software, Inc
//	Copyright (C) 2017 Pivotal Software, Inc
//---------------------------------------------------------------------------

#ifndef SPQOPT_CEquivalenceClassesTest_H
#define SPQOPT_CEquivalenceClassesTest_H

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"

namespace spqopt
{
using namespace spqos;

// Static unit tests for equivalence classes
class CEquivalenceClassesTest
{
public:
	// unittests
	static SPQOS_RESULT EresUnittest();
	static SPQOS_RESULT EresUnittest_NotDisjointEquivalanceClasses();
	static SPQOS_RESULT EresUnittest_IntersectEquivalanceClasses();
	static CColRefSetArray *createEquivalenceClasses(CMemoryPool *mp,
													 CColRefSet *pcrs,
													 int breakpoints[]);

};	// class CEquivalenceClassesTest
}  // namespace spqopt

#endif	// !SPQOPT_CEquivalenceClassesTest_H


// EOF
