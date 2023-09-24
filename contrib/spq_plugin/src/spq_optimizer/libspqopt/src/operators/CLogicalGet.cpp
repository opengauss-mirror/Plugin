//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CLogicalGet.cpp
//
//	@doc:
//		Implementation of basic table access
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalGet.h"

#include "spqos/base.h"
#include "spqos/common/CAutoP.h"
#include "spqos/common/CDynamicPtrArray.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CKeyCollection.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "spqopt/translate/CTranslatorDXLToExpr.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::CLogicalGet
//
//	@doc:
//		ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalGet::CLogicalGet(CMemoryPool *mp)
	: CLogical(mp),
	  m_pnameAlias(NULL),
	  m_ptabdesc(NULL),
	  m_pdrspqcrOutput(NULL),
	  m_pdrspqdrspqcrPart(NULL),
	  m_pcrsDist(NULL)
{
	m_fPattern = true;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::CLogicalGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGet::CLogicalGet(CMemoryPool *mp, const CName *pnameAlias,
						 CTableDescriptor *ptabdesc)
	: CLogical(mp),
	  m_pnameAlias(pnameAlias),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcrOutput(NULL),
	  m_pdrspqdrspqcrPart(NULL),
	  m_pcrsDist(NULL)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);

	// generate a default column set for the table descriptor
	m_pdrspqcrOutput = PdrspqcrCreateMapping(mp, m_ptabdesc->Pdrspqcoldesc(),
										   UlOpId(), m_ptabdesc->MDId());

	if (m_ptabdesc->IsPartitioned())
	{
		m_pdrspqdrspqcrPart = PdrspqdrspqcrCreatePartCols(
			mp, m_pdrspqcrOutput, m_ptabdesc->PdrspqulPart());
	}

	m_pcrsDist = CLogical::PcrsDist(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::CLogicalGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalGet::CLogicalGet(CMemoryPool *mp, const CName *pnameAlias,
						 CTableDescriptor *ptabdesc,
						 CColRefArray *pdrspqcrOutput)
	: CLogical(mp),
	  m_pnameAlias(pnameAlias),
	  m_ptabdesc(ptabdesc),
	  m_pdrspqcrOutput(pdrspqcrOutput),
	  m_pdrspqdrspqcrPart(NULL)
{
	SPQOS_ASSERT(NULL != ptabdesc);
	SPQOS_ASSERT(NULL != pnameAlias);

	if (m_ptabdesc->IsPartitioned())
	{
		m_pdrspqdrspqcrPart = PdrspqdrspqcrCreatePartCols(
			mp, m_pdrspqcrOutput, m_ptabdesc->PdrspqulPart());
	}

	m_pcrsDist = CLogical::PcrsDist(mp, m_ptabdesc, m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::~CLogicalGet
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CLogicalGet::~CLogicalGet()
{
	CRefCount::SafeRelease(m_ptabdesc);
	CRefCount::SafeRelease(m_pdrspqcrOutput);
	CRefCount::SafeRelease(m_pdrspqdrspqcrPart);
	CRefCount::SafeRelease(m_pcrsDist);

	SPQOS_DELETE(m_pnameAlias);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(COperator::HashValue(),
									   m_ptabdesc->MDId()->HashValue());
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalGet::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}
	CLogicalGet *popGet = CLogicalGet::PopConvert(pop);

	return m_ptabdesc->MDId()->Equals(popGet->m_ptabdesc->MDId()) &&
		   m_pdrspqcrOutput->Equals(popGet->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalGet::PopCopyWithRemappedColumns(CMemoryPool *mp,
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
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, *m_pnameAlias);
	m_ptabdesc->AddRef();

	return SPQOS_NEW(mp) CLogicalGet(mp, pnameAlias, m_ptabdesc, pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGet::DeriveOutputColumns(CMemoryPool *mp,
								 CExpressionHandle &  // exprhdl
)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	for (ULONG i = 0; i < m_pdrspqcrOutput->Size(); i++)
	{
		// We want to limit the output columns to only those which are referenced in the query
		// We will know the entire list of columns which are referenced in the query only after
		// translating the entire DXL to an expression. Hence we should not limit the output columns
		// before we have processed the entire DXL.
		if ((*m_pdrspqcrOutput)[i]->GetUsage() == CColRef::EUsed ||
			(*m_pdrspqcrOutput)[i]->GetUsage() == CColRef::EUnknown)
		{
			pcrs->Include((*m_pdrspqcrOutput)[i]);
		}
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::DeriveNotNullColumns
//
//	@doc:
//		Derive not null output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalGet::DeriveNotNullColumns(CMemoryPool *mp,
								  CExpressionHandle &exprhdl) const
{
	// get all output columns
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(exprhdl.DeriveOutputColumns());

	// filters out nullable columns
	CColRefSetIter crsi(*exprhdl.DeriveOutputColumns());
	while (crsi.Advance())
	{
		CColRefTable *pcrtable =
			CColRefTable::PcrConvert(const_cast<CColRef *>(crsi.Pcr()));
		if (pcrtable->IsNullable())
		{
			pcrs->Exclude(pcrtable);
		}
	}

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::FInputOrderSensitive
//
//	@doc:
//		Not called for leaf operators
//
//---------------------------------------------------------------------------
BOOL
CLogicalGet::FInputOrderSensitive() const
{
	SPQOS_ASSERT(!"Unexpected function call of FInputOrderSensitive");
	return false;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::PkcDeriveKeys
//
//	@doc:
//		Derive key collection
//
//---------------------------------------------------------------------------
CKeyCollection *
CLogicalGet::DeriveKeyCollection(CMemoryPool *mp,
								 CExpressionHandle &  // exprhdl
) const
{
	const CBitSetArray *pdrspqbs = m_ptabdesc->PdrspqbsKeys();

	return CLogical::PkcKeysBaseTable(mp, pdrspqbs, m_pdrspqcrOutput);
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(CXform::ExfGet2TableScan);
    /* SPQ: for ShareScan */
    (void) xform_set->ExchangeSet(CXform::ExfGet2TableShareScan);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::PstatsDerive
//
//	@doc:
//		Load up statistics from metadata
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalGet::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
						  IStatisticsArray *  // not used
) const
{
	// requesting stats on distribution columns to estimate data skew
	IStatistics *pstatsTable =
		PstatsBaseTable(mp, exprhdl, m_ptabdesc, m_pcrsDist);

	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp, m_pdrspqcrOutput);
	CUpperBoundNDVs *upper_bound_NDVs =
		SPQOS_NEW(mp) CUpperBoundNDVs(pcrs, pstatsTable->Rows());
	CStatistics::CastStats(pstatsTable)->AddCardUpperBound(upper_bound_NDVs);

	return pstatsTable;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalGet::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalGet::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}
	else
	{
		os << SzId() << " ";
		// alias of table as referenced in the query
		m_pnameAlias->OsPrint(os);

		// actual name of table in catalog and columns
		os << " (";
		m_ptabdesc->Name().OsPrint(os);
		os << "), Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
		os << "] Key sets: {";

		const ULONG ulColumns = m_pdrspqcrOutput->Size();
		const CBitSetArray *pdrspqbsKeys = m_ptabdesc->PdrspqbsKeys();
		for (ULONG ul = 0; ul < pdrspqbsKeys->Size(); ul++)
		{
			CBitSet *pbs = (*pdrspqbsKeys)[ul];
			if (0 < ul)
			{
				os << ", ";
			}
			os << "[";
			ULONG ulPrintedKeys = 0;
			for (ULONG ulKey = 0; ulKey < ulColumns; ulKey++)
			{
				if (pbs->Get(ulKey))
				{
					if (0 < ulPrintedKeys)
					{
						os << ",";
					}
					os << ulKey;
					ulPrintedKeys++;
				}
			}
			os << "]";
		}
		os << "}";
	}

	return os;
}



// EOF
