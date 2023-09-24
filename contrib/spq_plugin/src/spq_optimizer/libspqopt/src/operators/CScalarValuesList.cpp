//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2022 VMware, Inc. or its affiliates.
//
//	@filename:
//		CScalarValuesList.cpp
//
//	@doc:
//		Implementation of scalar arrayref index list
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarValuesList.h"

#include "spqos/base.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarValuesList::CScalarValuesList
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarValuesList::CScalarValuesList(CMemoryPool *mp) : CScalar(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarValuesList::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarValuesList::Matches(COperator *pop) const
{
	return pop->Eopid() == Eopid();
}

// EOF
