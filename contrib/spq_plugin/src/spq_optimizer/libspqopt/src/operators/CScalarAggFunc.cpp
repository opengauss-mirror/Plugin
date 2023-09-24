//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CScalarAggFunc.cpp
//
//	@doc:
//		Implementation of scalar aggregate function call operators
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarAggFunc.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CDrvdPropScalar.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDAggregate.h"


using namespace spqopt;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::CScalarAggFunc
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarAggFunc::CScalarAggFunc(CMemoryPool *mp, IMDId *pmdidAggFunc,
							   IMDId *resolved_rettype,
							   const CWStringConst *pstrAggFunc,
							   BOOL is_distinct, EAggfuncStage eaggfuncstage,
							   BOOL fSplit, EAggfuncKind aggkind,
							   IMDId *spq_agg_mdid)
	: CScalar(mp),
	  m_pmdidAggFunc(pmdidAggFunc),
	  m_pmdidResolvedRetType(resolved_rettype),
	  m_return_type_mdid(NULL),
	  m_pstrAggFunc(pstrAggFunc),
	  m_is_distinct(is_distinct),
	  m_aggkind(aggkind),
	  m_eaggfuncstage(eaggfuncstage),
	  m_fSplit(fSplit),
	  m_spq_agg_mdid(spq_agg_mdid)
{
	SPQOS_ASSERT(NULL != pmdidAggFunc);
	SPQOS_ASSERT(NULL != pstrAggFunc);
	SPQOS_ASSERT(pmdidAggFunc->IsValid());
	SPQOS_ASSERT_IMP(NULL != resolved_rettype, resolved_rettype->IsValid());
	SPQOS_ASSERT(EaggfuncstageSentinel > eaggfuncstage);

	// store id of type obtained by looking up MD cache
	IMDId *mdid = PmdidLookupReturnType(
		m_pmdidAggFunc, (EaggfuncstageGlobal == m_eaggfuncstage));
	mdid->AddRef();
	m_return_type_mdid = mdid;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::PstrAggFunc
//
//	@doc:
//		Aggregate function name
//
//---------------------------------------------------------------------------
const CWStringConst *
CScalarAggFunc::PstrAggFunc() const
{
	return m_pstrAggFunc;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::MDId
//
//	@doc:
//		Aggregate function id
//
//---------------------------------------------------------------------------
IMDId *
CScalarAggFunc::MDId() const
{
	return m_pmdidAggFunc;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::FCountStar
//
//	@doc:
//		Is function count(*)?
//
//---------------------------------------------------------------------------
BOOL
CScalarAggFunc::FCountStar() const
{
	// TODO,  04/26/2012, make this function system-independent
	// using MDAccessor
	return m_pmdidAggFunc->Equals(&CMDIdSPQDB::m_mdid_count_star);
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::FCountAny
//
//	@doc:
//		Is function count(Any)?
//
//---------------------------------------------------------------------------
BOOL
CScalarAggFunc::FCountAny() const
{
	// TODO,  04/26/2012, make this function system-independent
	// using MDAccessor
	return m_pmdidAggFunc->Equals(&CMDIdSPQDB::m_mdid_count_any);
}

// Is function either min() or max()?
BOOL
CScalarAggFunc::IsMinMax(const IMDType *mdtype) const
{
	return m_pmdidAggFunc->Equals(
			   mdtype->GetMdidForAggType(IMDType::EaggMin)) ||
		   m_pmdidAggFunc->Equals(mdtype->GetMdidForAggType(IMDType::EaggMax));
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CScalarAggFunc::HashValue() const
{
	ULONG ulAggfuncstage = (ULONG) m_eaggfuncstage;
	return spqos::CombineHashes(
		CombineHashes(COperator::HashValue(), m_pmdidAggFunc->HashValue()),
		CombineHashes(spqos::HashValue<ULONG>(&ulAggfuncstage),
					  CombineHashes(spqos::HashValue<BOOL>(&m_is_distinct),
									spqos::HashValue<BOOL>(&m_fSplit))));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarAggFunc::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarAggFunc *popScAggFunc = CScalarAggFunc::PopConvert(pop);

		// match if func ids are identical
		return ((popScAggFunc->IsDistinct() == m_is_distinct) &&
				(popScAggFunc->Eaggfuncstage() == Eaggfuncstage()) &&
				(popScAggFunc->FSplit() == m_fSplit) &&
				m_pmdidAggFunc->Equals(popScAggFunc->MDId()));
	}

	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::PmdidLookupReturnType
//
//	@doc:
//		Lookup mdid of return type for given Agg function
//
//---------------------------------------------------------------------------
IMDId *
CScalarAggFunc::PmdidLookupReturnType(IMDId *pmdidAggFunc, BOOL fGlobal,
									  CMDAccessor *pmdaInput)
{
	SPQOS_ASSERT(NULL != pmdidAggFunc);
	CMDAccessor *md_accessor = pmdaInput;

	if (NULL == md_accessor)
	{
		md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	}
	SPQOS_ASSERT(NULL != md_accessor);

	// get aggregate function return type from the MD cache
	const IMDAggregate *pmdagg = md_accessor->RetrieveAgg(pmdidAggFunc);
	if (fGlobal)
	{
		return pmdagg->GetResultTypeMdid();
	}

	return pmdagg->GetIntermediateResultTypeMdid();
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarAggFunc::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarAggFunc::OsPrint(IOstream &os) const
{
	os << SzId() << " (";
	os << PstrAggFunc()->GetBuffer();
	os << " , Distinct: ";
	os << (m_is_distinct ? "true" : "false");
	os << " , Aggregate Stage: ";

	switch (m_eaggfuncstage)
	{
		case EaggfuncstageGlobal:
			os << "Global";
			break;

		case EaggfuncstageIntermediate:
			os << "Intermediate";
			break;

		case EaggfuncstageLocal:
			os << "Local";
			break;

		default:
			SPQOS_ASSERT(!"Unsupported aggregate type");
	}

	os << ")";

	return os;
}


// EOF
