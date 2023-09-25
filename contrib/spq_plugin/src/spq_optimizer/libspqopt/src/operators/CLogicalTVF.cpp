//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp
//
//	@filename:
//		CLogicalTVF.cpp
//
//	@doc:
//		Implementation of table functions
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalTVF.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::CLogicalTVF
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalTVF::CLogicalTVF(CMemoryPool *mp)
	: CLogical(mp),
	  m_func_mdid(NULL),
	  m_return_type_mdid(NULL),
	  m_pstr(NULL),
	  m_pdrspqcoldesc(NULL),
	  m_pdrspqcrOutput(NULL),
	  m_efs(IMDFunction::EfsImmutable),
	  m_returns_set(true)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::CLogicalTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalTVF::CLogicalTVF(CMemoryPool *mp, IMDId *mdid_func,
						 IMDId *mdid_return_type, CWStringConst *str,
						 CColumnDescriptorArray *pdrspqcoldesc)
	: CLogical(mp),
	  m_func_mdid(mdid_func),
	  m_return_type_mdid(mdid_return_type),
	  m_pstr(str),
	  m_pdrspqcoldesc(pdrspqcoldesc),
	  m_pdrspqcrOutput(NULL)
{
	SPQOS_ASSERT(mdid_return_type->IsValid());
	SPQOS_ASSERT(NULL != str);
	SPQOS_ASSERT(NULL != pdrspqcoldesc);

	// generate a default column set for the list of column descriptors
	m_pdrspqcrOutput = PdrspqcrCreateMapping(mp, pdrspqcoldesc, UlOpId());

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	if (mdid_func->IsValid())
	{
		const IMDFunction *pmdfunc = md_accessor->RetrieveFunc(m_func_mdid);

		m_efs = pmdfunc->GetFuncStability();
		m_returns_set = pmdfunc->ReturnsSet();
	}
	else
	{
		m_efs = spqmd::IMDFunction::EfsImmutable;
		m_returns_set = false;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::CLogicalTVF
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalTVF::CLogicalTVF(CMemoryPool *mp, IMDId *mdid_func,
						 IMDId *mdid_return_type, CWStringConst *str,
						 CColumnDescriptorArray *pdrspqcoldesc,
						 CColRefArray *pdrspqcrOutput)
	: CLogical(mp),
	  m_func_mdid(mdid_func),
	  m_return_type_mdid(mdid_return_type),
	  m_pstr(str),
	  m_pdrspqcoldesc(pdrspqcoldesc),
	  m_pdrspqcrOutput(pdrspqcrOutput)
{
	SPQOS_ASSERT(mdid_func->IsValid());
	SPQOS_ASSERT(mdid_return_type->IsValid());
	SPQOS_ASSERT(NULL != str);
	SPQOS_ASSERT(NULL != pdrspqcoldesc);
	SPQOS_ASSERT(NULL != pdrspqcrOutput);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDFunction *pmdfunc = md_accessor->RetrieveFunc(m_func_mdid);

	m_efs = pmdfunc->GetFuncStability();
	m_returns_set = pmdfunc->ReturnsSet();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::~CLogicalTVF
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalTVF::~CLogicalTVF()
{
	CRefCount::SafeRelease(m_func_mdid);
	CRefCount::SafeRelease(m_return_type_mdid);
	CRefCount::SafeRelease(m_pdrspqcoldesc);
	CRefCount::SafeRelease(m_pdrspqcrOutput);
	SPQOS_DELETE(m_pstr);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalTVF::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(
			m_func_mdid->HashValue(),
			spqos::CombineHashes(
				m_return_type_mdid->HashValue(),
				spqos::HashPtr<CColumnDescriptorArray>(m_pdrspqcoldesc))));

	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));
	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalTVF::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalTVF *popTVF = CLogicalTVF::PopConvert(pop);

	return m_func_mdid->Equals(popTVF->FuncMdId()) &&
		   m_return_type_mdid->Equals(popTVF->ReturnTypeMdId()) &&
		   m_pdrspqcoldesc->Equals(popTVF->Pdrspqcoldesc()) &&
		   m_pdrspqcrOutput->Equals(popTVF->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalTVF::PopCopyWithRemappedColumns(CMemoryPool *mp,
										UlongToColRefMap *colref_mapping,
										BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput = NULL;
	if (must_exist)
	{
		pdrspqcrOutput =
			CUtils::PdrspqcrRemapAndCreate(mp, m_pdrspqcrOutput, colref_mapping);
	}
	else
	{
		pdrspqcrOutput = CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput,
											 colref_mapping, must_exist);
	}

	CWStringConst *str = SPQOS_NEW(mp) CWStringConst(m_pstr->GetBuffer());
	m_func_mdid->AddRef();
	m_return_type_mdid->AddRef();
	m_pdrspqcoldesc->AddRef();

	return SPQOS_NEW(mp) CLogicalTVF(mp, m_func_mdid, m_return_type_mdid, str,
									m_pdrspqcoldesc, pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalTVF::DeriveOutputColumns(CMemoryPool *mp,
								 CExpressionHandle &  // exprhdl
)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(m_pdrspqcrOutput);

	return pcrs;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::DeriveFunctionProperties
//
//	@doc:
//		Derive function properties
//
//---------------------------------------------------------------------------
CFunctionProp *
CLogicalTVF::DeriveFunctionProperties(CMemoryPool *mp,
									  CExpressionHandle &exprhdl) const
{
	BOOL fVolatileScan = (IMDFunction::EfsVolatile == m_efs);
	return PfpDeriveFromChildren(mp, exprhdl, m_efs, fVolatileScan,
								 true /*fScan*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::FInputOrderSensitive
//
//	@doc:
//		Sensitivity to input order
//
//---------------------------------------------------------------------------
BOOL
CLogicalTVF::FInputOrderSensitive() const
{
	return true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalTVF::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfUnnestTVF);
	(void) xform_set->ExchangeSet(CXform::ExfImplementTVF);
	(void) xform_set->ExchangeSet(CXform::ExfImplementTVFNoArgs);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalTVF::DeriveMaxCard(CMemoryPool *,		// mp
						   CExpressionHandle &	// exprhdl
) const
{
	if (m_returns_set)
	{
		// unbounded by default
		return CMaxCard();
	}

	return CMaxCard(1);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------

IStatistics *
CLogicalTVF::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
						  IStatisticsArray *  // stats_ctxt
) const
{
	CDouble rows(1.0);
	if (m_returns_set)
	{
		rows = CStatistics::DefaultRelationRows;
	}

	return PstatsDeriveDummy(mp, exprhdl, rows);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalTVF::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalTVF::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}
	os << SzId() << " (" << Pstr()->GetBuffer() << ") ";
	os << "Columns: [";
	CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
	os << "] ";

	return os;
}

// EOF
