//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CTranslatorDXLToExprUtils.h
//
//	@doc:
//		Class providing helper methods for translating from DXL to Expr
//---------------------------------------------------------------------------
#ifndef SPQOPT_CTranslatorDXLToExprUtils_H
#define SPQOPT_CTranslatorDXLToExprUtils_H

#include "spqos/base.h"

#include "spqopt/mdcache/CMDAccessor.h"
#include "spqopt/metadata/CTableDescriptor.h"
#include "spqopt/operators/CExpression.h"
#include "spqopt/operators/ops.h"
#include "naucrates/dxl/operators/CDXLColDescr.h"
#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLScalarBoolExpr.h"

namespace spqmd
{
class IMDRelation;
}

namespace spqopt
{
using namespace spqos;
using namespace spqmd;
using namespace spqdxl;

//---------------------------------------------------------------------------
//	@class:
//		CTranslatorDXLToExprUtils
//
//	@doc:
//		Class providing helper methods for translating from DXL to Expr
//
//---------------------------------------------------------------------------
class CTranslatorDXLToExprUtils
{
public:
	// create a cast expression from INT to INT8
	static CExpression *PexprConstInt8(CMemoryPool *mp,
									   CMDAccessor *md_accessor,
									   CSystemId sysid, LINT liValue);

	// create a scalar const operator from a DXL scalar const operator
	static CScalarConst *PopConst(CMemoryPool *mp, CMDAccessor *md_accessor,
								  const CDXLScalarConstValue *dxl_op);

	// create a datum from a DXL scalar const operator
	static IDatum *GetDatum(CMDAccessor *md_accessor,
							const CDXLScalarConstValue *dxl_op);

	// create a datum array from a dxl datum array
	static IDatumArray *Pdrspqdatum(CMemoryPool *mp, CMDAccessor *md_accessor,
								   const CDXLDatumArray *pdrspqdatum);

	// update table descriptor's key sets info from the MD cache object
	static void AddKeySets(CMemoryPool *mp, CTableDescriptor *ptabdesc,
						   const IMDRelation *pmdrel,
						   UlongToUlongMap *phmululColMapping);

	// check if a dxl node is a boolean expression of the given type
	static BOOL FScalarBool(const CDXLNode *dxlnode,
							EdxlBoolExprType edxlboolexprtype);

	// returns the equivalent bool expression type in the optimizer for
	// a given DXL bool expression type
	static CScalarBoolOp::EBoolOperator EBoolOperator(
		EdxlBoolExprType edxlbooltype);

	// construct a dynamic array of col refs corresponding to the given col ids
	static CColRefArray *Pdrspqcr(CMemoryPool *mp,
								 UlongToColRefMap *colref_mapping,
								 const ULongPtrArray *colids);

	// is the given expression is a scalar function that casts
	static BOOL FCastFunc(CMDAccessor *md_accessor, const CDXLNode *dxlnode,
						  IMDId *pmdidInput);
};
}  // namespace spqopt

#endif	// !SPQOPT_CTranslatorDXLToExprUtils_H

// EOF
