//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubqueryQuantified.cpp
//
//	@doc:
//		Implementation of quantified subquery operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarSubqueryQuantified.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/xforms/CSubqueryHandler.h"
#include "naucrates/md/IMDScalarOp.h"
#include "naucrates/md/IMDTypeBool.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::CScalarSubqueryQuantified
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarSubqueryQuantified::CScalarSubqueryQuantified(
	CMemoryPool *mp, IMDId *scalar_op_mdid, const CWStringConst *pstrScalarOp,
	const CColRef *colref)
	: CScalar(mp),
	  m_scalar_op_mdid(scalar_op_mdid),
	  m_pstrScalarOp(pstrScalarOp),
	  m_pcr(colref)
{
	SPQOS_ASSERT(scalar_op_mdid->IsValid());
	SPQOS_ASSERT(NULL != pstrScalarOp);
	SPQOS_ASSERT(NULL != colref);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::~CScalarSubqueryQuantified
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CScalarSubqueryQuantified::~CScalarSubqueryQuantified()
{
	m_scalar_op_mdid->Release();
	SPQOS_DELETE(m_pstrScalarOp);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::PstrOp
//
//	@doc:
//		Operator name
//
//---------------------------------------------------------------------------
const CWStringConst *
CScalarSubqueryQuantified::PstrOp() const
{
	return m_pstrScalarOp;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::MdIdOp
//
//	@doc:
//		Scalar operator metadata id
//
//---------------------------------------------------------------------------
IMDId *
CScalarSubqueryQuantified::MdIdOp() const
{
	return m_scalar_op_mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::MdidType
//
//	@doc:
//		Type of scalar's value
//
//---------------------------------------------------------------------------
IMDId *
CScalarSubqueryQuantified::MdidType() const
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	IMDId *mdid_type =
		md_accessor->RetrieveScOp(m_scalar_op_mdid)->GetResultTypeMdid();

	SPQOS_ASSERT(
		md_accessor->PtMDType<IMDTypeBool>()->MDId()->Equals(mdid_type));

	return mdid_type;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CScalarSubqueryQuantified::HashValue() const
{
	return spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(m_scalar_op_mdid->HashValue(),
							spqos::HashPtr<CColRef>(m_pcr)));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarSubqueryQuantified::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	// match if contents are identical
	CScalarSubqueryQuantified *popSsq =
		CScalarSubqueryQuantified::PopConvert(pop);
	return popSsq->Pcr() == m_pcr && popSsq->MdIdOp()->Equals(m_scalar_op_mdid);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::PcrsUsed
//
//	@doc:
//		Locally used columns
//
//---------------------------------------------------------------------------
CColRefSet *
CScalarSubqueryQuantified::PcrsUsed(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	// used columns is an empty set unless subquery column is an outer reference
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);

	CColRefSet *pcrsChildOutput =
		exprhdl.DeriveOutputColumns(0 /* child_index */);
	if (!pcrsChildOutput->FMember(m_pcr))
	{
		// subquery column is not produced by relational child, add it to used columns
		pcrs->Include(m_pcr);
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::DerivePartitionInfo
//
//	@doc:
//		Derive partition consumers
//
//---------------------------------------------------------------------------
CPartInfo *
CScalarSubqueryQuantified::PpartinfoDerive(CMemoryPool *,  // mp,
										   CExpressionHandle &exprhdl) const
{
	CPartInfo *ppartinfoChild = exprhdl.DerivePartitionInfo(0);
	SPQOS_ASSERT(NULL != ppartinfoChild);
	ppartinfoChild->AddRef();
	return ppartinfoChild;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryQuantified::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarSubqueryQuantified::OsPrint(IOstream &os) const
{
	os << SzId();
	os << "(" << PstrOp()->GetBuffer() << ")";
	os << "[";
	m_pcr->OsPrint(os);
	os << "]";

	return os;
}


// EOF
