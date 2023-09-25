//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		CDXLScalarSortCol.h
//
//	@doc:
//		Class for representing sorting column info in DXL Sort and Motion nodes.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarSortCol_H
#define SPQDXL_CDXLScalarSortCol_H


#include "spqos/base.h"

#include "naucrates/dxl/spqdb_types.h"
#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CDXLScalarSortCol
//
//	@doc:
//		Sorting column info in DXL Sort and Motion nodes
//
//---------------------------------------------------------------------------
class CDXLScalarSortCol : public CDXLScalar
{
private:
	// id of the sorting column
	ULONG m_colid;

	// catalog Oid of the sorting operator
	IMDId *m_mdid_sort_op;

	// name of sorting operator
	CWStringConst *m_sort_op_name_str;

	// sort nulls before other values
	BOOL m_must_sort_nulls_first;

	// private copy ctor
	CDXLScalarSortCol(CDXLScalarSortCol &);

public:
	// ctor/dtor
	CDXLScalarSortCol(CMemoryPool *mp, ULONG colid, IMDId *sort_op_id,
					  CWStringConst *pstrTypeName, BOOL fSortNullsFirst);

	virtual ~CDXLScalarSortCol();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the operator
	const CWStringConst *GetOpNameStr() const;

	// Id of the sorting column
	ULONG GetColId() const;

	// mdid of the sorting operator
	IMDId *GetMdIdSortOp() const;

	// whether nulls are sorted before other values
	BOOL IsSortedNullsFirst() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *, const CDXLNode *) const;

	// conversion function
	static CDXLScalarSortCol *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopScalarSortCol == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLScalarSortCol *>(dxl_op);
	}

	// does the operator return a boolean result
	virtual BOOL
	HasBoolResult(CMDAccessor *	 //md_accessor
	) const
	{
		SPQOS_ASSERT(!"Invalid function call for this operator");
		return false;
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarSortCol_H

// EOF
