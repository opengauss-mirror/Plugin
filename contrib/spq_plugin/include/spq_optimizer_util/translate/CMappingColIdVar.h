//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CMappingColIdVar.h
//
//	@doc:
//		Abstract base class of ColId to VAR mapping when translating CDXLScalars
//		If we need a CDXLScalar translator during DXL->PlStmt or DXL->Query translation
//		we implement a colid->Var mapping for PlStmt or Query respectively that
//		is derived from this interface.
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_CMappingColIdVar_H
#define SPQDXL_CMappingColIdVar_H

#include "spqos/base.h"

//fwd decl
struct Var;
struct Param;
struct PlannedStmt;
struct Query;

namespace spqdxl
{
using namespace spqos;

// fwd decl
class CDXLScalarIdent;

//---------------------------------------------------------------------------
//	@class:
//		CMappingColIdVar
//
//	@doc:
//		Class providing the interface for ColId in CDXLScalarIdent to Var
//		mapping when translating CDXLScalar nodes
//
//---------------------------------------------------------------------------
class CMappingColIdVar
{
protected:
	// memory pool
	CMemoryPool *m_mp;

public:
	// ctor/dtor
	explicit CMappingColIdVar(CMemoryPool *);

	virtual ~CMappingColIdVar()
	{
	}

	// translate DXL ScalarIdent node into SPQDB Var node
	virtual Var *VarFromDXLNodeScId(const CDXLScalarIdent *) = 0;
};
}  // namespace spqdxl

#endif	//SPQDXL_CMappingColIdVar_H

// EOF
