//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarIf.cpp
//
//	@doc:
//		Implementation of scalar if operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarIf.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;


//---------------------------------------------------------------------------
//	@function:
//		CScalarIf::CScalarIf
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarIf::CScalarIf(CMemoryPool *mp, IMDId *mdid)
	: CScalar(mp), m_mdid_type(mdid), m_fBoolReturnType(false)
{
	SPQOS_ASSERT(mdid->IsValid());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_fBoolReturnType = CMDAccessorUtils::FBoolType(md_accessor, m_mdid_type);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarIf::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		return type id
//
//---------------------------------------------------------------------------
ULONG
CScalarIf::HashValue() const
{
	return spqos::CombineHashes(COperator::HashValue(),
							   m_mdid_type->HashValue());
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarIf::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarIf::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarIf *popScIf = CScalarIf::PopConvert(pop);

		// match if return types are identical
		return popScIf->MdidType()->Equals(m_mdid_type);
	}

	return false;
}

// EOF
