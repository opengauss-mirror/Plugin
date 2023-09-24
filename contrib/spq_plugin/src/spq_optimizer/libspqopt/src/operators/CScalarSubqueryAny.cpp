//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CScalarSubqueryAny.cpp
//
//	@doc:
//		Implementation of scalar subquery ANY operator
//---------------------------------------------------------------------------

#include "spqopt/operators/CScalarSubqueryAny.h"

#include "spqos/base.h"

#include "spqopt/base/CUtils.h"

using namespace spqopt;

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryAny::CScalarSubqueryAny
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CScalarSubqueryAny::CScalarSubqueryAny(CMemoryPool *mp, IMDId *scalar_op_mdid,
									   const CWStringConst *pstrScalarOp,
									   const CColRef *colref)
	: CScalarSubqueryQuantified(mp, scalar_op_mdid, pstrScalarOp, colref)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CScalarSubqueryAny::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CScalarSubqueryAny::PopCopyWithRemappedColumns(CMemoryPool *mp,
											   UlongToColRefMap *colref_mapping,
											   BOOL must_exist)
{
	CColRef *colref = CUtils::PcrRemap(Pcr(), colref_mapping, must_exist);

	IMDId *scalar_op_mdid = MdIdOp();
	scalar_op_mdid->AddRef();

	CWStringConst *pstrScalarOp =
		SPQOS_NEW(mp) CWStringConst(mp, PstrOp()->GetBuffer());

	return SPQOS_NEW(mp)
		CScalarSubqueryAny(mp, scalar_op_mdid, pstrScalarOp, colref);
}

// EOF
