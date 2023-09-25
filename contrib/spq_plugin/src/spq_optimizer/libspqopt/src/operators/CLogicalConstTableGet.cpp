//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp
//
//	@filename:
//		CLogicalConstTableGet.cpp
//
//	@doc:
//		Implementation of const table access
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalConstTableGet.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/operators/CExpressionHandle.h"
#include "naucrates/statistics/CStatistics.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::CLogicalConstTableGet
//
//	@doc:
//		ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalConstTableGet::CLogicalConstTableGet(CMemoryPool *mp)
	: CLogical(mp),
	  m_pdrspqcoldesc(NULL),
	  m_pdrspqdrspqdatum(NULL),
	  m_pdrspqcrOutput(NULL)
{
	m_fPattern = true;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::CLogicalConstTableGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalConstTableGet::CLogicalConstTableGet(
	CMemoryPool *mp, CColumnDescriptorArray *pdrspqcoldesc,
	IDatum2dArray *pdrspqdrspqdatum)
	: CLogical(mp),
	  m_pdrspqcoldesc(pdrspqcoldesc),
	  m_pdrspqdrspqdatum(pdrspqdrspqdatum),
	  m_pdrspqcrOutput(NULL)
{
	SPQOS_ASSERT(NULL != pdrspqcoldesc);
	SPQOS_ASSERT(NULL != pdrspqdrspqdatum);

	// generate a default column set for the list of column descriptors
	m_pdrspqcrOutput = PdrspqcrCreateMapping(mp, pdrspqcoldesc, UlOpId());

#ifdef SPQOS_DEBUG
	for (ULONG ul = 0; ul < pdrspqdrspqdatum->Size(); ul++)
	{
		IDatumArray *pdrspqdatum = (*pdrspqdrspqdatum)[ul];
		SPQOS_ASSERT(pdrspqdatum->Size() == pdrspqcoldesc->Size());
	}
#endif
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::CLogicalConstTableGet
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalConstTableGet::CLogicalConstTableGet(CMemoryPool *mp,
											 CColRefArray *pdrspqcrOutput,
											 IDatum2dArray *pdrspqdrspqdatum)
	: CLogical(mp),
	  m_pdrspqcoldesc(NULL),
	  m_pdrspqdrspqdatum(pdrspqdrspqdatum),
	  m_pdrspqcrOutput(pdrspqcrOutput)
{
	SPQOS_ASSERT(NULL != pdrspqcrOutput);
	SPQOS_ASSERT(NULL != pdrspqdrspqdatum);

	// generate column descriptors for the given output columns
	m_pdrspqcoldesc = PdrspqcoldescMapping(mp, pdrspqcrOutput);

#ifdef SPQOS_DEBUG
	for (ULONG ul = 0; ul < pdrspqdrspqdatum->Size(); ul++)
	{
		IDatumArray *pdrspqdatum = (*pdrspqdrspqdatum)[ul];
		SPQOS_ASSERT(pdrspqdatum->Size() == m_pdrspqcoldesc->Size());
	}
#endif
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::~CLogicalConstTableGet
//
//	@doc:
//		dtor
//
//---------------------------------------------------------------------------
CLogicalConstTableGet::~CLogicalConstTableGet()
{
	CRefCount::SafeRelease(m_pdrspqcoldesc);
	CRefCount::SafeRelease(m_pdrspqdrspqdatum);
	CRefCount::SafeRelease(m_pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::HashValue
//
//	@doc:
//		Operator specific hash function
//
//---------------------------------------------------------------------------
ULONG
CLogicalConstTableGet::HashValue() const
{
	ULONG ulHash = spqos::CombineHashes(
		COperator::HashValue(),
		spqos::CombineHashes(
			spqos::HashPtr<CColumnDescriptorArray>(m_pdrspqcoldesc),
			spqos::HashPtr<IDatum2dArray>(m_pdrspqdrspqdatum)));
	ulHash =
		spqos::CombineHashes(ulHash, CUtils::UlHashColArray(m_pdrspqcrOutput));

	return ulHash;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalConstTableGet::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}

	CLogicalConstTableGet *popCTG = CLogicalConstTableGet::PopConvert(pop);

	// match if column descriptors, const values and output columns are identical
	return m_pdrspqcoldesc->Equals(popCTG->Pdrspqcoldesc()) &&
		   m_pdrspqdrspqdatum->Equals(popCTG->Pdrspqdrspqdatum()) &&
		   m_pdrspqcrOutput->Equals(popCTG->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalConstTableGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *colref_array = NULL;
	if (must_exist)
	{
		colref_array =
			CUtils::PdrspqcrRemapAndCreate(mp, m_pdrspqcrOutput, colref_mapping);
	}
	else
	{
		colref_array = CUtils::PdrspqcrRemap(mp, m_pdrspqcrOutput, colref_mapping,
											must_exist);
	}
	m_pdrspqdrspqdatum->AddRef();

	return SPQOS_NEW(mp)
		CLogicalConstTableGet(mp, colref_array, m_pdrspqdrspqdatum);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::DeriveOutputColumns
//
//	@doc:
//		Derive output columns
//
//---------------------------------------------------------------------------
CColRefSet *
CLogicalConstTableGet::DeriveOutputColumns(CMemoryPool *mp,
										   CExpressionHandle &	// exprhdl
)
{
	CColRefSet *pcrs = SPQOS_NEW(mp) CColRefSet(mp);
	pcrs->Include(m_pdrspqcrOutput);

	return pcrs;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::DeriveMaxCard
//
//	@doc:
//		Derive max card
//
//---------------------------------------------------------------------------
CMaxCard
CLogicalConstTableGet::DeriveMaxCard(CMemoryPool *,		  // mp
									 CExpressionHandle &  // exprhdl
) const
{
	return CMaxCard(m_pdrspqdrspqdatum->Size());
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::FInputOrderSensitive
//
//	@doc:
//		Not called for leaf operators
//
//---------------------------------------------------------------------------
BOOL
CLogicalConstTableGet::FInputOrderSensitive() const
{
	SPQOS_ASSERT(!"Unexpected function call of FInputOrderSensitive");
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalConstTableGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementConstTableGet);
	return xform_set;
}


//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::PdrspqcoldescMapping
//
//	@doc:
//		Construct column descriptors from column references
//
//---------------------------------------------------------------------------
CColumnDescriptorArray *
CLogicalConstTableGet::PdrspqcoldescMapping(CMemoryPool *mp,
										   CColRefArray *colref_array) const
{
	SPQOS_ASSERT(NULL != colref_array);
	CColumnDescriptorArray *pdrspqcoldesc =
		SPQOS_NEW(mp) CColumnDescriptorArray(mp);

	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];

		ULONG length = spqos::ulong_max;
		if (CColRef::EcrtTable == colref->Ecrt())
		{
			CColRefTable *pcrTable = CColRefTable::PcrConvert(colref);
			length = pcrTable->Width();
		}

		CColumnDescriptor *pcoldesc = SPQOS_NEW(mp) CColumnDescriptor(
			mp, colref->RetrieveType(), colref->TypeModifier(), colref->Name(),
			ul + 1,	 //attno
			true,	 // IsNullable
			length);
		pdrspqcoldesc->Append(pcoldesc);
	}

	return pdrspqcoldesc;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::PstatsDerive
//
//	@doc:
//		Derive statistics
//
//---------------------------------------------------------------------------
IStatistics *
CLogicalConstTableGet::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl,
									IStatisticsArray *	// not used
) const
{
	SPQOS_ASSERT(Esp(exprhdl) > EspNone);
	CReqdPropRelational *prprel =
		CReqdPropRelational::GetReqdRelationalProps(exprhdl.Prp());
	CColRefSet *pcrs = prprel->PcrsStat();
	ULongPtrArray *colids = SPQOS_NEW(mp) ULongPtrArray(mp);
	pcrs->ExtractColIds(mp, colids);
	ULongPtrArray *pdrspqulColWidth = CUtils::Pdrspqul(mp, m_pdrspqcrOutput);

	IStatistics *stats = CStatistics::MakeDummyStats(
		mp, colids, pdrspqulColWidth, m_pdrspqdrspqdatum->Size());

	// clean up
	colids->Release();
	pdrspqulColWidth->Release();

	return stats;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalConstTableGet::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CLogicalConstTableGet::OsPrint(IOstream &os) const
{
	if (m_fPattern)
	{
		return COperator::OsPrint(os);
	}
	else
	{
		os << SzId() << " ";
		os << "Columns: [";
		CUtils::OsPrintDrgPcr(os, m_pdrspqcrOutput);
		os << "] ";
		os << "Values: [";
		for (ULONG ulA = 0; ulA < m_pdrspqdrspqdatum->Size(); ulA++)
		{
			if (0 < ulA)
			{
				os << "; ";
			}
			os << "(";
			IDatumArray *pdrspqdatum = (*m_pdrspqdrspqdatum)[ulA];

			const ULONG length = pdrspqdatum->Size();
			for (ULONG ulB = 0; ulB < length; ulB++)
			{
				IDatum *datum = (*pdrspqdatum)[ulB];
				datum->OsPrint(os);

				if (ulB < length - 1)
				{
					os << ", ";
				}
			}
			os << ")";
		}
		os << "]";
	}

	return os;
}



// EOF
