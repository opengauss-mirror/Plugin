//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		CDXLPhysicalAgg.h
//
//	@doc:
//		Class for representing DXL aggregate operators.
//---------------------------------------------------------------------------



#ifndef SPQDXL_CDXLPhysicalAgg_H
#define SPQDXL_CDXLPhysicalAgg_H

#include "spqos/base.h"

#include "naucrates/dxl/operators/CDXLNode.h"
#include "naucrates/dxl/operators/CDXLPhysical.h"

namespace spqdxl
{
// indices of group by elements in the children array
enum Edxlagg
{
	EdxlaggIndexProjList = 0,
	EdxlaggIndexFilter,
	EdxlaggIndexChild,
	EdxlaggIndexSentinel
};

enum EdxlAggStrategy
{
	EdxlaggstrategyPlain,
	EdxlaggstrategySorted,
	EdxlaggstrategyHashed,
	EdxlaggstrategySentinel
};

//---------------------------------------------------------------------------
//	@class:
//		CDXLPhysicalAgg
//
//	@doc:
//		Class for representing DXL aggregate operators
//
//---------------------------------------------------------------------------
class CDXLPhysicalAgg : public CDXLPhysical
{
private:
	// private copy ctor
	CDXLPhysicalAgg(const CDXLPhysicalAgg &);

	// grouping column ids
	ULongPtrArray *m_grouping_colids_array;

	EdxlAggStrategy m_dxl_agg_strategy;

	// is it safe to stream the local hash aggregate
	BOOL m_stream_safe;

	// serialize output grouping columns indices in DXL
	void SerializeGroupingColsToDXL(CXMLSerializer *xml_serializer) const;

public:
	// ctor
	CDXLPhysicalAgg(CMemoryPool *mp, EdxlAggStrategy dxl_agg_strategy,
					BOOL stream_safe);

	// dtor
	virtual ~CDXLPhysicalAgg();

	// accessors
	Edxlopid GetDXLOperator() const;
	EdxlAggStrategy GetAggStrategy() const;

	const CWStringConst *GetOpNameStr() const;
	const CWStringConst *GetAggStrategyNameStr() const;
	const CWStringConst *PstrAggLevel() const;
	const ULongPtrArray *GetGroupingColidArray() const;

	// set grouping column indices
	void SetGroupingCols(ULongPtrArray *);

	// is aggregate a hash aggregate that it safe to stream
	BOOL
	IsStreamSafe() const
	{
		return (EdxlaggstrategyHashed == m_dxl_agg_strategy) && m_stream_safe;
	}

	// serialize operator in DXL format
	virtual void SerializeToDXL(CXMLSerializer *xml_serializer,
								const CDXLNode *node) const;

	// conversion function
	static CDXLPhysicalAgg *
	Cast(CDXLOperator *dxl_op)
	{
		SPQOS_ASSERT(NULL != dxl_op);
		SPQOS_ASSERT(EdxlopPhysicalAgg == dxl_op->GetDXLOperator());

		return dynamic_cast<CDXLPhysicalAgg *>(dxl_op);
	}

#ifdef SPQOS_DEBUG
	// checks whether the operator has valid structure, i.e. number and
	// types of child nodes
	void AssertValid(const CDXLNode *, BOOL validate_children) const;
#endif	// SPQOS_DEBUG
};
}  // namespace spqdxl
#endif	// !SPQDXL_CDXLPhysicalAgg_H

// EOF
