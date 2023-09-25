//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarWindowFunc.cpp
//
//	@doc:
//		Implementation of scalar window function call operators
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarWindowFunc.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/mdcache/CMDAccessorUtils.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/operators/CScalarFunc.h"
#include "naucrates/md/IMDAggregate.h"
#include "naucrates/md/IMDFunction.h"

using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarWindowFunc::CScalarWindowFunc
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarWindowFunc::CScalarWindowFunc(CMemoryPool *mp, IMDId *mdid_func,
									 IMDId *mdid_return_type,
									 const CWStringConst *pstrFunc,
									 EWinStage ewinstage, BOOL is_distinct,
									 BOOL is_star_arg, BOOL is_simple_agg)
	: CScalarFunc(mp),
	  m_ewinstage(ewinstage),
	  m_is_distinct(is_distinct),
	  m_is_star_arg(is_star_arg),
	  m_is_simple_agg(is_simple_agg),
	  m_fAgg(false)
{
	SPQOS_ASSERT(mdid_func->IsValid());
	SPQOS_ASSERT(mdid_return_type->IsValid());
	m_func_mdid = mdid_func;
	m_return_type_mdid = mdid_return_type;
	m_pstrFunc = pstrFunc;

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	m_fAgg = md_accessor->FAggWindowFunc(m_func_mdid);
	if (!m_fAgg)
	{
		const IMDFunction *pmdfunc = md_accessor->RetrieveFunc(m_func_mdid);
		m_efs = pmdfunc->GetFuncStability();
	}
	else
	{
		// TODO: , Aug 15, 2012; pull out properties of aggregate functions
		m_efs = IMDFunction::EfsImmutable;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarWindowFunc::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CScalarWindowFunc::HashValue() const
{
	return spqos::CombineHashes(
		CombineHashes(
			CombineHashes(
				CombineHashes(
					spqos::CombineHashes(
						COperator::HashValue(),
						spqos::CombineHashes(m_func_mdid->HashValue(),
											m_return_type_mdid->HashValue())),
					m_ewinstage),
				spqos::HashValue<BOOL>(&m_is_distinct)),
			spqos::HashValue<BOOL>(&m_is_star_arg)),
		spqos::HashValue<BOOL>(&m_is_simple_agg));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarWindowFunc::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarWindowFunc::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarWindowFunc *popFunc = CScalarWindowFunc::PopConvert(pop);

		// match if the func id, and properties are identical
		return ((popFunc->IsDistinct() == m_is_distinct) &&
				(popFunc->IsStarArg() == m_is_star_arg) &&
				(popFunc->IsSimpleAgg() == m_is_simple_agg) &&
				(popFunc->FAgg() == m_fAgg) &&
				m_func_mdid->Equals(popFunc->FuncMdId()) &&
				m_return_type_mdid->Equals(popFunc->MdidType()) &&
				(popFunc->Ews() == m_ewinstage));
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarWindowFunc::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarWindowFunc::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << PstrFunc()->GetBuffer();
	os << " , Agg: " << (m_fAgg ? "true" : "false");
	os << " , Distinct: " << (m_is_distinct ? "true" : "false");
	os << " , StarArgument: " << (m_is_star_arg ? "true" : "false");
	os << " , SimpleAgg: " << (m_is_simple_agg ? "true" : "false");
	os << ")";

	return os;
}

// EOF
