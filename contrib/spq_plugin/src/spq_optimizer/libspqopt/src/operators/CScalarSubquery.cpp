//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarSubquery.cpp
//
//	@doc:
//		Implementation of scalar subqueries
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarSubquery.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/operators/CExpressionHandle.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::CScalarSubquery
//
//	@doc:
//		Constructor
//
//---------------------------------------------------------------------------
CScalarSubquery::CScalarSubquery(CMemoryPool *mp, const CColRef *colref,
								 BOOL fGeneratedByExist,
								 BOOL fGeneratedByQuantified)
	: CScalar(mp),
	  m_pcr(colref),
	  m_fGeneratedByExist(fGeneratedByExist),
	  m_fGeneratedByQuantified(fGeneratedByQuantified)
{
	SPQOS_ASSERT(NULL != colref);
	SPQOS_ASSERT(!(fGeneratedByExist && fGeneratedByQuantified));
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::~CScalarSubquery
//
//	@doc:
//		Destructor
//
//---------------------------------------------------------------------------
CScalarSubquery::~CScalarSubquery()
{
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::MdidType
//
//	@doc:
//		Type of scalar's value
//
//---------------------------------------------------------------------------
IMDId *
CScalarSubquery::MdidType() const
{
	return m_pcr->RetrieveType()->MDId();
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CScalarSubquery::HashValue() const
{
	return spqos::CombineHashes(COperator::HashValue(),
							   spqos::HashPtr<CColRef>(m_pcr));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarSubquery::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarSubquery *popScalarSubquery = CScalarSubquery::PopConvert(pop);

		// match if computed columns are identical
		return popScalarSubquery->Pcr() == m_pcr &&
			   popScalarSubquery->FGeneratedByQuantified() ==
				   m_fGeneratedByQuantified &&
			   popScalarSubquery->FGeneratedByExist() == m_fGeneratedByExist;
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CScalarSubquery::PopCopyWithRemappedColumns(CMemoryPool *mp,
											UlongToColRefMap *colref_mapping,
											BOOL must_exist)
{
	CColRef *colref = CUtils::PcrRemap(m_pcr, colref_mapping, must_exist);

	return SPQOS_NEW(mp) CScalarSubquery(mp, colref, m_fGeneratedByExist,
										m_fGeneratedByQuantified);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::PcrsUsed
//
//	@doc:
//		Locally used columns
//
//---------------------------------------------------------------------------
CColRefSet *
CScalarSubquery::PcrsUsed(CMemoryPool *mp, CExpressionHandle &exprhdl)
{
	SPQOS_ASSERT(1 == exprhdl.Arity());

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
//		CScalarSubquery::DerivePartitionInfo
//
//	@doc:
//		Derive partition consumers
//
//---------------------------------------------------------------------------
CPartInfo *
CScalarSubquery::PpartinfoDerive(CMemoryPool *,	 // mp,
								 CExpressionHandle &exprhdl) const
{
	CPartInfo *ppartinfoChild = exprhdl.DerivePartitionInfo(0);
	SPQOS_ASSERT(NULL != ppartinfoChild);
	ppartinfoChild->AddRef();
	return ppartinfoChild;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubquery::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarSubquery::OsPrint(IOstream &os) const
{
	os << SzId() << "[";
	m_pcr->OsPrint(os);
	os << "]";

	if (m_fGeneratedByExist)
	{
		os << " generated by Exist SQ";
	}
	if (m_fGeneratedByQuantified)
	{
		os << " generated by Quantified SQ";
	}
	return os;
}


// EOF
