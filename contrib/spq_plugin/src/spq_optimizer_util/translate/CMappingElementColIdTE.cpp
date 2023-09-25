//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMappingElementColIdTE.cpp
//
//	@doc:
//		Implementation of the functions that provide the mapping from CDXLNode to
//		Var during DXL->Query translation
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spq_optimizer_util/translate/CMappingElementColIdTE.h"

using namespace spqdxl;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMappingElementColIdTE::CMappingElementColIdTE
//
//	@doc:
//		Constructor
//
//---------------------------------------------------------------------------
CMappingElementColIdTE::CMappingElementColIdTE(ULONG colid, ULONG query_level,
											   TargetEntry *target_entry)
	: m_colid(colid), m_query_level(query_level), m_target_entry(target_entry)
{
}

// EOF
