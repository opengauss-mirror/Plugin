//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CScalarProjectElement.cpp
//
//	@doc:
//		Implementation of scalar project operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarProjectElement.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"

using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CScalarProjectElement::HashValue
//
//	@doc:
//		Hash value built from colref and Eop
//
//---------------------------------------------------------------------------
ULONG
CScalarProjectElement::HashValue() const
{
	return spqos::CombineHashes(COperator::HashValue(),
							   spqos::HashPtr<CColRef>(m_pcr));
}


//---------------------------------------------------------------------------
//	@function:
//		CScalarProjectElement::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CScalarProjectElement::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		CScalarProjectElement *popScPrEl =
			CScalarProjectElement::PopConvert(pop);

		// match if column reference is same
		return Pcr() == popScPrEl->Pcr();
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarProjectElement::FInputOrderSensitive
//
//	@doc:
//		Not called for leaf operators
//
//---------------------------------------------------------------------------
BOOL
CScalarProjectElement::FInputOrderSensitive() const
{
	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarProjectElement::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CScalarProjectElement::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	ULONG id = m_pcr->Id();
	CColRef *colref = colref_mapping->Find(&id);
	if (NULL == colref)
	{
		if (must_exist)
		{
			// not found in hashmap, so create a new colref and add to hashmap
			CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();

			CName name(m_pcr->Name());
			colref = col_factory->PcrCreate(m_pcr->RetrieveType(),
											m_pcr->TypeModifier(), name);

#ifdef SPQOS_DEBUG
			BOOL result =
#endif	// SPQOS_DEBUG
				colref_mapping->Insert(SPQOS_NEW(mp) ULONG(id), colref);
			SPQOS_ASSERT(result);
		}
		else
		{
			colref = m_pcr;
		}
	}

	return SPQOS_NEW(mp) CScalarProjectElement(mp, colref);
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarProjectElement::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CScalarProjectElement::OsPrint(IOstream &os) const
{
	os << SzId() << " ";
	m_pcr->OsPrint(os);

	return os;
}

// EOF
