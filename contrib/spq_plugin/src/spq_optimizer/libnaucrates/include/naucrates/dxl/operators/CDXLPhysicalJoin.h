//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalJoin.h
//
//	@doc:
//		Base class for representing physical DXL join operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalJoin_H
#define SPQDXL_CDXLPhysicalJoin_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalJoin
//
//	@doc:
//		Base class for representing physical DXL join operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalJoin : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalJoin(const CDXLPhysicalJoin &);

	// join type (inner, outer, ...)
	EdxlJoinType m_join_type;

public:
	// ctor
	CDXLPhysicalJoin(CMemoryPool *mp, EdxlJoinType join_type);

	// join type
	EdxlJoinType GetJoinType() const;

	const CWStringConst *GetJoinTypeNameStr() const;
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalJoin_H

// EOF
