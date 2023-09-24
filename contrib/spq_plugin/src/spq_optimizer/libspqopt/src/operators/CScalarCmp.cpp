//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CScalarCmp.cpp
//
//	@doc:
//		Implementation of scalar comparison operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarCmp.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::CScalarCmp
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarCmp::CScalarCmp(CMemoryPool *mp, IMDId *mdid_op,
					   const CWStringConst *pstrOp, IMDType::ECmpType cmp_type)
	: CScalar(mp),
	  m_mdid_op(mdid_op),
	  m_pstrOp(pstrOp),
	  m_comparision_type(cmp_type),
	  m_returns_null_on_null_input(false)
{
	SPQOS_ASSERT(mdid_op->IsValid());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_returns_null_on_null_input =
		CMDAccessorUtils::FScalarOpReturnsNullOnNullInput(md_accessor,
														  m_mdid_op);
	m_fCommutative =
		CMDAccessorUtils::FCommutativeScalarOp(md_accessor, m_mdid_op);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::GetMDName
//
//	@doc:
//		Comparison operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CScalarCmp::Pstr() const
{
	return m_pstrOp;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::MdIdOp
//
//	@doc:
//		Comparison operator metadata id
//
//---------------------------------------------------------------------------
IMDId *
CScalarCmp::MdIdOp() const
{
	return m_mdid_op;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::HashValue
//
//	@doc:
//		Operator specific hash function; combined hash of operator id and
//		metadata id
//
//---------------------------------------------------------------------------
ULONG
CScalarCmp::HashValue() const
{
	return spqos::CombineHashes(COperator::HashValue(), m_mdid_op->HashValue());
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarCmp::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarCmp *popScCmp = CScalarCmp::PopConvert(pop);

		// match if operator oid are identical
		return m_mdid_op->Equals(popScCmp->MdIdOp());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::FInputOrderSensitive
//
//	@doc:
//		Sensitivity to order of inputs
//
//---------------------------------------------------------------------------
BOOL
CScalarCmp::FInputOrderSensitive() const
{
	return !m_fCommutative;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::MdidType
//
//	@doc:
//		Expression type
//
//---------------------------------------------------------------------------
IMDId *
CScalarCmp::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	return md_accessor->PtMDType<IMDTypeBool>()->MDId();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::Eber
//
//	@doc:
//		Perform boolean expression evaluation
//
//---------------------------------------------------------------------------
CScalar::EBoolEvalResult
CScalarCmp::Eber(ULongPtrArray *pdrspqulChildren) const
{
	if (m_returns_null_on_null_input)
	{
		return EberNullOnAnyNullChild(pdrspqulChildren);
	}

	return EberAny;
}

// get metadata id of the commuted operator
IMDId *
CScalarCmp::PmdidCommuteOp(CMDAccessor *md_accessor, COperator *pop)
{
	CScalarCmp *popScalarCmp = dynamic_cast<CScalarCmp *>(pop);
	const IMDScalarOp *pmdScalarCmpOp =
		md_accessor->RetrieveScOp(popScalarCmp->MdIdOp());

	IMDId *pmdidScalarCmpCommute = pmdScalarCmpOp->GetCommuteOpMdid();
	return pmdidScalarCmpCommute;
}

// get the string representation of a metadata object
CWStringConst *
CScalarCmp::Pstr(CMemoryPool *mp, CMDAccessor *md_accessor, IMDId *mdid)
{
	mdid->AddRef();
	return SPQOS_NEW(mp) CWStringConst(
		mp,
		(md_accessor->RetrieveScOp(mdid)->Mdname().GetMDName())->GetBuffer());
}

// get commuted scalar comparision operator
CScalarCmp *
CScalarCmp::PopCommutedOp(CMemoryPool *mp, COperator *pop)
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid = PmdidCommuteOp(md_accessor, pop);
	if (NULL != mdid && mdid->IsValid())
	{
		return SPQOS_NEW(mp) CScalarCmp(mp, mdid, Pstr(mp, md_accessor, mdid),
									   CUtils::ParseCmpType(mdid));
	}
	return NULL;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarCmp::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarCmp::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << Pstr()->GetBuffer();
	os << ")";

	return os;
}


// EOF
