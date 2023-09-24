//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CScalarArrayRefIndexList.cpp
//
//	@doc:
//		Implementation of scalar arrayref index list
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarArrayRefIndexList.h"

#include "spqos/base.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayRefIndexList::CScalarArrayRefIndexList
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarArrayRefIndexList::CScalarArrayRefIndexList(CMemoryPool *mp,
												   EIndexListType eilt)
	: CScalar(mp), m_eilt(eilt)
{
	SPQOS_ASSERT(EiltSentinel > eilt);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarArrayRefIndexList::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarArrayRefIndexList::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CScalarArrayRefIndexList *popIndexList =
		CScalarArrayRefIndexList::PopConvert(pop);

	return m_eilt == popIndexList->Eilt();
}

// EOF
