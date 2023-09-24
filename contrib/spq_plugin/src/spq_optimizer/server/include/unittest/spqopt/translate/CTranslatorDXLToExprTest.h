//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CTranslatorDXLToExprTest.h
//
//	@doc:
//		Tests translating DXL trees into Expr tree
//
//---------------------------------------------------------------------------


#ifndef SPQOPT_CTranslatorDXLToExprTest_H
#define SPQOPT_CTranslatorDXLToExprTest_H

#include "spqos/base.h"
#include "spqos/common/CRefCount.h"
#include "spqos/io/COstreamString.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/metadata/CColumnDescriptor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpression.h"

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CTranslatorDXLToExprTest
//
//	@doc:
//		Static unit tests
//
//---------------------------------------------------------------------------

class CTranslatorDXLToExprTest
{
private:
	// translate a dxl document and check against the expected Expr tree's string representation
	static SPQOS_RESULT EresTranslateAndCheck(
		CMemoryPool *mp,
		const CHAR
			*dxl_filename,	// DXL document representing the DXL logical tree
		const CWStringDynamic
			*pstrExpQuery  // string representation of the expected query
	);

	// translate a dxl document into Expr Tree
	static CExpression *Pexpr(
		CMemoryPool *mp,  // memory pool
		const CHAR
			*dxl_filename  // DXL document representing the DXL logical tree
	);

	// generate a string representation of a given Expr tree
	static CWStringDynamic *Pstr(CMemoryPool *mp, CExpression *pexpr);

	// create a get expression for a table (r) with two integer columns
	static CExpression *PexprGet(CMemoryPool *mp);

public:
	// unittests
	static SPQOS_RESULT EresUnittest();

	// tests for supported operators
	static SPQOS_RESULT EresUnittest_MetadataColumnMapping();
	static SPQOS_RESULT EresUnittest_SingleTableQuery();
	static SPQOS_RESULT EresUnittest_SelectQuery();
	static SPQOS_RESULT EresUnittest_SelectQueryWithConst();
	static SPQOS_RESULT EresUnittest_SelectQueryWithBoolExpr();
	static SPQOS_RESULT EresUnittest_SelectQueryWithScalarOp();
	static SPQOS_RESULT EresUnittest_Limit();
	static SPQOS_RESULT EresUnittest_LimitNoOffset();
	static SPQOS_RESULT EresUnittest_ScalarSubquery();
	static SPQOS_RESULT EresUnittest_TVF();
	static SPQOS_RESULT EresUnittest_SelectQueryWithConstInList();

};	// class CTranslatorDXLToExprTest
}  // namespace spqopt

#endif	// !SPQOPT_CTranslatorDXLToExprTest_H

// EOF
