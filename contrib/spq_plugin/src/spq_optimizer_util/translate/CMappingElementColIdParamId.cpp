//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CMappingElementColIdParamId.cpp
//
//	@doc:
//		Implementation of the functions for the mapping element between ColId
//		and ParamId during DXL->PlStmt translation
//
//	@test:
//
//
//---------------------------------------------------------------------------

#include "spq_optimizer_util/translate/CMappingElementColIdParamId.h"

using namespace spqdxl;
using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		CMappingElementColIdParamId::CMappingElementColIdParamId
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CMappingElementColIdParamId::CMappingElementColIdParamId(ULONG colid,
														 ULONG paramid,
														 IMDId *mdid,
														 INT type_modifier)
	: m_colid(colid),
	  m_paramid(paramid),
	  m_mdid(mdid),
	  m_type_modifier(type_modifier)
{
}

// EOF
