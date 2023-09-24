//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2017 Pivotal Software, Inc.
//
//	@filename:
//		CDXLScalarValuesList.h
//
//	@doc:
//		Class for representing DXL value list operator.
//---------------------------------------------------------------------------

#ifndef SPQDXL_CDXLScalarValuesList_H
#define SPQDXL_CDXLScalarValuesList_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLScalar.h"
#include "naucrates/md/IMDId.h"

namespace spqdxl
{
using namespace spqos;
using namespace spqmd;


// class for representing DXL value list operator
class CDXLScalarValuesList : public CDXLScalar
{
private:
	// private copy ctor
	CDXLScalarValuesList(CDXLScalarValuesList &);

public:
	// ctor
	CDXLScalarValuesList(CMemoryPool *mp);

	// dtor
	virtual ~CDXLScalarValuesList();

	// ident accessors
	Edxlopid GetDXLOperator() const;

	// name of the DXL operator
	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// serialize operator in DXL format
	void SerializeToDXL(CXMLSerializer *xml_serializer, const CDXLNode *dxlnode,
						const CHAR *attrname) const;

	// conversion function
	static CDXLScalarValuesList *Cast(CDXLOperator *dxl_op);

	// does the operator return a boolean result
	virtual BOOL HasBoolResult(CMDAccessor * /*md_accessor*/) const;

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *dxlnode, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl

#endif	// !SPQDXL_CDXLScalarValuesList_H

// EOF
