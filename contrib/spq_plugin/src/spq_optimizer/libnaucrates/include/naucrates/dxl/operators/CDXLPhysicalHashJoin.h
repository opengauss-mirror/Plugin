//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalHashJoin.h
//
//	@doc:
//		Class for representing DXL hashjoin operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalHashJoin_H
#define SPQDXL_CDXLPhysicalHashJoin_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysicalJoin.h"

namespace spqdxl
{
// indices of hash join elements in the children array
enum Edxlhj
{
	EdxlhjIndexProjList = 0,
	EdxlhjIndexFilter,
	EdxlhjIndexJoinFilter,
	EdxlhjIndexHashCondList,
	EdxlhjIndexHashLeft,
	EdxlhjIndexHashRight,
	EdxlhjIndexSentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalHashJoin
//
//	@doc:
//		Class for representing DXL hashjoin operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalHashJoin : public CDXLPhysicalJoin
{
private:
	// private copy ctor
	CDXLPhysicalHashJoin(const CDXLPhysicalHashJoin &);

//   m_is_set_op_join: define whether the PhysicalHashJoin is extended for Logical Intersect/Except
    BOOL m_is_set_op_join;

public:
	// ctor/dtor
    CDXLPhysicalHashJoin(CMemoryPool *mp, EdxlJoinType join_type);

	// accessors
	Edxlopid GetDXLOperator() const;

    BOOL IsSetOpJoin()
    {
        return m_is_set_op_join;
    };

    void SetOpJoin(BOOL is_set_op_join)
    {
        m_is_set_op_join = is_set_op_join;
    }

	const CWStringConst *GetOpNameStr() const;

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *dxlnode) const;

	// conversion function
	static CDXLPhysicalHashJoin *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalHashJoin == dxl_op->GetDXLOperator());
		return dynamic_cast<CDXLPhysicalHashJoin *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalHashJoin_H

// EOF
