//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CLogicalExternalGet.cpp
//
//	@doc:
//		Implementation of external get
//---------------------------------------------------------------------------

#include "spqopt/operators/CLogicalExternalGet.h"

#include "spqos/base.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/base/CColRefSetIter.h"
#include "spqopt/base/CColRefTable.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "spqopt/metadata/CName.h"
#include "spqopt/metadata/CTableDescriptor.h"


using namespace spqopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp) : CLogicalGet(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp,
										 const CName *pnameAlias,
										 CTableDescriptor *ptabdesc)
	: CLogicalGet(mp, pnameAlias, ptabdesc)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp,
										 const CName *pnameAlias,
										 CTableDescriptor *ptabdesc,
										 CColRefArray *pdrspqcrOutput)
	: CLogicalGet(mp, pnameAlias, ptabdesc, pdrspqcrOutput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalExternalGet::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}
	CLogicalExternalGet *popGet = CLogicalExternalGet::PopConvert(pop);

	return Ptabdesc() == popGet->Ptabdesc() &&
		   PdrspqcrOutput()->Equals(popGet->PdrspqcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalExternalGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrspqcrOutput = NULL;
	if (must_exist)
	{
		pdrspqcrOutput =
			CUtils::PdrspqcrRemapAndCreate(mp, PdrspqcrOutput(), colref_mapping);
	}
	else
	{
		pdrspqcrOutput = CUtils::PdrspqcrRemap(mp, PdrspqcrOutput(),
											 colref_mapping, must_exist);
	}
	CName *pnameAlias = SPQOS_NEW(mp) CName(mp, Name());

	CTableDescriptor *ptabdesc = Ptabdesc();
	ptabdesc->AddRef();

	return SPQOS_NEW(mp)
		CLogicalExternalGet(mp, pnameAlias, ptabdesc, pdrspqcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalExternalGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = SPQOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfExternalGet2ExternalScan);

	return xform_set;
}

// EOF
