//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMappingColIdVarPlStmt.h
//
//	@doc:
//		Class defining the functions that provide the mapping between Var, Param
//		and variables of Sub-query to CDXLNode during Query->DXL translation
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef SPQDXL_CMappingColIdVarPlStmt_H
#define SPQDXL_CMappingColIdVarPlStmt_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CHashMap.h"

#include "spq_optimizer_util/translate/CDXLTranslateContext.h"
#include "spq_optimizer_util/translate/CMappingColIdVar.h"

//fwd decl
struct Var;
struct Plan;

namespace spqdxl
{
// fwd decl
class CDXLTranslateContextBaseTable;
class CContextDXLToPlStmt;

//---------------------------------------------------------------------------
//	@class:
//		CMappingColIdVarPlStmt
//
//	@doc:
//	Class defining functions that provide the mapping between Var, Param
//	and variables of Sub-query to CDXLNode during Query->DXL translation
//
//---------------------------------------------------------------------------
class CMappingColIdVarPlStmt : public CMappingColIdVar
{
private:
	const CDXLTranslateContextBaseTable *m_base_table_context;

	// the array of translator context (one for each child of the DXL operator)
	CDXLTranslationContextArray *m_child_contexts;

	CDXLTranslateContext *m_output_context;

	// translator context used to translate initplan and subplans associated
	// with a param node
	CContextDXLToPlStmt *m_dxl_to_plstmt_context;

public:
	CMappingColIdVarPlStmt(
		CMemoryPool *mp,
		const CDXLTranslateContextBaseTable *base_table_context,
		CDXLTranslationContextArray *child_contexts,
		CDXLTranslateContext *output_context,
		CContextDXLToPlStmt *dxl_to_plstmt_context);

	// translate DXL ScalarIdent node into SPQDB Var node
	virtual Var *VarFromDXLNodeScId(const CDXLScalarIdent *dxlop);

	// translate DXL ScalarIdent node into SPQDB Param node
	Param *ParamFromDXLNodeScId(const CDXLScalarIdent *dxlop);

	// get the output translator context
	CDXLTranslateContext *GetOutputContext();

	// return the context of the DXL->PlStmt translation
	CContextDXLToPlStmt *GetDXLToPlStmtContext();
};
}  // namespace spqdxl

#endif	// SPQDXL_CMappingColIdVarPlStmt_H

// EOF
