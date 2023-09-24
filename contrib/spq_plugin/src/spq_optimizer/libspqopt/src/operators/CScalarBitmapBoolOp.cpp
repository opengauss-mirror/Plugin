//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CScalarBitmapBoolOp.cpp
//
//	@doc:
//		Bitmap index probe scalar operator
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarBitmapBoolOp.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/metadata/CIndexDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/xforms/CXform.h"
#include "naucrates/statistics/CStatisticsUtils.h"

using namespace spqopt;

const WCHAR CScalarBitmapBoolOp::m_rgwszBitmapOpType[EbitmapboolSentinel][30] =
	{SPQOS_WSZ_LIT("BitmapAnd"), SPQOS_WSZ_LIT("BitmapOr")};

//---------------------------------------------------------------------------
//	@function:
//		CScalarBitmapBoolOp::CScalarBitmapBoolOp
//
//	@doc:
//		Ctor
//		Takes ownership of the bitmap type id.
//
//---------------------------------------------------------------------------
CScalarBitmapBoolOp::CScalarBitmapBoolOp(CMemoryPool *mp,
										 EBitmapBoolOp ebitmapboolop,
										 IMDId *pmdidBitmapType)
	: CScalar(mp),
	  m_ebitmapboolop(ebitmapboolop),
	  m_pmdidBitmapType(pmdidBitmapType)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(EbitmapboolSentinel > ebitmapboolop);
	SPQOS_ASSERT(NULL != pmdidBitmapType);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarBitmapBoolOp::~CScalarBitmapBoolOp
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScalarBitmapBoolOp::~CScalarBitmapBoolOp()
{
	m_pmdidBitmapType->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarBitmapBoolOp::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CScalarBitmapBoolOp::HashValue() const
{
	ULONG ulBoolop = (ULONG) Ebitmapboolop();
	return spqos::CombineHashes(COperator::HashValue(),
							   spqos::HashValue<ULONG>(&ulBoolop));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarBitmapBoolOp::Matches
//
//	@doc:
//		Match this operator with the given one.
//
//---------------------------------------------------------------------------
BOOL
CScalarBitmapBoolOp::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}
	CScalarBitmapBoolOp *popBitmapBoolOp = PopConvert(pop);

	return popBitmapBoolOp->Ebitmapboolop() == Ebitmapboolop() &&
		   popBitmapBoolOp->MdidType()->Equals(m_pmdidBitmapType);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarBitmapBoolOp::OsPrint
//
//	@doc:
//		Debug print of this operator
//
//---------------------------------------------------------------------------
IOstream &
CScalarBitmapBoolOp::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << m_rgwszBitmapOpType[m_ebitmapboolop];
	os << ")";

	return os;
}

// EOF
