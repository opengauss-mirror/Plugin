//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2015 Pivotal Inc.
//
//	@filename:
//		CScalarMinMax.cpp
//
//	@doc:
//		Implementation of scalar MinMax operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarMinMax.h"

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
//		CScalarMinMax::CScalarMinMax
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarMinMax::CScalarMinMax(CMemoryPool *mp, IMDId *mdid_type,
							 EScalarMinMaxType esmmt)
	: CScalar(mp),
	  m_mdid_type(mdid_type),
	  m_esmmt(esmmt),
	  m_fBoolReturnType(false)
{
	SPQOS_ASSERT(mdid_type->IsValid());
	SPQOS_ASSERT(EsmmtSentinel > esmmt);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_fBoolReturnType = CMDAccessorUtils::FBoolType(md_accessor, m_mdid_type);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarMinMax::~CScalarMinMax
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScalarMinMax::~CScalarMinMax()
{
	m_mdid_type->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarMinMax::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		return type id
//
//---------------------------------------------------------------------------
ULONG
CScalarMinMax::HashValue() const
{
	ULONG ulminmax = (ULONG) this->Esmmt();

	return spqos::CombineHashes(
		m_mdid_type->HashValue(),
		spqos::CombineHashes(COperator::HashValue(),
							spqos::HashValue<ULONG>(&ulminmax)));
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarMinMax::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarMinMax::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CScalarMinMax *popScMinMax = CScalarMinMax::PopConvert(pop);

	// match if return types are identical
	return popScMinMax->Esmmt() == m_esmmt &&
		   popScMinMax->MdidType()->Equals(m_mdid_type);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarMinMax::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarMinMax::OsPrint(IOstream &os) const
{
	os << SzId() << " (";

	if (EsmmtMin == m_esmmt)
	{
		os << "Min";
	}
	else
	{
		os << "Max";
	}
	os << ")";

	return os;
}

// EOF
