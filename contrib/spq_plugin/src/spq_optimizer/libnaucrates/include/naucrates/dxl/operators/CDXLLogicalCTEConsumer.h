//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CDXLLogicalCTEConsumer.h
//
//	@doc:
//		Class for representing DXL logical CTE Consumer operators
//---------------------------------------------------------------------------
#ifndef SPQDXL_CDXLLogicalCTEConsumer_H
#define SPQDXL_CDXLLogicalCTEConsumer_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLLogical.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLLogicalCTEConsumer
//
//	@doc:
//		Class for representing DXL logical CTE Consumers
//
//---------------------------------------------------------------------------
class CDXLLogicalCTEConsumer : public CDXLLogical
{
private:
	// cte id
	ULONG m_id;

	// output column ids
	ULongPtrArray *m_output_colids_array;

	// private copy ctor
	CDXLLogicalCTEConsumer(CDXLLogicalCTEConsumer &);

public:
	// ctor
	CDXLLogicalCTEConsumer(CMemoryPool *mp, ULONG id,
						   ULongPtrArray *output_colids_array);

	// dtor
	virtual ~CDXLLogicalCTEConsumer();

	// operator type
	virtual Edxlopid GetDXLOperator() const;

	// operator name
	virtual const CWStringConst *GetOpNameStr() const;

	// cte identifier
	ULONG
	Id() const
	{
		return m_id;
	}

	ULongPtrArray *
	GetOutputColIdsArray() const
	{
		return m_output_colids_array;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// check if given column is defined by operator
	virtual BOOL IsColDefined(ULONG colid) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG

	// conversion function
	static CDXLLogicalCTEConsumer *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopLogicalCTEConsumer == dxl_op->GetDXLOperator());
		return dynamic_cast<CDXLLogicalCTEConsumer *>(dxl_op);
	}
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLLogicalCTEConsumer_H

// EOF
