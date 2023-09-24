//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarCoalesce.cpp
//
//	@doc:
//		Implementation of scalar coalesce operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarCoalesce.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarCoalesce::CScalarCoalesce
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarCoalesce::CScalarCoalesce(CMemoryPool *mp, IMDId *mdid_type)
	: CScalar(mp), m_mdid_type(mdid_type), m_fBoolReturnType(false)
{
	SPQOS_ASSERT(mdid_type->IsValid());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_fBoolReturnType = CMDAccessorUtils::FBoolType(md_accessor, m_mdid_type);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCoalesce::~CScalarCoalesce
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScalarCoalesce::~CScalarCoalesce()
{
	m_mdid_type->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCoalesce::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		return type id
//
//---------------------------------------------------------------------------
ULONG
CScalarCoalesce::HashValue() const
{
	return spqos::CombineHashes(COperator::HashValue(),
							   m_mdid_type->HashValue());
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCoalesce::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarCoalesce::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarCoalesce *popScCoalesce = CScalarCoalesce::PopConvert(pop);

		// match if return types are identical
		return popScCoalesce->MdidType()->Equals(m_mdid_type);
	}

	return false;
}


// EOF
