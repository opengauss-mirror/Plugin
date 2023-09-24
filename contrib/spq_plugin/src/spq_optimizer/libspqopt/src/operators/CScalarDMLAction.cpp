//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarDMLAction.cpp
//
//	@doc:
//		Implementation of scalar DML action operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarDMLAction.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeInt4.h"

using namespace spqopt;
using namespace spqmd;


//---------------------------------------------------------------------------
//	@function:
//		CScalarDMLAction::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarDMLAction::Matches(COperator *pop) const
{
	return pop->Eopid() == Eopid();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarDMLAction::MdidType
//
//	@doc:
//		Operator type
//
//---------------------------------------------------------------------------
IMDId *
CScalarDMLAction::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	return md_accessor->PtMDType<IMDTypeInt4>()->MDId();
}
// EOF
