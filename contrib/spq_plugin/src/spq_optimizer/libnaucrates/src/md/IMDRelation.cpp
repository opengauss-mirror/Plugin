//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		IMDRelation.cpp
//
//	@doc:
//		Implementation
//---------------------------------------------------------------------------

#include "naucrates/md/IMDRelation.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/xml/CXMLSerializer.h"

using namespace spqdxl;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@function:
//		IMDRelation::GetDistrPolicyStr
//
//	@doc:
//		Return relation distribution policy as a string value
//
//---------------------------------------------------------------------------
const CWStringConst *
IMDRelation::GetDistrPolicyStr(Ereldistrpolicy rel_distr_policy)
{
	switch (rel_distr_policy)
	{
		case EreldistrMasterOnly:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrMasterOnly);
		case EreldistrHash:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrHash);
		case EreldistrRandom:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrRandom);
		case EreldistrReplicated:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelDistrReplicated);
		default:
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		IMDRelation::GetStorageTypeStr
//
//	@doc:
//		Return name of storage type
//
//---------------------------------------------------------------------------
const CWStringConst *
IMDRelation::GetStorageTypeStr(IMDRelation::Erelstoragetype rel_storage_type)
{
	switch (rel_storage_type)
	{
		case ErelstorageHeap:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageHeap);
		case ErelstorageAppendOnlyCols:
			return CDXLTokens::GetDXLTokenStr(
				EdxltokenRelStorageAppendOnlyCols);
		case ErelstorageAppendOnlyRows:
			return CDXLTokens::GetDXLTokenStr(
				EdxltokenRelStorageAppendOnlyRows);
		case ErelstorageAppendOnlyParquet:
			return CDXLTokens::GetDXLTokenStr(
				EdxltokenRelStorageAppendOnlyParquet);
		case ErelstorageExternal:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageExternal);
		case ErelstorageVirtual:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageVirtual);
		case ErelstorageSentinel:
			return CDXLTokens::GetDXLTokenStr(EdxltokenRelStorageSentinel);
		default:
			return NULL;
	}
}

//---------------------------------------------------------------------------
//	@function:
//		IMDRelation::ColumnsToStr
//
//	@doc:
//		Serialize an array of column ids into a comma-separated string
//
//---------------------------------------------------------------------------
CWStringDynamic *
IMDRelation::ColumnsToStr(CMemoryPool *mp, ULongPtrArray *colid_array)
{
	CWStringDynamic *str = SPQOS_NEW(mp) CWStringDynamic(mp);

	ULONG length = colid_array->Size();
	for (ULONG ul = 0; ul < length; ul++)
	{
		ULONG id = *((*colid_array)[ul]);
		if (ul == length - 1)
		{
			// last element: do not print a comma
			str->AppendFormat(SPQOS_WSZ_LIT("%d"), id);
		}
		else
		{
			str->AppendFormat(
				SPQOS_WSZ_LIT("%d%ls"), id,
				CDXLTokens::GetDXLTokenStr(EdxltokenComma)->GetBuffer());
		}
	}

	return str;
}

// check if index is partial given its mdid
BOOL
IMDRelation::IsPartialIndex(IMDId *	 // mdid
) const
{
	return false;
}

// EOF
