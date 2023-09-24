//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		IMDIndex.cpp
//
//	@doc:
//		Implementation of MD index
//---------------------------------------------------------------------------

#include "naucrates/md/IMDIndex.h"

#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/xml/dxltokens.h"

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
IMDIndex::GetDXLStr(EmdindexType index_type)
{
	switch (index_type)
	{
		case EmdindBtree:
			return CDXLTokens::GetDXLTokenStr(EdxltokenIndexTypeBtree);
		case EmdindBitmap:
			return CDXLTokens::GetDXLTokenStr(EdxltokenIndexTypeBitmap);
		case EmdindGist:
			return CDXLTokens::GetDXLTokenStr(EdxltokenIndexTypeGist);
		case EmdindGin:
			return CDXLTokens::GetDXLTokenStr(EdxltokenIndexTypeGin);
		default:
			SPQOS_ASSERT(!"Unrecognized index type");
			return NULL;
	}
}

// EOF
