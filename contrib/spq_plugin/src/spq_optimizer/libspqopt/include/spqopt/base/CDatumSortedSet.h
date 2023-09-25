//	Greenplum Database
//	Copyright (C) 2016 Pivotal Software, Inc.

#ifndef SPQOPT_CDatumSortedSet_H
#define SPQOPT_CDatumSortedSet_H

#include "spqos/memory/CMemoryPool.h"

#include "spqopt/base/IComparator.h"
#include "spqopt/operators/CExpression.h"
#include "naucrates/base/IDatum.h"

namespace spqopt
{
// A sorted and uniq'd array of pointers to datums
// It facilitates the construction of CConstraintInterval
class CDatumSortedSet : public IDatumArray
{
private:
	BOOL m_fIncludesNull;

public:
	CDatumSortedSet(CMemoryPool *mp, CExpression *pexprArray,
					const IComparator *pcomp);

	BOOL FIncludesNull() const;
};
}  // namespace spqopt

#endif
