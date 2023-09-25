//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalRowTrigger.cpp
//
//	@doc:
//		Implementation of logical row-level trigger operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalRowTrigger.h"

#include "spqos/base.h"

#include "spqopt/base/COptCtxt.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/md/CMDTriggerSPQDB.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::CLogicalRowTrigger
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalRowTrigger::CLogicalRowTrigger(CMemoryPool *mp)
	: CLogical(mp),
	  m_rel_mdid(NULL),
	  m_type(0),
	  m_pdrspqcrOld(NULL),
	  m_pdrspqcrNew(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::CLogicalRowTrigger
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalRowTrigger::CLogicalRowTrigger(CMemoryPool *mp, IMDId *rel_mdid,
									   INT type, CColRefArray *pdrspqcrOld,
									   CColRefArray *pdrspqcrNew)
	: CLogical(mp),
	  m_rel_mdid(rel_mdid),
	  m_type(type),
	  m_pdrspqcrOld(pdrspqcrOld),
	  m_pdrspqcrNew(pdrspqcrNew)
{
	SPQOS_ASSERT(rel_mdid->IsValid());
	SPQOS_ASSERT(0 != type);
	SPQOS_ASSERT(NULL != pdrspqcrNew || NULL != pdrspqcrOld);
	SPQOS_ASSERT_IMP(NULL != pdrspqcrNew && NULL != pdrspqcrOld,
					pdrspqcrNew->Size() == pdrspqcrOld->Size());
	InitFunctionProperties();
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::~CLogicalRowTrigger
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CLogicalRowTrigger::~CLogicalRowTrigger()
{
	CRefCount::SafeRelease(m_rel_mdid);
	CRefCount::SafeRelease(m_pdrspqcrOld);
	CRefCount::SafeRelease(m_pdrspqcrNew);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::InitFunctionProperties
//
//	@doc:
//		Initialize function properties
//
//---------------------------------------------------------------------------
void
CLogicalRowTrigger::InitFunctionProperties()
{
	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(m_rel_mdid);
	const ULONG ulTriggers = pmdrel->TriggerCount();

	for (ULONG ul = 0; ul < ulTriggers; ul++)
	{
		const IMDTrigger *pmdtrigger =
			md_accessor->RetrieveTrigger(pmdrel->TriggerMDidAt(ul));
		if (!pmdtrigger->IsEnabled() || !pmdtrigger->ExecutesOnRowLevel() ||
			(ITriggerType(pmdtrigger) & m_type) != m_type)
		{
			continue;
		}

		const IMDFunction *pmdfunc =
			md_accessor->RetrieveFunc(pmdtrigger->FuncMdId());
		IMDFunction::EFuncStbl efs = pmdfunc->GetFuncStability();

		if (efs > m_efs)
		{
			m_efs = efs;
		}
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::ITriggerType
//
//	@doc:
//		Return the type of a given trigger as an integer
//
//---------------------------------------------------------------------------
INT
CLogicalRowTrigger::ITriggerType(const IMDTrigger *pmdtrigger) const
{
	INT type = SPQMD_TRIGGER_ROW;
	if (pmdtrigger->IsBefore())
	{
		type |= SPQMD_TRIGGER_BEFORE;
	}

	if (pmdtrigger->IsInsert())
	{
		type |= SPQMD_TRIGGER_INSERT;
	}

	if (pmdtrigger->IsDelete())
	{
		type |= SPQMD_TRIGGER_DELETE;
	}

	if (pmdtrigger->IsUpdate())
	{
		type |= SPQMD_TRIGGER_UPDATE;
	}

	return type;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalRowTrigger::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalRowTrigger *popRowTrigger = CLogicalRowTrigger::PopConvert(pop);

	return m_rel_mdid->Equals(popRowTrigger->GetRelMdId()) &&
		   m_type == popRowTrigger->GetType() &&
		   m_pdrspqcrOld->Equals(popRowTrigger->PdrspqcrOld()) &&
		   m_pdrspqcrNew->Equals(popRowTrigger->PdrspqcrNew());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalRowTrigger::HashValue() const
{
	ULONG ulHash =
		spqos::CombineHashes(COperator::HashValue(), m_rel_mdid->HashValue());
	ulHash = spqos::CombineHashes(ulHash, spqos::HashValue<INT>(&m_type));

	if (NULL != m_pdrspqcrOld)
	{
		ulHash =
			spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOld));
	}

	if (NULL != m_pdrspqcrNew)
	{
		ulHash =
			spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrNew));
	}

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalRowTrigger::PopCopyWithRemappedColumns(CMemoryPool *mp,
											   UlongToColRefMap *colref_mapping,
											   BOOL must_exist)
{
	CColRefArray *pdrspqcrOld = NULL;
	if (NULL != m_pdrspqcrOld)
	{
		pdrspqcrOld =
			CUtils::PdrspqcrRemap(mp, m_pdrspqcrOld, colref_mapping, must_exist);
	}

	CColRefArray *pdrspqcrNew = NULL;
	if (NULL != m_pdrspqcrNew)
	{
		pdrspqcrNew =
			CUtils::PdrspqcrRemap(mp, m_pdrspqcrNew, colref_mapping, must_exist);
	}

	m_rel_mdid->AddRef();

	return SPQOS_NEW(mp)
		CLogicalRowTrigger(mp, m_rel_mdid, m_type, pdrspqcrOld, pdrspqcrNew);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalRowTrigger::DeriveOutputColumns(CMemoryPool *,	//mp,
										CExpressionHandle &exprhdl)
{
	return PcrsDeriveOutputPassThru(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalRowTrigger::DeriveKeyCollection(CMemoryPool *,	// mp
										CExpressionHandle &exprhdl) const
{
	return PkcDeriveKeysPassThru(exprhdl, 0 /* ulChild */);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalRowTrigger::DeriveMaxCard(CMemoryPool *,  // mp
								  CExpressionHandle &exprhdl) const
{
	// pass on max card of first child
	return exprhdl.DeriveMaxCard(0);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalRowTrigger::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementRowTrigger);
	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalRowTrigger::PstatsDerive(CMemoryPool *,	 // mp,
								 CExpressionHandle &exprhdl,
								 IStatisticsArray *	 // not used
) const
{
	return PstatsPassThruOuter(exprhdl);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::DeriveFunctionProperties
//
//	@doc:
//		Derive function properties
//
//---------------------------------------------------------------------------
CFunctionProp *
CLogicalRowTrigger::DeriveFunctionProperties(CMemoryPool *mp,
											 CExpressionHandle &exprhdl) const
{
	return PfpDeriveFromChildren(mp, exprhdl, m_efs,
								 false /*fHasVolatileFunctionScan*/,
								 false /*fScan*/);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalRowTrigger::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalRowTrigger::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}

	os << SzId() << " (Type: " << m_type << ")";

	if (NULL != m_pdrspqcrOld)
	{
		os << ", Old Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOld);
		os << "]";
	}

	if (NULL != m_pdrspqcrNew)
	{
		os << ", New Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrNew);
		os << "]";
	}

	return os;
}

// EOF
