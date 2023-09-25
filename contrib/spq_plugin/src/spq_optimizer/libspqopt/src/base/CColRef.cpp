//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CColRef.cpp
//
//	@doc:
//		Implementation of column reference class
//---------------------------------------------------------------------------

#include "spqopt/base/CColRef.h"

#include "spqos/base.h"

#ifdef SPQOS_DEBUG
#include "spqos/error/CAutoTrace.h"

#include "spqopt/base/COptCtxt.h"
#endif	// SPQOS_DEBUG

using namespace spqopt;

// invalid key
const ULONG CColRef::m_ulInvalid = spqos::ulong_max;

//---------------------------------------------------------------------------
//	@function:
//		CColRef::CColRef
//
//	@doc:
//		ctor
//		takes ownership of string; verify string is properly formatted
//
//---------------------------------------------------------------------------
CColRef::CColRef(const IMDType *pmdtype, const INT type_modifier, ULONG id,
				 const CName *pname)
	: m_pmdtype(pmdtype),
	  m_type_modifier(type_modifier),
	  m_pname(pname),
	  m_used(EUnknown),
	  m_mdid_table(NULL),
	  m_id(id)
{
	SPQOS_ASSERT(NULL != pmdtype);
	SPQOS_ASSERT(pmdtype->MDId()->IsValid());
	SPQOS_ASSERT(NULL != pname);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRef::~CColRef
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CColRef::~CColRef()
{
	// we own the name
	SPQOS_DELETE(m_pname);
}


//---------------------------------------------------------------------------
//	@function:
//		CColRef::HashValue
//
//	@doc:
//		static hash function
//
//---------------------------------------------------------------------------
ULONG
CColRef::HashValue(const ULONG &ulptr)
{
	return spqos::HashValue<ULONG>(&ulptr);
}

//---------------------------------------------------------------------------
//	@function:
//		CColRef::HashValue
//
//	@doc:
//		static hash function
//
//---------------------------------------------------------------------------
ULONG
CColRef::HashValue(const CColRef *colref)
{
	ULONG id = colref->Id();
	return spqos::HashValue<ULONG>(&id);
}


FORCE_GENERATE_DBGSTR(spqopt::CColRef);

//---------------------------------------------------------------------------
//	@function:
//		CColRef::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CColRef::OsPrint(IOstream &os) const
{
	m_pname->OsPrint(os);
	os << " (" << Id() << ")";

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRef::Pdrspqul
//
//	@doc:
//		Extract array of colids from array of colrefs
//
//---------------------------------------------------------------------------
ULongPtrArray *
CColRef::Pdrspqul(CMemoryPool *mp, CColRefArray *colref_array)
{
	ULongPtrArray *pdrspqul = SPQOS_NEW(mp) ULongPtrArray(mp);
	const ULONG length = colref_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		CColRef *colref = (*colref_array)[ul];
		pdrspqul->Append(SPQOS_NEW(mp) ULONG(colref->Id()));
	}

	return pdrspqul;
}

//---------------------------------------------------------------------------
//	@function:
//		CColRef::Equals
//
//	@doc:
//		Are the two arrays of column references equivalent
//
//---------------------------------------------------------------------------
BOOL
CColRef::Equals(const CColRefArray *pdrspqcr1, const CColRefArray *pdrspqcr2)
{
	if (NULL == pdrspqcr1 || NULL == pdrspqcr2)
	{
		return (NULL == pdrspqcr1 && NULL == pdrspqcr2);
	}

	return pdrspqcr1->Equals(pdrspqcr2);
}

// check if the the array of column references are equal. Note that since we have unique
// copy of the column references, we can compare pointers.
BOOL
CColRef::Equals(const CColRef2dArray *pdrgdrspqcr1,
				const CColRef2dArray *pdrgdrspqcr2)
{
	ULONG ulLen1 = (pdrgdrspqcr1 == NULL) ? 0 : pdrgdrspqcr1->Size();
	ULONG ulLen2 = (pdrgdrspqcr2 == NULL) ? 0 : pdrgdrspqcr2->Size();

	if (ulLen1 != ulLen2)
	{
		return false;
	}

	for (ULONG ulLevel = 0; ulLevel < ulLen1; ulLevel++)
	{
		BOOL fEqual = (*pdrgdrspqcr1)[ulLevel]->Equals((*pdrgdrspqcr2)[ulLevel]);
		if (!fEqual)
		{
			return false;
		}
	}

	return true;
}
